#include "Stalker.h"

static struct victim lonlyVictim;

static bool isChannelMaxReached;
static bool not_enough_rights;
static struct TS3Functions ts3Functions;
static char *pluginID;

void initStalker(const struct TS3Functions &_ts3Functions) {

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
		ts3Functions.printMessageToCurrentTab("Victim's new channel is passworded, you can't follow him.");
		return;
	}
	//char returnCode[RETURNCODE_BUFSIZE];

	//ts3Functions.createReturnCode(getPluginID() , returnCode, RETURNCODE_BUFSIZE);
	
	if (ts3Functions.requestClientMove(schID, myID, lonlyVictim.victimChannelID, "", "stalk") == 777U) {
		
	}
	
	/*
	Sleep(1000);

	printf(std::to_string(error).c_str());

	if (error == 777) {
		ts3Functions.printMessageToCurrentTab("Channel Max Clients Reached");
		isChannelMaxReached = true;
	}

	*/

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
	if (isChannelMaxReached && (movedID != lonlyVictim.victimID)) {
		if (lonlyVictim.victimChannelID == oldChannelID) {
			isChannelMaxReached = false;
		}
	}
	if (isChannelMaxReached) {
		return;
	}
	std::thread mythread(Join_Behind, schID);
	mythread.detach();
	

}


void kickedFromServer(uint64 schID, anyID kickedID) {
	if (lonlyVictim.active == false) return;
	if (schID != lonlyVictim.schID) return;

	if (kickedID == lonlyVictim.victimID) {
		deleteVictim();
		printf("Your victim was kicked from the Server, stopping!");
	}
}
void bannedFromServer(uint64 schID, anyID kickedID) {
	if (lonlyVictim.active == false) return;
	if (schID != lonlyVictim.schID) return;

	if (kickedID == lonlyVictim.victimID) {
		deleteVictim();
		printf("Your victim was banned from the Server, stopping!");
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
		ts3Functions.printMessageToCurrentTab("You dont have a Victim");
	}
	else {
		std::string output;
		output += "Your Victim: ";
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

int ts3plugin_onServerErrorEvent(uint64 serverConnectionHandlerID, const char* errorMessage, unsigned int error, const char* returnCode, const char* extraMessage) {
	std::cout << returnCode << "test1" << error << std::endl;

	if (std::string::npos != (std::string(returnCode).find( "stalk"))) {

		if (error == 777U) {

			ts3Functions.printMessageToCurrentTab("Channel Max Clients Reached");
			isChannelMaxReached = true;
		}

		/* A plugin could now check the returnCode with previously (when calling a function) remembered returnCodes and react accordingly */
		/* In case of using a a plugin return code, the plugin can return:
		* 0: Client will continue handling this error (print to chat tab)
		* 1: Client will ignore this error, the plugin announces it has handled it */
		return 1;
	}
	return 0;  /* If no plugin return code was used, the return value of this function is ignored */
}

void ts3plugin_onUpdateChannelEditedEvent(uint64 serverConnectionHandlerID, uint64 channelID, anyID invokerID, const char* invokerName, const char* invokerUniqueIdentifier) {

	if (serverConnectionHandlerID != lonlyVictim.schID) return;

	if (channelID == lonlyVictim.victimChannelID) {

		Join_Behind(serverConnectionHandlerID);
		isChannelMaxReached = false;
		not_enough_rights = false;
	}

}

