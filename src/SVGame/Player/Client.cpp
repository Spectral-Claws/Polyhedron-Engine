/*
Copyright (C) 1997-2001 Id Software, Inc.
Copyright (C) 2019, NVIDIA CORPORATION. All rights reserved.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
// General.
#include "../ServerGameLocal.h" // SVGame.
#include "../ChaseCamera.h"     // Chase Camera.
#include "../Effects.h"         // Effects.
#include "../Entities.h"        // Entities.
#include "../Utilities.h"       // Util funcs.
#include "Client.h"             // Include Player Client header.
#include "Hud.h"                // Include HUD header.
#include "View.h"               // View header.
#include "Weapons.h"

// ClassEntities.
#include "../Entities/Base/SVGBasePlayer.h"
#include "../Entities/Info/InfoPlayerStart.h"

// Game modes.
#include "../Gamemodes/IGamemode.h"

// World.
#include "../World/Gameworld.h"

// Shared Game.
#include "SharedGame/SharedGame.h" // Include SG Base.
#include "SharedGame/PMove.h"   // Include SG PMove.
#include "Animations.h"         // Include Player Client Animations.


//===============
// ClientUserInfoChanged
//
// called whenever the player updates a userinfo variable.
//
// The game can override any of the settings in place
// (forcing skins or names, etc) before copying it off.
//================
void SVG_ClientUserinfoChanged(Entity* ent, char* userinfo) {
    if (!ent)
        return;

    GetGamemode()->ClientUserinfoChanged(ent, userinfo);
}

//=======================================================================

// TODO: Move into game mode.
void SVG_TossClientWeapon(SVGBasePlayer *player)
{
    SVGBaseItemWeapon *item = nullptr;
    Entity      *drop= nullptr;
    float       spread = 1.5f;

    // Always allow.
    //if (!deathmatch->value)
    //    return;

    // Sanity check.
    if (!player) {
        return;
    }

    // Get client
    ServerClient* client = player->GetClient();

    // Sanity check.
    if (!client) {
        return;
    }

    // Fetch active weapon, if any.
    //item = player->GetActiveWeapon();

    //if (!player->GetClient()->persistent.inventory[player->GetClient()->ammoIndex])
    //    item = NULL;
    //if (item && (strcmp(item->pickupName, "Blaster") == 0))
    //    item = NULL;

    if (item) {
        //playerClient->GetClient()->aimAngles[vec3_t::Yaw] -= spread;
        //drop = SVG_DropItem(playerClient->GetServerEntity(), item);
        //playerClient->GetClient()->aimAngles[vec3_t::Yaw] += spread;
        //drop->spawnFlags = ItemSpawnFlags::DroppedPlayerItem;
    }
}

//=======================================================================

/*
* only called when persistent.isSpectator changes
* note that resp.isSpectator should be the opposite of persistent.isSpectator here
*/
void spectator_respawn(Entity *ent)
{
    int i, numspec;

    // If the user wants to become a isSpectator, make sure he doesn't
    // exceed max_spectators
    if (ent->client->persistent.isSpectator) {
        // Test if the isSpectator password was correct, if not, error and return.
        char *value = Info_ValueForKey(ent->client->persistent.userinfo, "isspectator");
        if (*spectator_password->string &&
            strcmp(spectator_password->string, "none") &&
            strcmp(spectator_password->string, value)) {
            // Report error message by centerprinting it to client.
            gi.CPrintf(ent, PRINT_HIGH, "Spectator password incorrect.\n");

            // Enable isSpectator state.
            ent->client->persistent.isSpectator = false;

            // Let the client go out of its isSpectator mode by using a StuffCmd.
            gi.StuffCmd(ent, "isspectator 0\n");
            return;
        }

        // Count actual active spectators
        for (i = 1, numspec = 0; i <= maximumclients->value; i++)
            if (game.world->GetServerEntities()[i].inUse && game.world->GetServerEntities()[i].client->persistent.isSpectator)
                numspec++;

        if (numspec >= maxspectators->value) {
            // Report error message by centerprinting it to client.
            gi.CPrintf(ent, PRINT_HIGH, "Server isSpectator limit is full.\n");

            // Enable isSpectator state.
            ent->client->persistent.isSpectator = false;

            // Let the client go out of its isSpectator mode by using a StuffCmd.
            gi.StuffCmd(ent, "isspectator 0\n");
            return;
        }
    } else {
        // He was a isSpectator and wants to join the game 
        // He must have the right password
        // Test if the isSpectator password was correct, if not, error and return.
        char *value = Info_ValueForKey(ent->client->persistent.userinfo, "password");
        if (*password->string && strcmp(password->string, "none") &&
            strcmp(password->string, value)) {
            // Report error message by centerprinting it to client.
            gi.CPrintf(ent, PRINT_HIGH, "Password incorrect.\n");

            // Enable isSpectator state.
            ent->client->persistent.isSpectator = true;

            // Let the client go in its isSpectator mode by using a StuffCmd.
            gi.StuffCmd(ent, "isspectator 1\n");
            return;
        }
    }

    // clear client on respawn
    ent->client->respawn.score = ent->client->persistent.score = 0;

    ent->serverFlags &= ~EntityServerFlags::NoClient;
    GetGamemode()->PlacePlayerInGame(dynamic_cast<SVGBasePlayer*>(ent->classEntity));

    // add a teleportation effect
    if (!ent->client->persistent.isSpectator)  {
        // send effect
        gi.MSG_WriteUint8(ServerGameCommand::MuzzleFlash);//WriteByte(ServerGameCommand::MuzzleFlash);
        gi.MSG_WriteInt16(ent - game.world->GetServerEntities());//WriteShort(ent - game.world->GetServerEntities());
        gi.MSG_WriteUint8(MuzzleFlashType::Login);//WriteByte(MuzzleFlashType::Login);
        gi.Multicast(ent->state.origin, Multicast::PVS);

        // hold in place briefly
        ent->client->playerState.pmove.flags = PMF_TIME_TELEPORT;
        ent->client->playerState.pmove.time = 14;
    }

    ent->client->respawnTime = level.time;

    if (ent->client->persistent.isSpectator)
        gi.BPrintf(PRINT_HIGH, "%s has moved to the sidelines\n", ent->client->persistent.netname);
    else
        gi.BPrintf(PRINT_HIGH, "%s joined the game\n", ent->client->persistent.netname);
}

