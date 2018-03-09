#pragma once
#include "ts3_functions.h"
#include "plugin_definitions.h"
#include "teamspeak/public_rare_definitions.h"
#include "plugin.h"
#include <Windows.h>
#include <string>

struct victim {
	uint64 schID;
	anyID victimID;
	uint64 victimChannelID;
	bool active;
};
void initTS3FuntkionPointer(const struct TS3Functions &_ts3Functions);
void newVictim(uint64 schID, anyID victimID);
void deleteVictim();
void moveevent(uint64 schID, anyID movedID, uint64 oldChannelID,uint64 newChannelID);
void bannedFromServer(uint64 schID, anyID kickedID);
void kickedFromServer(uint64 schID, anyID kickedID);
void kickedFromChannel(uint64 schID, anyID kickedID);
void whereIsMyVictim();

