/*
// LICENSE HERE.

//
// DeathmatchGamemode.cpp
//
//
*/
#include "../ServerGameLocal.h"          // SVGame.
#include "../Effects.h"     // Effects.
#include "../Entities.h"    // Entities.
#include "../Utilities.h"       // Util funcs.

// Server Game Base Entity.
#include "../Entities/Base/PlayerClient.h"

// Weapons.h
#include "../Player/Client.h"
#include "../Player/Hud.h"
#include "../Player/Weapons.h"
#include "../Player/View.h"

// Game Mode.
#include "DeathmatchGamemode.h"

//
// Constructor/Deconstructor.
//
DeathmatchGamemode::DeathmatchGamemode() : DefaultGamemode() {

}
DeathmatchGamemode::~DeathmatchGamemode() {

}

//
// Interface functions. 
//
//

//
//===============
// DeathmatchGamemode::CanDamage
//
// Template function serves as an example atm.
//===============
//
qboolean DeathmatchGamemode::CanDamage(SVGBaseEntity* target, SVGBaseEntity* inflictor) {
    // Let it be to DefaultGamemode. :)
    return DefaultGamemode::CanDamage(target, inflictor);
}

//===============
// DeathmatchGamemode::ClientBegin
// 
// Called when a client is ready to be placed in the game after connecting.
//===============
void DeathmatchGamemode::ClientBegin(Entity* serverEntity) {
    if (!serverEntity) {
        gi.DPrintf("ClientBegin executed with invalid (nullptr) serverEntity");
        return;
    }

    if (!serverEntity->client) {
        gi.DPrintf("ClientBegin executed with invalid (nullptr) serverEntity->client");
        return;
    }

    // Setup the client for the server entity.
    serverEntity->client = game.clients + (serverEntity - g_entities - 1);

    // Initialize a clean serverEntity.
    SVG_InitEntity(serverEntity);

    // Delete previous classentity, if existent (older client perhaps).
    SVG_FreeClassFromEntity(serverEntity);

    // Recreate class PlayerClient entity.
    serverEntity->classEntity = SVG_CreateClassEntity<PlayerClient>(serverEntity, false);

    // Initialize client respawn data.
    InitializeClientRespawnData(serverEntity->client);
 
    // Put into our server and blast away! (Takes care of spawning classEntity).
    PutClientInServer(serverEntity);

    if (level.intermission.time) {
        HUD_MoveClientToIntermission(serverEntity);
    } else {
        gi.WriteByte(SVG_CMD_MUZZLEFLASH);
        gi.WriteShort(serverEntity - g_entities);
        gi.WriteByte(MuzzleFlashType::Login);
        gi.Multicast(serverEntity->state.origin, MultiCast::PVS);
    }
    
    gi.BPrintf(PRINT_HIGH, "%s entered the game\n", serverEntity->client->persistent.netname);

    // Call ClientEndServerFrame to update him through the beginning frame.
    ClientEndServerFrame(serverEntity);
}

