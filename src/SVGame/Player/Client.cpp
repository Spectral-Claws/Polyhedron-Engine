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

    game.GetGamemode()->ClientUserinfoChanged(ent, userinfo);
}

//=======================================================================

// TODO: Move into game mode.
void SVG_TossClientWeapon(SVGBasePlayer *player)
{
    SVGBaseItemWeapon *item;
    Entity      *drop;
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
    game.GetGamemode()->PlacePlayerInGame(dynamic_cast<SVGBasePlayer*>(ent->classEntity));

    // add a teleportation effect
    if (!ent->client->persistent.isSpectator)  {
        // send effect
        gi.WriteByte(ServerGameCommand::MuzzleFlash);
        gi.WriteShort(ent - game.world->GetServerEntities());
        gi.WriteByte(MuzzleFlashType::Login);
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
    game.GetGamemode()->ClientBegin(ent);
    //game.GetGamemode()->ClientEndServerFrame(ent);
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
    return game.GetGamemode()->ClientConnect(ent, userinfo);
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
    game.GetGamemode()->ClientDisconnect(dynamic_cast<SVGBasePlayer*>(ent->classEntity), ent->client);

    // FIXME: don't break skins on corpses, etc
    //int32_t playernum = ent-g_entities-1;
    //gi.configstring (ConfigStrings::PlayerSkins+playernum, "");
}


//==============================================================
Entity *pm_passent;

// pmove doesn't need to know about passent and contentmask
trace_t q_gameabi PM_Trace(const vec3_t &start, const vec3_t &mins, const vec3_t &maxs, const vec3_t &end)
{
    if (pm_passent->classEntity && pm_passent->classEntity->GetHealth() > 0)
        return gi.Trace(start, mins, maxs, end, pm_passent, CONTENTS_MASK_PLAYERSOLID);
    else
        return gi.Trace(start, mins, maxs, end, pm_passent, CONTENTS_MASK_DEADSOLID);
}

unsigned CheckBlock(void *b, int c)
{
    int v, i;
    v = 0;
    for (i = 0 ; i < c ; i++)
        v += ((byte *)b)[i];
    return v;
}
void PrintPMove(PlayerMove *pm)
{
    unsigned    c1, c2;

    c1 = CheckBlock(&pm->state, sizeof(pm->state));
    c2 = CheckBlock(&pm->moveCommand, sizeof(pm->moveCommand));
    Com_Printf("sv %3i:%i %i\n", pm->moveCommand.input.impulse, c1, c2);
}
//==============================================================

/*
==============
ClientThink

This will be called once for each client frame, which will
usually be a couple times for each server frame.
==============
*/
void SVG_ClientThink(Entity *serverEntity, ClientMoveCommand *moveCommand)
{
    ServerClient* client = nullptr;
    SVGBasePlayer *classEntity = nullptr;
    Entity* other = nullptr;

    PlayerMove pm = {};

    // Sanity checks.
    if (!serverEntity) {
        Com_Error(ErrorType::ERR_DROP, "%s: has a NULL *ent!\n", __FUNCTION__);
    }
    if (!serverEntity->client)
        Com_Error(ErrorType::ERR_DROP, "%s: *ent has no client to think with!\n", __FUNCTION__);

    if (!serverEntity->classEntity)
        return;

    // Store the current entity to be run from SVG_RunFrame.
    level.currentEntity = serverEntity->classEntity;

    // Fetch the entity client.
    client = serverEntity->client;

    // Fetch the class entity.
    classEntity = (SVGBasePlayer*)serverEntity->classEntity;

    if (level.intermission.time) {
        client->playerState.pmove.type = EnginePlayerMoveType::Freeze;
        // can exit intermission after five seconds
        if (level.time > level.intermission.time + 5.0
            && (moveCommand->input.buttons & ButtonBits::Any))
            level.intermission.exitIntermission = true;
        return;
    }

    pm_passent = serverEntity;

    if (client->chaseTarget) {
        // Angles are fetched from the client we are chasing.
        client->respawn.commandViewAngles[0] = moveCommand->input.viewAngles[0];
        client->respawn.commandViewAngles[1] = moveCommand->input.viewAngles[1];
        client->respawn.commandViewAngles[2] = moveCommand->input.viewAngles[2];
    } else {

        // set up for pmove
        memset(&pm, 0, sizeof(pm));

        if ( classEntity->GetMoveType() == MoveType::NoClip )
            client->playerState.pmove.type = PlayerMoveType::Noclip;
        else if ( classEntity->GetMoveType() == MoveType::Spectator )
            client->playerState.pmove.type = PlayerMoveType::Spectator;
        else if (classEntity->GetModelIndex() != 255 )
            client->playerState.pmove.type = EnginePlayerMoveType::Gib;
        else if ( classEntity->GetDeadFlag() )
            client->playerState.pmove.type = EnginePlayerMoveType::Dead;
        else
            client->playerState.pmove.type = PlayerMoveType::Normal;

        client->playerState.pmove.gravity = sv_gravity->value;

        // Copy over the latest playerstate its pmove state.
        pm.state = client->playerState.pmove;

        // Move over entity state values into the player move state so it is up to date.
        pm.state.origin = classEntity->GetOrigin();
        pm.state.velocity = classEntity->GetVelocity();
        pm.moveCommand = *moveCommand;
        if (classEntity->GetGroundEntity())
            pm.groundEntityPtr = classEntity->GetGroundEntity()->GetServerEntity();
        else
            pm.groundEntityPtr = nullptr;

        pm.Trace = PM_Trace;
        pm.PointContents = gi.PointContents;

        // perform a pmove
        PMove(&pm);

        // Save client pmove results.
        client->playerState.pmove = pm.state;

        // Move over needed results to the entity and its state.
        classEntity->SetOrigin(pm.state.origin);
        classEntity->SetVelocity(pm.state.velocity);
        classEntity->SetMins(pm.mins);
        classEntity->SetMaxs(pm.maxs);
        classEntity->SetViewHeight(pm.state.viewOffset[2]);
        classEntity->SetWaterLevel(pm.waterLevel);
        classEntity->SetWaterType(pm.waterType);

        // Check for jumping sound.
        if (classEntity->GetGroundEntity() && !pm.groundEntityPtr && (pm.moveCommand.input.upMove >= 10) && (pm.waterLevel == 0)) {
            gi.Sound(serverEntity, CHAN_VOICE, gi.SoundIndex("*jump1.wav"), 1, ATTN_NORM, 0);
            SVG_PlayerNoise(classEntity, classEntity->GetOrigin(), PNOISE_SELF);
        }

        if (pm.groundEntityPtr)
            classEntity->SetGroundEntity(pm.groundEntityPtr->classEntity);
        else
            classEntity->SetGroundEntity(nullptr);

        // Copy over the user command angles so they are stored for respawns.
        // (Used when going into a new map etc.)
        client->respawn.commandViewAngles[0] = moveCommand->input.viewAngles[0];
        client->respawn.commandViewAngles[1] = moveCommand->input.viewAngles[1];
        client->respawn.commandViewAngles[2] = moveCommand->input.viewAngles[2];

        // Store entity link count in case we have a ground entity pointer.
        if (pm.groundEntityPtr)
            classEntity->SetGroundEntityLinkCount(pm.groundEntityPtr->linkCount);

        // Special treatment for angles in case we are dead. Target the killer entity yaw angle.
        if (classEntity->GetDeadFlag()) {
            client->playerState.pmove.viewAngles[vec3_t::Roll] = 40;
            client->playerState.pmove.viewAngles[vec3_t::Pitch] = -15;
            client->playerState.pmove.viewAngles[vec3_t::Yaw] = client->killerYaw;
        } else {
            // Otherwise, store the resulting view angles accordingly.
            client->aimAngles = pm.viewAngles;
            client->playerState.pmove.viewAngles = pm.viewAngles;
        }

        // Link it back in for collision testing.
        classEntity->LinkEntity();

        // Only check for trigger and object touches if not one of these movetypes.
        if (classEntity->GetMoveType() != MoveType::NoClip && classEntity->GetMoveType() != MoveType::Spectator)
            UTIL_TouchTriggers(classEntity);

        // Touch other objects
        int32_t i = 0;
        int32_t j = 0;
        for (i = 0 ; i < pm.numTouchedEntities; i++) {
            other = pm.touchedEntities[i];
            for (j = 0 ; j < i ; j++)
                if (pm.touchedEntities[j] == other)
                    break;
            if (j != i)
                continue;   // duplicated
                            //if (!other->Touch)
                            //    continue;
                            //other->Touch(other, ent, NULL, NULL);
            if (!other->classEntity)
                continue;
            other->classEntity->Touch(other->classEntity, classEntity, NULL, NULL);
        }

    }

    client->oldButtons = client->buttons;
    client->buttons = moveCommand->input.buttons;
    client->latchedButtons |= client->buttons & ~client->oldButtons;

    // save light level the player is standing on for
    // monster sighting AI
    //ent->lightLevel = moveCommand->input.lightLevel;

    // fire weapon from final position if needed
    if (client->latchedButtons & ButtonBits::Attack) {
        if (client->respawn.isSpectator) {

            client->latchedButtons = 0;

            if (client->chaseTarget) {
                client->chaseTarget = NULL;
                client->playerState.pmove.flags &= ~PMF_NO_PREDICTION;
            } else
                SVG_GetChaseTarget(classEntity);

        } else {// if (!client->weaponState.shouldThink) {
            //client->weaponState.shouldThink = true;
            SVG_ThinkWeapon(classEntity);
        }
    } else {
	    if (!client->respawn.isSpectator) {
	        SVG_ThinkWeapon(classEntity);
	    }
    }

    if (client->respawn.isSpectator) {
        if (moveCommand->input.upMove >= 10) {
            if (!(client->playerState.pmove.flags & PMF_JUMP_HELD)) {
                client->playerState.pmove.flags |= PMF_JUMP_HELD;
                if (client->chaseTarget)
                    SVG_ChaseNext(classEntity);
                else
                    SVG_GetChaseTarget(classEntity);
            }
        } else
            client->playerState.pmove.flags &= ~PMF_JUMP_HELD;
    }

    // update chase cam if being followed
    for (int i = 1; i <= maximumclients->value; i++) {
        other = game.world->GetServerEntities() + i;
        if (other->inUse && other->client->chaseTarget == serverEntity)
            SVG_UpdateChaseCam(classEntity);
    }
}