//==============================================================

/*
===========
ClientBegin

called when a client has finished connecting, and is ready
to be placed into the game.  This will happen every level load.
============
*/
extern void DebugShitForEntitiesLulz();

void SVG_ClientBegin(Entity *ent)
{
    // Fetch this entity's client.
    ent->client = game.GetClients() + (ent - game.world->GetServerEntities() - 1);

    // Let the game mode decide from here on out.
    GetGamemode()->ClientBegin(ent);
    //GetGamemode()->ClientEndServerFrame(ent);
}


/*
===========
ClientConnect

Called when a player begins connecting to the server.
The game can refuse entrance to a client by returning false.
If the client is allowed, the connection process will continue
and eventually get to ClientBegin()
Changing levels will NOT cause this to be called again, but
loadgames will.
============
*/
qboolean SVG_ClientConnect(Entity *ent, char *userinfo)
{
    return GetGamemode()->ClientConnect(ent, userinfo);
}

/*
===========
ClientDisconnect

Called when a player drops from the server.
Will not be called between levels.
============
*/
void SVG_ClientDisconnect(Entity *ent)
{
    // Ensure this entity has a client.
    if (!ent->client)
        return;
    // Ensure it has a class entity also.
    if (!ent->classEntity)
        return;

    // Since it does, we pass it on to the game mode.
    GetGamemode()->ClientDisconnect(dynamic_cast<SVGBasePlayer*>(ent->classEntity), ent->client);

    // FIXME: don't break skins on corpses, etc
    //int32_t playernum = ent-g_entities-1;
    //gi.configstring (ConfigStrings::PlayerSkins+playernum, "");
}

/*
==============
ClientThink

This will be called once for each client frame, which will
usually be a couple times for each server frame.
==============
*/
void SVG_ClientThink(Entity *svEntity, ClientMoveCommand *moveCommand)
{
    // Acquire player entity pointer.
    SVGBaseEntity *validEntity = Gameworld::ValidateEntity(svEntity, true, true);

    // Sanity check.
    if (!validEntity || !validEntity->IsSubclassOf<SVGBasePlayer>()) {
        gi.DPrintf("Warning: ClientThink called on svEntity(#%i) without a SVGBasePlayer or derivate class entity.\n", svEntity->state.number);
        return;
    }

    // Save to cast now.
    SVGBasePlayer *player = dynamic_cast<SVGBasePlayer*>(validEntity);

    // We can safely acquire client entity since it's already been sanitized by GetPlayerClassEntity.
    ServerClient *client = player->GetClient();

    // Do client think.
    GetGameworld()->GetGamemode()->ClientThink(player, client, moveCommand);

    // update chase cam if being followed
    //for (int i = 1; i <= maximumclients->value; i++) {
    //    other = game.world->GetServerEntities() + i;
    //    if (other->inUse && other->client->chaseTarget == serverEntity)
    //        SVG_UpdateChaseCam(playerEntity);
    //}
}