void DeathmatchGamemode::PutClientInServer(Entity *ent) {
    // Find a spawn point for this client to be "placed"/"put" at.
    vec3_t  mins = PM_MINS;
    vec3_t  maxs = PM_MAXS;

    ClientPersistentData persistentData; // Saved data from 
    ClientRespawnData respawnData;

    // Find a spawn point
    // Do it before setting health back up, so farthest
    // ranging doesn't count this client
    vec3_t  spawnOrigin = vec3_zero();
    vec3_t  spawnAngles = vec3_zero();

    DefaultGamemode::SelectClientSpawnPoint(ent, spawnOrigin, spawnAngles, "info_player_deathmatch");

    // Fetch the entity index, and the client right off the bat.
    int32_t index = ent - g_entities - 1;
    ServerClient* client = ent->client;

    // Deathmatch wipes most client data every spawn
    //-----------------------------------------------------------------------
    //if (deathmatch->value) {
    char        userinfo[MAX_INFO_STRING];

    respawnData = client->respawn;
    memcpy(userinfo, client->persistent.userinfo, sizeof(userinfo));
    InitializeClientPersistentData(client);
    ClientUserinfoChanged(ent, userinfo);
    //} else {
    //-----------------------------------------------------------------------
    //}
    //-------------------------------------------------------------------------------

    // Clear everything but the persistant data
    persistentData = client->persistent;
    memset(client, 0, sizeof(*client));
    client->persistent = persistentData;
    if (client->persistent.health <= 0)
        InitializeClientPersistentData(client);
    client->respawn = respawnData;

    // Copy some data from the client to the entity
    FetchClientEntityData(ent);

    // clear entity values
    PlayerClient* playerClient = (PlayerClient*)ent->classEntity;
    playerClient->SetGroundEntity(nullptr);
    playerClient->SetClient(&game.clients[index]);
    playerClient->SetTakeDamage(TakeDamage::Aim);
    playerClient->SetMoveType(MoveType::Walk);
    playerClient->SetViewHeight(22);
    playerClient->SetInUse(true);
    playerClient->SetClassname("player");
    playerClient->SetSolid(Solid::BoundingBox);
    playerClient->SetDeadFlag(DEAD_NO);
    playerClient->SetAirFinishedTime(level.time + 12);
    playerClient->SetClipMask(CONTENTS_MASK_PLAYERSOLID);
    playerClient->SetModel("players/male/tris.md2");
    //playerClient->SetTakeDamageCallback(&PlayerClient::PlayerClientTakeDamage);
    playerClient->SetDieCallback(&PlayerClient::PlayerClientDie);
    /*ent->pain = player_pain;*/
    playerClient->SetWaterLevel(0);
    playerClient->SetWaterType(0);
    playerClient->SetFlags(playerClient->GetFlags() & ~EntityFlags::NoKnockBack);
    playerClient->SetServerFlags(playerClient->GetServerFlags() & ~EntityServerFlags::DeadMonster);
    playerClient->SetMins(mins);
    playerClient->SetMaxs(maxs);
    playerClient->SetVelocity(vec3_zero());

    // Clear playerstate values
    memset(&ent->client->playerState, 0, sizeof(client->playerState));

    // Setup player move origin to spawnpoint origin.
    client->playerState.pmove.origin = spawnOrigin;

    if ((int)gamemodeflags->value & GamemodeFlags::FixedFOV) {
        client->playerState.fov = 90;
    } else {
        client->playerState.fov = atoi(Info_ValueForKey(client->persistent.userinfo, "fov"));
        if (client->playerState.fov < 1)
            client->playerState.fov = 90;
        else if (client->playerState.fov > 160)
            client->playerState.fov = 160;
    }

    // Set gun index to whichever was persistent in (if any) previous map.
    client->playerState.gunIndex = gi.ModelIndex(client->persistent.activeWeapon->viewModel);

    // clear entity state values
    ent->state.effects = 0;
    ent->state.modelIndex = 255;        // will use the skin specified model
    ent->state.modelIndex2 = 255;       // custom gun model
                                        // sknum is player num and weapon number
                                        // weapon number will be added in changeweapon
    ent->state.skinNumber = ent - g_entities - 1;

    ent->state.frame = 0;
    ent->state.origin = spawnOrigin;
    ent->state.origin.z += 1;  // Mmake sure entity is off the ground

                               // Set old Origin to current, because hell, we are here now spawning.
    ent->state.oldOrigin = ent->state.origin;

    // set the delta angle
    client->playerState.pmove.deltaAngles = spawnAngles - client->respawn.commandViewAngles;

    ent->state.angles[vec3_t::Pitch] = 0;
    ent->state.angles[vec3_t::Yaw] = spawnAngles[vec3_t::Yaw];
    ent->state.angles[vec3_t::Roll] = 0;

    client->playerState.pmove.viewAngles = ent->state.angles;
    client->aimAngles = ent->state.angles;
    //VectorCopy(ent->s.angles, client->ps.viewangles);
    //VectorCopy(ent->s.angles, client->v_angle);

    // spawn a spectator in case the client was/is one.
    if (client->persistent.isSpectator) {
        // Nodefault chase target.
        client->chaseTarget = nullptr;

        // Well we knew this but store it in respawn data too.
        client->respawn.isSpectator = true;

        // Movement type is the obvious noclip
        playerClient->SetMoveType(MoveType::NoClip);

        // No solid.
        ent->solid = Solid::Not;

        // NoClient flag, aka, do not send this entity to other clients. It is invisible to them.
        ent->serverFlags |= EntityServerFlags::NoClient;

        // Obviously no gun index.
        ent->client->playerState.gunIndex = 0;

        // Last but not least link our entity.
        gi.LinkEntity(ent);

        return;
    } else {
        client->respawn.isSpectator = false;
    }

    if (!SVG_KillBox(playerClient)) {
        // could't spawn in?
    }

    gi.LinkEntity(ent);

    // force the current weapon up
    client->newWeapon = client->persistent.activeWeapon;
    SVG_ChangeWeapon(playerClient);
}

