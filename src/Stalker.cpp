#include "Stalker.h"

static struct victim lonlyVictim;

static bool isChannelMaxReached;
static bool not_enough_rights;
static struct TS3Functions ts3Functions;

void initTS3FuntkionPointer(const struct TS3Functions &_ts3Functions) {
	ts3Functions = _ts3Functions;
}


void newVictim(uint64 schID,anyID victimID) {
	deleteVictim();
	ts3Functions.getChannelOfClient(schID, victimID, &lonlyVictim.victimChannelID);
	lonlyVictim.schID = schID;
	lonlyVictim.victimID = victimID;
	lonlyVictim.victimChannelID;
	lonlyVictim.active = true;
}

void deleteVictim() {
	lonlyVictim.schID = 0;
	lonlyVictim.victimID = 0;
	lonlyVictim.victimChannelID = 0;
	lonlyVictim.active = false;
}


void getClientIdLink(uint64 serverConnectionHandlerID, anyID clientID, std::string &clientLink) {
	std::string cClientID = std::to_string(clientID);

	char *clientUid;
	char *username;

	if (clientID == 0) {
		ts3Functions.getServerVariableAsString(serverConnectionHandlerID, VIRTUALSERVER_NAME, &username);
		clientUid = "";
	}
	else {
		ts3Functions.getClientVariableAsString(serverConnectionHandlerID, clientID, CLIENT_UNIQUE_IDENTIFIER, &clientUid);
		ts3Functions.getClientVariableAsString(serverConnectionHandlerID, clientID, CLIENT_NICKNAME, &username);
	}

	clientLink = "[color=firebrick][URL=client://" + cClientID + "/" + clientUid + "]\"" + username + "\"[/URL][/color]";
}

void Join_Behind(uint64 schID) {

	anyID myID;
	ts3Functions.getClientID(schID, &myID);

	uint64 myChannelID;
	ts3Functions.getChannelOfClient(schID, myID, &myChannelID);


	if (myChannelID == lonlyVictim.victimChannelID) {
		return;
	}

	int pw;
	ts3Functions.getChannelVariableAsInt(schID, lonlyVictim.victimChannelID,CHANNEL_FLAG_PASSWORD, &pw);
	if (pw == 1) {
		ts3Functions.printMessageToCurrentTab("Channel of the victim has an password not possible to join");
		return;
	}
	

	int error = ts3Functions.requestClientMove(schID, myID, lonlyVictim.victimChannelID, "", NULL);

	if (error == 0x0309) {
		ts3Functions.printMessageToCurrentTab("Channel Max Clients Reached");
		isChannelMaxReached = true;
	}



}


void moveevent(uint64 schID, anyID movedID,uint64 oldChannelID,uint64 newChannelID) {
	if (lonlyVictim.active == false) return;
	if (schID != lonlyVictim.schID) return;

	if (newChannelID == 0 && movedID == lonlyVictim.victimID){
		deleteVictim();
	}

	if (movedID == lonlyVictim.victimID) {
		lonlyVictim.victimChannelID = newChannelID;
		isChannelMaxReached = false;
		not_enough_rights = false;
	}

	if (isChannelMaxReached) {
		if (lonlyVictim.victimChannelID == oldChannelID) {
			isChannelMaxReached = false;
		}
	}

	Join_Behind(schID);

}


void kickedFromServer(uint64 schID, anyID kickedID) {
	if (lonlyVictim.active == false) return;
	if (schID != lonlyVictim.schID) return;

	if (kickedID == lonlyVictim.victimID) {
		deleteVictim();
		printf("Your victim was kicked out of the Server, the ID get resetted");
	}
}
void bannedFromServer(uint64 schID, anyID kickedID) {
	if (lonlyVictim.active == false) return;
	if (schID != lonlyVictim.schID) return;

	if (kickedID == lonlyVictim.victimID) {
		deleteVictim();
		printf("Your victim was kicked out of the Server, the ID get resetted");
	}
}

void kickedFromChannel(uint64 schID,anyID kickedID) {
	if (lonlyVictim.active == false) return;

	if (schID != lonlyVictim.schID) return;

	if (kickedID == lonlyVictim.victimID) {
		Join_Behind(schID);
		printf("Your victim was kicked out of his channel");
	}
}

void whereIsMyVictim() {
	if (lonlyVictim.active == false) {
		ts3Functions.printMessageToCurrentTab("you dont have a Victim");
	}
	else {
		std::string output;
		output += "your Victim: ";
		std::string buf1;
		getClientIdLink(lonlyVictim.schID, lonlyVictim.victimID, buf1);
		output += buf1;
		output += " is in the Channel: ";
		char *buf2;
		ts3Functions.getChannelVariableAsString(lonlyVictim.schID, lonlyVictim.victimChannelID, CHANNEL_NAME, &buf2);
		output += buf2;
		output += " on the Server: ";
		char *buf3;
		ts3Functions.getServerVariableAsString(lonlyVictim.schID, VIRTUALSERVER_NAME, &buf3);
		output += buf3;
		ts3Functions.printMessageToCurrentTab(output.c_str());
	}
}


anyID getIDofUID(uint64 schID, std::string UID ) {
	uint64 numberOfClients = 0;

	ts3Functions.requestServerVariables(schID);
	
	ts3Functions.getServerVariableAsUInt64(schID, VIRTUALSERVER_CLIENTS_ONLINE, &numberOfClients);


	anyID *allClientIDs = new anyID[numberOfClients];
	ts3Functions.getClientList(schID, &allClientIDs);

	for (int i = 0; i < numberOfClients; i++) {

		char *buffer;
		ts3Functions.getClientVariableAsString(schID, allClientIDs[i], CLIENT_UNIQUE_IDENTIFIER, &buffer);
		if (UID.compare(buffer))
			return allClientIDs[i];
	
	}
	delete[] allClientIDs;
	return 0;
}