//===============
// DeathmatchGamemode::ClientUserinfoChanged
// 
//===============
void DeathmatchGamemode::ClientUserinfoChanged(Entity* ent, char* userinfo) {
    char    *s;
    int     playernum;

    // check for malformed or illegal info strings
    if (!Info_Validate(userinfo)) {
        strcpy(userinfo, "\\name\\badinfo\\skin\\male/grunt");
    }

    // set name
    s = Info_ValueForKey(userinfo, "name");
    strncpy(ent->client->persistent.netname, s, sizeof(ent->client->persistent.netname) - 1);

    // set spectator
    s = Info_ValueForKey(userinfo, "spectator");
    // spectators are only supported in deathmatch
    if (*s && strcmp(s, "0"))
        ent->client->persistent.isSpectator = true;
    else
        ent->client->persistent.isSpectator = false;

    // set skin
    s = Info_ValueForKey(userinfo, "skin");

    playernum = ent - g_entities - 1;

    // combine name and skin into a configstring
    gi.configstring(ConfigStrings::PlayerSkins + playernum, va("%s\\%s", ent->client->persistent.netname, s));

    // fov
    if (((int)gamemodeflags->value & GamemodeFlags::FixedFOV)) {
        ent->client->playerState.fov = 90;
    } else {
        ent->client->playerState.fov = atoi(Info_ValueForKey(userinfo, "fov"));
        if (ent->client->playerState.fov < 1)
            ent->client->playerState.fov = 90;
        else if (ent->client->playerState.fov > 160)
            ent->client->playerState.fov = 160;
    }

    // handedness
    s = Info_ValueForKey(userinfo, "hand");
    if (strlen(s)) {
        ent->client->persistent.hand = atoi(s);
    }

    // save off the userinfo in case we want to check something later
    strncpy(ent->client->persistent.userinfo, userinfo, sizeof(ent->client->persistent.userinfo));
}

//===============
// DefaultGamemode::ClientBeginServerFrame
// 
// Does logic checking for a client's start of a server frame. In case there
// is a "level.intermission.time" set, it'll flat out return.
// 
// This basically allows for the game to disable fetching user input that makes
// our movement tick. And/or shoot weaponry while in intermission time.
//===============
void DeathmatchGamemode::ClientBeginServerFrame(SVGBaseEntity* entity, ServerClient *client) {
    // Ensure we aren't in an intermission time.
    if (level.intermission.time)
        return;

    // This has to go ofc.... lol. What it simply does though, is determine whether there is 
    // a need to respawn as spectator.
    if (client->persistent.isSpectator != client->respawn.isSpectator &&
        (level.time - client->respawnTime) >= 5) {
        RespawnSpectator(dynamic_cast<PlayerClient*>(entity));
        return;
    }

    // Run weapon animations in case this has not been done by user input itself.
    // (Idle animations, and general weapon thinking when a weapon is not in action.)
    if (!client->weaponThunk && !client->respawn.isSpectator)
        SVG_ThinkWeapon(dynamic_cast<PlayerClient*>(entity));
    else
        client->weaponThunk = false;

    // Check if the player is actually dead or not. If he is, we're going to enact on
    // the user input that's been given to us. When fired, we'll respawn.
    int32_t buttonMask = 0;
    if (entity->GetDeadFlag()) {
        // Wait for any button just going down
        if (level.time > client->respawnTime) {
            // In old code, the need to hit a key was only set in DM mode.
            // I figured, let's keep it like this instead.
            //if (deathmatch->value)
            buttonMask = BUTTON_ATTACK;
            //else
            //buttonMask = -1;

            if ((client->latchedButtons & buttonMask) ||
                 ((int)gamemodeflags->value & GamemodeFlags::ForceRespawn)) {
                RespawnClient(dynamic_cast<PlayerClient*>(entity));
                client->latchedButtons = 0;
            }
        }
        return;
    }

    //// add player trail so monsters can follow
    //if (!deathmatch->value)
    //    if (!visible(ent, SVG_PlayerTrail_LastSpot()))
    //        SVG_PlayerTrail_Add(ent->state.oldOrigin);

    // Reset the latched buttons.
    client->latchedButtons = 0;
}

//===============
// DeathmatchGamemode::ClientUpdateObituary.
// 
//===============
void DeathmatchGamemode::ClientUpdateObituary(SVGBaseEntity* self, SVGBaseEntity* inflictor, SVGBaseEntity* attacker) {
    std::string message = ""; // String stating what happened to whichever entity. "suicides", "was squished" etc.
    std::string messageAddition = ""; // String stating what is additioned to it, "'s shrapnell" etc. Funny stuff.

    // No friendly fire in DeathMatch.
    //if (coop->value && attacker->client)
    //    meansOfDeath |= MOD_FRIENDLY_FIRE;

    //// Set a bool for whether we got friendly fire.
    //qboolean friendlyFire = meansOfDeath & MeansOfDeath::FriendlyFire;
    qboolean friendlyFire = false;
    // Quickly remove it from meansOfDeath again, our bool is set. This prevents it from 
    // sticking around when we process the next entity/client.
    int32_t finalMeansOfDeath = meansOfDeath;// &~MeansOfDeath::FriendlyFire; // Sum of things, final means of death.

    // Determine the means of death.
    switch (finalMeansOfDeath) {
        case MeansOfDeath::Suicide:
            message = "suicides";
            break;
        case MeansOfDeath::Falling:
            message = "cratered";
            break;
        case MeansOfDeath::Crush:
            message = "was squished";
            break;
        case MeansOfDeath::Water:
            message = "sank like a rock";
            break;
        case MeansOfDeath::Slime:
            message = "melted";
            break;
        case MeansOfDeath::Lava:
            message = "does a back flip into the lava";
            break;
        case MeansOfDeath::Explosive:
        case MeansOfDeath::Barrel:
            message = "blew up";
            break;
        case MeansOfDeath::Exit:
            message = "found a way out";
            break;
        case MeansOfDeath::Splash:
        case MeansOfDeath::TriggerHurt:
            message = "was in the wrong place";
            break;
    }

    // Check if the attacker hurt himself, if so, ... n00b! :D 
    if (attacker == self) {
        switch (finalMeansOfDeath) {
            case MeansOfDeath::GrenadeSplash:
                message = "tripped on his own grenade";
                break;
            case MeansOfDeath::RocketSplash:
                message = "blew himself up";
                break;
            default:
                message = "killed himself";
                break;
        }
    }

    // Generated a message?
    if (message != "") {
        gi.BPrintf(PRINT_MEDIUM, "%s %s.\n", self->GetClient()->persistent.netname, message.c_str());
        //if (deathmatch->value)
        self->GetClient()->respawn.score--;
        self->SetEnemy(NULL);
        return;
    }

    // Set 'self' its attacker entity pointer.
    self->SetEnemy(attacker);

    // If we have an attacker, and it IS a client...
    if (attacker && attacker->GetClient()) {
        switch (finalMeansOfDeath) {
            case MeansOfDeath::Blaster:
                message = "was blasted by";
                break;
            case MeansOfDeath::Shotgun:
                message = "was gunned down by";
                break;
            case MeansOfDeath::SuperShotgun:
                message = "was blown away by";
                messageAddition = "'s super shotgun";
                break;
            case MeansOfDeath::Machinegun:
                message = "was machinegunned by";
                break;
            case MeansOfDeath::Grenade:
                message = "was popped by";
                messageAddition = "'s grenade";
                break;
            case MeansOfDeath::GrenadeSplash:
                message = "was shredded by";
                messageAddition = "'s shrapnel";
                break;
            case MeansOfDeath::Rocket:
                message = "ate";
                messageAddition = "'s rocket";
                break;
            case MeansOfDeath::RocketSplash:
                message = "almost dodged";
                messageAddition = "'s rocket";
                break;
            case MeansOfDeath::TeleFrag:
                message = "tried to invade";
                messageAddition = "'s personal space";
                break;
        }

        // In case we have a message, proceed.
        if (!message.empty()) {
            // Print it.
            gi.BPrintf(PRINT_MEDIUM, "%s %s %s%s.\n", self->GetClient()->persistent.netname, message.c_str(), attacker->GetClient()->persistent.netname, messageAddition.c_str());
            
            // WID: Old piec of code // if (deathmatch->value) {
            if (friendlyFire)
                attacker->GetClient()->respawn.score--;
            else
                attacker->GetClient()->respawn.score++;
            // WID: Old piec of code // }
            return;
        }
    }

    // Check for monster deaths here.
    if (attacker->GetServerFlags() & EntityServerFlags::Monster) {
        // Fill in message here
        // aka if (attacker->classname == "monster_1337h4x0r")
        // Then we do...
        // Also we gotta adjust that ->classname thing, but this is a template, cheers :)
        if (!message.empty()) {
            gi.BPrintf(PRINT_MEDIUM, "%s %s %s%s\n", self->GetClient()->persistent.netname, message.c_str(), attacker->GetClassname(), messageAddition.c_str());
            if (friendlyFire) {
                attacker->GetClient()->respawn.score--;
            } else {
                attacker->GetClient()->respawn.score++;
            }

            return;
        }
    }

    //// Inform the client died.
    //gi.BPrintf(PRINT_MEDIUM, "%s died.\n", self->GetClient()->persistent.netname);

    //// WID: This was an old piece of code, keeping it so people know what..// if (deathmatch->value)
    //// Get the client, and change its current score.
    //self->GetClient()->respawn.score--;
}

//===============
// DeathmatchGamemode::RespawnClient
// 
// Respawns a client after intermission and hitting a button.
//===============
void DeathmatchGamemode::RespawnClient(PlayerClient* ent) {
    // Spectator's don't leave bodies
    if (ent->GetMoveType() != MoveType::NoClip)
        SpawnClientCorpse(ent);

    ent->SetServerFlags(ent->GetServerFlags() & ~EntityServerFlags::NoClient);
    PutClientInServer(ent->GetServerEntity());

    // Add a teleportation effect
    ent->SetEventID(EntityEvent::PlayerTeleport);

    // Hold in place briefly
    ServerClient* serverClient = ent->GetClient();

    // Hold in place for 14 frames and set pmove flags to teleport so the player can
    // respawn somewhere safe without it interpolating its positions.
    serverClient->playerState.pmove.flags = PMF_TIME_TELEPORT;
    serverClient->playerState.pmove.time = 14;

    // Setup respawn time.
    serverClient->respawnTime = level.time;
}

//===============
// DeathmatchGamemode::RespawnAllClients
//
// Respawn all valid client entities who's health is < 0.
//===============
void DeathmatchGamemode::RespawnAllClients() {

    for (auto& clientEntity : g_baseEntities | bef::Standard | bef::HasClient | bef::IsSubclassOf<PlayerClient>()) {
        if (clientEntity->GetHealth() < 0) {
            RespawnClient(dynamic_cast<PlayerClient*>(clientEntity));
        }
    }
}

//===============
// DeathmatchGamemode::ClientDeath
// 
// Does nothing for this game mode.
//===============
void DeathmatchGamemode::ClientDeath(PlayerClient *clientEntity) {

}

//===============
// DeathmatchGamemode::RespawnSpectator
// 
// Respawns a spectator after intermission and hitting a button.
//===============
void DeathmatchGamemode::RespawnSpectator(PlayerClient* ent) {
    // Spectator's don't leave bodies
    if (ent->GetMoveType() != MoveType::NoClip)
        SpawnClientCorpse(ent);

    ent->SetServerFlags(ent->GetServerFlags() & ~EntityServerFlags::NoClient);
    PutClientInServer(ent->GetServerEntity());

    // add a teleportation effect
    ent->SetEventID(EntityEvent::PlayerTeleport);

    // hold in place briefly
    ServerClient* client = ent->GetClient();

    client->playerState.pmove.flags = PMF_TIME_TELEPORT;
    client->playerState.pmove.time = 14;

    client->respawnTime = level.time;
}