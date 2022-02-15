/*
// LICENSE HERE.

//
// DefaultGamemode.cpp
//
//
*/
#include "../ServerGameLocal.h"          // SVGame.
#include "../Effects.h"     // Effects.
#include "../Entities.h"    // Entities.
#include "../Utilities.h"       // Util funcs.

// Server Game Base Entity.
#include "../Entities/Base/BodyCorpse.h"
#include "../Entities/Base/SVGBasePlayer.h"
#include "../Entities/Info/InfoPlayerStart.h"

// Weapons.h
#include "../Player/Client.h"
#include "../Player/Hud.h"
#include "../Player/Weapons.h"
#include "../Player/View.h"
#include "../Player/Animations.h"

// Game Mode.
#include "DefaultGamemode.h"

//
// Constructor/Deconstructor.
//
DefaultGamemode::DefaultGamemode() {

}
DefaultGamemode::~DefaultGamemode() {

}

//
// Interface functions. 
//
//===============
// DefaultGamemode::GetEntityTeamName
//
// Assigns the teamname to the string passed, returns false in case the entity
// is not part of a team at all.
//===============
qboolean DefaultGamemode::GetEntityTeamName(SVGBaseEntity* ent, std::string &teamName) {
    // Placeholder.
    teamName = "";

    return false;

    //// We'll assume that this entity needs to have a client.
    //if (!ent->GetClient())
    //    return "";

    //// Fetch the 'skin' info_valueforkey of the given client.
    //std::string clientSkin = Info_ValueForKey(ent->GetClient()->persistent.userinfo, "skin");

    //// Start scanning for a /, in case there is none, we can just return the skin as is.
    //auto slashPosition = clientSkin.find_last_of('/');
    //if (slashPosition == std::string::npos)
    //    return clientSkin;

    //// Since we did find one if we reach this code, we'll check waht our game mode flags demand.
    //if (gamemodeflags->integer & GamemodeFlags::ModelTeams) {
    //    return clientSkin;
    //}

    //// Otherwise, in case we got skin teams... Return the skin specific part as team name.
    //if (gamemodeflags->integer & GamemodeFlags::SkinTeams) {
    //    return clientSkin.substr(slashPosition);
    //}

    //// We should never reach this point, but... there just in case.
    //return "";
}

//===============
// DefaultGamemode::OnSameTeam
//
// Returns false either way, because yes, there is no... team in this case.
// PS: ClientTeam <-- weird function, needs C++-fying and oh.. it stinks anyhow.
//===============
qboolean DefaultGamemode::OnSameTeam(SVGBaseEntity* ent1, SVGBaseEntity* ent2) {
    //// There is only a reason to check for this in case these specific
    //// game mode flags are set.
    //if (!((int)(gamemodeflags->value) & (GamemodeFlags::ModelTeams | GamemodeFlags::SkinTeams)))
    //    return false;

    //// Fetch the team names of both entities.
    //std::string teamEntity1 = GetEntityTeamName(ent1);
    //std::string teamEntity2 = GetEntityTeamName(ent2);

    //// In case they are equal, return true.
    //if (!teamEntity1.empty() && !teamEntity2.empty())
    //    if (teamEntity1 != "" && teamEntity2 != "")
    //        if (teamEntity1 == teamEntity2)
    //            return true;

    // If we reached this point, we're done, no going on.
    return false;
}

//===============
// DefaultGamemode::CanDamage
//
//===============
qboolean DefaultGamemode::CanDamage(SVGBaseEntity* target, SVGBaseEntity* inflictor) {
    vec3_t  destination;
    SVGTrace trace;

    // WID: Admer, why the fuck did they rush hour these comments all the time?
    // bmodels need special checking because their origin is 0,0,0 <-- is bad.
    //
    // Solid entities need a special check, as their origin is usually 0,0,0
    // Exception to the above: the solid entity moves or has an origin brush
    if (target->GetMoveType() == MoveType::Push) {
        // Calculate destination.
        destination = target->GetAbsoluteMin() + target->GetAbsoluteMax();
        destination = vec3_scale(destination, 0.5f);
        trace = SVG_Trace(inflictor->GetOrigin(), vec3_zero(), vec3_zero(), destination, inflictor, CONTENTS_MASK_SOLID);
        if (trace.fraction == 1.0)
            return true;
        if (trace.ent == target)
            return true;
        return false;
    }

    // From here on we start tracing in various directions. Look at the code yourself to figure that one out...
    trace = SVG_Trace(inflictor->GetOrigin(), vec3_zero(), vec3_zero(), target->GetOrigin(), inflictor, CONTENTS_MASK_SOLID);
    if (trace.fraction == 1.0)
        return true;

    destination = target->GetOrigin();
    destination[0] += 15.0;
    destination[1] += 15.0;
    trace = SVG_Trace(inflictor->GetOrigin(), vec3_zero(), vec3_zero(), destination, inflictor, CONTENTS_MASK_SOLID);
    if (trace.fraction == 1.0)
        return true;

    destination = target->GetOrigin();
    destination[0] += 15.0;
    destination[1] -= 15.0;
    trace = SVG_Trace(inflictor->GetOrigin(), vec3_zero(), vec3_zero(), destination, inflictor, CONTENTS_MASK_SOLID);
    if (trace.fraction == 1.0)
        return true;

    destination = target->GetOrigin();
    destination[0] -= 15.0;
    destination[1] += 15.0;
    trace = SVG_Trace(inflictor->GetOrigin(), vec3_zero(), vec3_zero(), destination, inflictor, CONTENTS_MASK_SOLID);
    if (trace.fraction == 1.0)
        return true;

    destination = target->GetOrigin();
    destination[0] -= 15.0;
    destination[1] -= 15.0;
    trace = SVG_Trace(inflictor->GetOrigin(), vec3_zero(), vec3_zero(), destination, inflictor, CONTENTS_MASK_SOLID);
    if (trace.fraction == 1.0)
        return true;

    // If we reached this point... Well, it is false :)
    return false;
}

//===============
// DefaultGamemode::FindWithinRadius
//
// Returns a BaseEntityVector list containing the results of the found
// given entities that reside within the origin to radius. 
// 
// Flags can be set to determine which "solids" to exclude.
//===============
BaseEntityVector DefaultGamemode::FindBaseEnitiesWithinRadius(const vec3_t& origin, float radius, uint32_t excludeSolidFlags) {
    // List of base entities to return.
    std::vector<SVGBaseEntity*> baseEntityList;

    // Iterate over all entities, see who is nearby, and who is not.
    for (auto* radiusBaseEntity : GetBaseEntityRange<0, MAX_EDICTS>()
         | bef::Standard
         | bef::WithinRadius(origin, radius, excludeSolidFlags)) {

        // Push radiusEntity result item to the list.
        baseEntityList.push_back(radiusBaseEntity);
    }

    // The list might be empty, ensure to check for that ;-)
    return baseEntityList;
}

//===============
// DefaultGamemode::EntityKilled
//
// Called when an entity is killed, or at least, about to be.
// Determine how to deal with it, usually resides in a callback to Die.
//===============
void DefaultGamemode::EntityKilled(SVGBaseEntity* target, SVGBaseEntity* inflictor, SVGBaseEntity* attacker, int32_t damage, vec3_t point) {
    // Ensure health isn't exceeding limits.
    if (target->GetHealth() < -999)
        target->SetHealth(-999);

    // Set the enemy pointer to the current attacker.
    target->SetEnemy(attacker);

    // Determine whether it is a monster, and if it IS set to being dead....
    if ((target->GetServerFlags() & EntityServerFlags::Monster) && (target->GetDeadFlag() != DEAD_DEAD)) {
        target->SetServerFlags(target->GetServerFlags() | EntityServerFlags::DeadMonster);   // Now treat as a different content type

//        if (!(targ->monsterInfo.aiflags & AI_GOOD_GUY)) {
//            level.killedMonsters++;
//            if (coop->value && attacker->client)
//                attacker->client->respawn.score++;
//            // medics won't heal monsters that they kill themselves
//            if (strcmp(attacker->classname, "monster_medic") == 0)
//                targ->owner = attacker;
//        }
    }

    if (target->GetMoveType() == MoveType::Push || target->GetMoveType() == MoveType::Stop || target->GetMoveType() == MoveType::None) {
        // Doors, triggers, etc
        if (target) {
            target->Die(inflictor, attacker, damage, point);
        }

        return;
    }

    if ((target->GetServerFlags() & EntityServerFlags::Monster) && (target->GetDeadFlag() != DEAD_DEAD)) {
        target->SetTouchCallback(nullptr);

        // This can only be done on base monster entities and derivates
        //if (target->IsSubclassOf<BaseMonster>()) {
        //    target->DeathUse();
        //}
    //    monster_death_use(targ);
    }
    if (target) {
        target->Die(inflictor, attacker, damage, point);
    }
    //targ->Die(targ, inflictor, attacker, damage, point);
}

//===============
// DefaultGamemode::InflictDamage
// 
//===============
void DefaultGamemode::InflictDamage(SVGBaseEntity* target, SVGBaseEntity* inflictor, SVGBaseEntity* attacker, const vec3_t& dmgDir, const vec3_t& point, const vec3_t& normal, int32_t damage, int32_t knockBack, int32_t damageFlags, int32_t mod) {
    int32_t damageTaken = 0;   // Damage taken.
    int32_t damageSaved = 0;   // Damaged saved, from being taken.

                               // Best be save than sorry.
    if (!target || !inflictor || !attacker) {
        return;
    }

    // In case this entity is not taking any damage, all bets are off, don't bother moving on.
    if (!target->GetTakeDamage())
        return;

    // WID: This sticks around, cuz of reference, but truly will be all but this itself was.
    // friendly fire avoidance
    // if enabled you can't hurt teammates (but you can hurt yourself)
    // knockBack still occurs
    //if ((targ != attacker) && ((deathmatch->value && ((int)(gamemodeflags->value) & (GamemodeFlags::ModelTeams | GamemodeFlags::SkinTeams))) || coop->value)) {
    //    if (game.GetCurrentGamemode()->OnSameTeam(targ, attacker)) {
    //        if ((int)(gamemodeflags->value) & GamemodeFlags::NoFriendlyFire)
    //            damage = 0;
    //        else
    //            mod |= MeansOfDeath::FriendlyFire;
    //    }
    //}
    // We resort to defaults, but keep the above as mentioned.
    SetCurrentMeansOfDeath(mod);

    // Fetch client.
    ServerClient* client = target->GetClient();

    // Determine which temp entity event to use by default.
    int32_t tempEntityEvent = TempEntityEvent::Sparks;
    if (damageFlags & DamageFlags::Bullet)
        tempEntityEvent = TempEntityEvent::BulletSparks;

    // Retrieve normalized direction.
    vec3_t dir = vec3_normalize(dmgDir);

    // Ensure there is no odd knockback issues.
    if (target->GetFlags() & EntityFlags::NoKnockBack)
        knockBack = 0;

    // Figure out the momentum to add in case KnockBacks are off. 
    if (!(damageFlags & DamageFlags::NoKnockBack)) {
        if ((knockBack) && (target->GetMoveType() != MoveType::None) && (target->GetMoveType() != MoveType::Bounce) && (target->GetMoveType() != MoveType::Push) && (target->GetMoveType() != MoveType::Stop)) {
            vec3_t  kickbackVelocity = { 0.f, 0.f, 0.f };
            float   targetMass = 50; // Defaults to 50, otherwise... issues, this is the OG code style btw.

            // Based on mass, if it is below 50, we wanna hook it to being 50. Otherwise...
            if (target->GetMass() > 50)
                targetMass = target->GetMass();

            // Determine whether attacker == target, and the client itself, that means we gotta jump back hard.
            if (target->GetClient() && attacker == target)
                kickbackVelocity = vec3_scale(dir, 1600.0 * (float)knockBack / targetMass); // ROCKET JUMP HACK IS HERE BRUH <--
            else
                kickbackVelocity = vec3_scale(dir, 500 * (float)knockBack / targetMass);

            // Assign the new velocity, since yeah, it's bound to knock the fuck out of us.
            target->SetVelocity(target->GetVelocity() + kickbackVelocity);
        }
    }

    // Setup damages, so we can maths with them, yay. Misses code cuz we got no armors no more :P
    damageTaken = damage;       // Damage taken.
    damageSaved = 0;            // Damaged saved, from being taken.

    // Check for godmode.
    if ((target->GetFlags() & EntityFlags::GodMode) && !(damageFlags & DamageFlags::IgnoreProtection)) {
        damageTaken = 0;
        damageSaved = damage;

        // Leave it for the game mode to move on and spawn this temp entity (if allowed.)
        SpawnTempDamageEntity(tempEntityEvent, point, normal, damageSaved);
    }

    // Team damage avoidance
    if (!(damageFlags & DamageFlags::IgnoreProtection) && game.GetCurrentGamemode()->OnSameTeam(target, attacker))
        return;

    // Inflict the actual damage, in case we got to deciding to do so based on the above.
    if (damageTaken) {
        // Check if monster, or client, in which case, we spawn blood.
        // If not... :)... Do not.
        if (target->GetServerFlags() & EntityServerFlags::Monster || client) {
            // SpawnTempDamageEntity(TempEntityEvent::Blood, point, normal, take);
            // Leave it for the game mode to move on and spawn this temp entity (if allowed.)
	        SpawnTempDamageEntity(TempEntityEvent::Blood, point, dir, damageTaken);
        } else {
            // Leave it for the game mode to move on and spawn this temp entity (if allowed.)
            SpawnTempDamageEntity(tempEntityEvent, point, normal, damageTaken);
        }

        // Adjust health based on calculated damage to take.
        target->SetHealth(target->GetHealth() - damageTaken);

        // In case health is/was below 0.
        if (target->GetHealth() <= 0) {
            // Check if monster, or client, in which case, we execute no knockbacks :)
            if ((target->GetServerFlags() & EntityServerFlags::Monster) || (client))
                target->SetFlags(target->GetFlags() | EntityFlags::NoKnockBack);

            // It's dead though, or at least we assume so... Call on to: EntityKilled.
            EntityKilled(target, inflictor, attacker, damageTaken, point);
            return;
        }
    }

    // Special damage handling for monsters.
    if (target->GetServerFlags() & EntityServerFlags::Monster) {
        // WID: Maybe do some check for monster entities here sooner or later? Who knows...
        // Gotta have them cunts react to it. But we'll see, might as well be on TakeDamage :)
        //M_ReactToDamage(targ, attacker);

        //if (!(targ->monsterInfo.aiflags & AI_DUCKED) && (take)) {
        target->TakeDamage(attacker, knockBack, damageTaken);
        //// nightmare mode monsters don't go into pain frames often
        //if (skill->value == 3)
        //    targ->debouncePainTime = level.time + 5;
        //}
    } else {
        if (client) {
            //if (!(targ->flags & EntityFlags::GodMode) && (take))
            //    targ->Pain(targ, attacker, knockBack, take);
            if (!(target->GetFlags() & EntityFlags::GodMode) && (damageTaken)) {
                target->TakeDamage(attacker, knockBack, damageTaken);
            }
        } else if (damageTaken) {
            target->TakeDamage(attacker, knockBack, damageTaken);
        }
    }

    // add to the damage inflicted on a player this frame
    // the total will be turned into screen blends and view angle kicks
    // at the end of the frame
    if (client) {
        client->damages.blood += damageTaken;
        client->damages.knockBack += knockBack;
        client->damages.from = point;
    }
}

//===============
// DefaultGamemode::InflictDamage
// 
//===============
void DefaultGamemode::InflictRadiusDamage(SVGBaseEntity* inflictor, SVGBaseEntity* attacker, float damage, SVGBaseEntity* ignore, float radius, int32_t mod) {
    // Damage point counter for radius sum ups.
    float   points = 0.f;

    // Actual entity loop pointer.
    SVGBaseEntity* ent = nullptr;

    // N&C: From Yamagi Q2, to prevent issues.
    if (!inflictor || !attacker) {
        return;
    }

    // Find entities within radius.
    BaseEntityVector radiusEntities = FindBaseEnitiesWithinRadius(inflictor->GetOrigin(), radius, Solid::Not);

    //while ((ent = SVG_FindEntitiesWithinRadius(ent, inflictor->GetOrigin(), radius)) != NULL) {
    for (auto& baseEntity : radiusEntities) {
        //// Continue in case this entity has to be ignored from applying damage.
        if (baseEntity == ignore)
            continue;

        // Continue in case this entity CAN'T take any damage.
        if (!baseEntity->GetTakeDamage())
            continue;

        // Calculate damage points.
        vec3_t velocity = baseEntity->GetMins() + baseEntity->GetMaxs();
        velocity = vec3_fmaf(baseEntity->GetOrigin(), 0.5f, velocity);
        velocity -= inflictor->GetOrigin();
        points = damage - 0.5f * vec3_length(velocity);

        // In case the attacker is the own entity, half damage.
        if (ent == attacker)
            points = points * 0.5f;

        // Apply damage points.
        if (points > 0) {
            // Ensure whether we CAN actually apply damage.
            if (CanDamage(baseEntity, inflictor)) {
                // Calculate direcion.
                vec3_t dir = baseEntity->GetOrigin() - inflictor->GetOrigin();

                // Apply damages.
                InflictDamage(baseEntity, inflictor, attacker, dir, inflictor->GetOrigin(), vec3_zero(), (int)points, (int)points, DamageFlags::IndirectFromRadius, mod);
            }
        }
    }
}

//===============
// DefaultGamemode::SetCurrentMeansOfDeath
// 
//===============
void DefaultGamemode::SetCurrentMeansOfDeath(int32_t meansOfDeath) {
    this->meansOfDeath = meansOfDeath;
}

//===============
// DefaultGamemode::GetCurrentMeansOfDeath
// 
//===============
const int32_t& DefaultGamemode::GetCurrentMeansOfDeath() {
    return this->meansOfDeath;
}

//===============
// DefaultGamemode::SpawnClientCorpse
// 
// Spawns a dead body entity for the given client.
//===============
void DefaultGamemode::SpawnClientCorpse(SVGBaseEntity* ent) {
    // Ensure it is an entity.
    if (!ent)
        return;

    // Ensure it is a client.
    if (!ent->GetClient())
        return;

    // Unlink the player client entity.
    ent->UnlinkEntity();

    // Grab a body from the queue, and cycle to the next one.
    Entity *bodyEntity = &g_entities[game.GetMaxClients() + level.bodyQue + 1];
    level.bodyQue = (level.bodyQue + 1) % BODY_QUEUE_SIZE;

    // Send an effect on this body, in case it already has a model index.
    // This'll cause a body not to just "disappear", but actually play some
    // bloody particles over there.
    if (bodyEntity->state.modelIndex) {
        gi.WriteByte(ServerGameCommands::TempEntity);
        gi.WriteByte(TempEntityEvent::Blood);
        gi.WriteVector3(bodyEntity->state.origin);
        gi.WriteVector3(vec3_zero());
        gi.Multicast(bodyEntity->state.origin, MultiCast::PVS);
    }

    // Create the class entity for this queued bodyEntity.
    SVGBaseEntity *bodyClassEntity = SVG_CreateClassEntity<BodyCorpse>(bodyEntity, false);

    // Unlink the body entity, in case it was linked before.
    bodyClassEntity->UnlinkEntity();

    // Copy over the bodies state of the current entity into the body entity.
    bodyClassEntity->SetState(ent->GetState());
    // Change its number so it is accurately set to the one belonging to bodyEntity.
    // (Has to happen since we first copied over an entire entity state.)
    bodyClassEntity->SetNumber(bodyEntity - g_entities);
    // Set the event ID for this frame to OtherTeleport.
    bodyClassEntity->SetEventID(EntityEvent::OtherTeleport);

    // Copy over the serverflags from ent.
    bodyClassEntity->SetServerFlags(ent->GetServerFlags());
    bodyClassEntity->SetMins(ent->GetMins());
    bodyClassEntity->SetMaxs(ent->GetMaxs());
    bodyClassEntity->SetAbsoluteMin(ent->GetAbsoluteMin());
    bodyClassEntity->SetAbsoluteMax(ent->GetAbsoluteMax());
    bodyClassEntity->SetSize(ent->GetSize());
    bodyClassEntity->SetVelocity(ent->GetVelocity());
    bodyClassEntity->SetAngularVelocity(ent->GetAngularVelocity());
    bodyClassEntity->SetSolid(ent->GetSolid());
    bodyClassEntity->SetClipMask(ent->GetClipMask());
    bodyClassEntity->SetOwner(ent->GetOwner());
    bodyClassEntity->SetMoveType(ent->GetMoveType());
    //bodyClassEntity->SetGroundEntity(ent->GetGroundEntity());

    // Set the die callback, and set its take damage.
    bodyClassEntity->SetDieCallback(&BodyCorpse::BodyCorpseDie);
    bodyClassEntity->SetTakeDamage(TakeDamage::Yes);

    // Link it in for collision etc.
    bodyClassEntity->LinkEntity();
}

//===============
// DefaultGamemode::SpawnTempDamageEntity
// 
// Sends a message to all clients in the current PVS, spawning a temp entity for
// displaying damage entities client side. (Sparks, what have ya.)
//===============
void DefaultGamemode::SpawnTempDamageEntity(int32_t type, const vec3_t& origin, const vec3_t& normal, int32_t damage) {
    // WID: Ensure the effect can't send more damage. But that is unimplemented for the clients atm to even detect...
    if (damage > 255)
        damage = 255;

    // Write away.
    gi.WriteByte(ServerGameCommands::TempEntity);
    gi.WriteByte(type);
    //  gi.WriteByte (damage); // <-- This was legacy crap, might wanna implement it ourselves eventually.
    gi.WriteVector3(origin);
    gi.WriteVector3(normal);
    gi.Multicast(origin, MultiCast::PVS);
}

//===============
// DefaultGamemode::CalculateDamageVelocity
// 
// Default implementation for calculating velocity damage.
//===============
vec3_t DefaultGamemode::CalculateDamageVelocity(int32_t damage) {
    // Pick random velocities.
    vec3_t velocity = {
        100.0f * crandom(),
        100.0f * crandom(),
        200.0f + 100.0f * random()
    };

    // Scale velocities.
    if (damage < 50)
        velocity = vec3_scale(velocity, 0.7f);
    else
        velocity = vec3_scale(velocity, 1.2f);

    // Return.
    return velocity;
}

//===============
// DefaultGamemode::OnLevelExit
// 
// Default implementation for exiting levels.
//===============
void DefaultGamemode::OnLevelExit() {
    // Create the command to use for switching to the next game map.
    std::string command = "gamemap \"";
    command += level.intermission.changeMap;
    command += +"\"";

    // Add the gamemap command to the 
    gi.AddCommandString(command.c_str());
    // Reset the changeMap string, intermission time, and regular level time.
    level.intermission.changeMap = NULL;
    level.intermission.exitIntermission = 0;
    level.intermission.time = 0;

    // End the server frames for all clients.
    SVG_ClientEndServerFrames();

    // Loop through the server entities, and run the base entity frame if any exists.
    for (int32_t i = 0; i < maximumclients->value; i++) {
        // Fetch the Worldspawn entity number.
        Entity *serverEntity = &g_entities[i];

        if (!serverEntity)
            continue;

        if (!serverEntity->inUse)
            continue;

        uint32_t stateNumber = serverEntity->state.number;

        // Fetch the corresponding base entity.
        SVGBaseEntity* entity = g_baseEntities[stateNumber];

        // Ensure an entity its health is reset to default.
        if (entity->GetHealth() > entity->GetClient()->persistent.maxHealth)
            entity->SetHealth(entity->GetClient()->persistent.maxHealth);
    }
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
void DefaultGamemode::ClientBeginServerFrame(SVGBasePlayer* entity, ServerClient *client) {
    // Ensure we aren't in an intermission time.
    if (level.intermission.time)
        return;

    // This has to go ofc.... lol. What it simply does though, is determine whether there is 
    // a need to respawn as spectator.
    //if (deathmatch->value &&
    //    client->persistent.isSpectator != client->respawn.isSpectator &&
    //    (level.time - client->respawnTime) >= 5) {
    //    spectator_respawn(ent->GetServerEntity());
    //    return;
    //}

    // Run weapon animations in case this has not been done by user input itself.
    // (Idle animations, and general weapon thinking when a weapon is not in action.)
    if (!client->weaponThunk && !client->respawn.isSpectator)
        SVG_ThinkWeapon(dynamic_cast<SVGBasePlayer*>(entity));
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
            buttonMask = ButtonBits::Attack;
            //else
            //buttonMask = -1;

            if (client->latchedButtons & buttonMask) 
                // || (deathmatch->value && ((int)gamemodeflags->value & GamemodeFlags::ForceRespawn))) {
            {
                game.GetCurrentGamemode()->RespawnClient(dynamic_cast<SVGBasePlayer*>(entity));
                client->latchedButtons = 0;
            }
        }
        return;
    }

    // add player trail so monsters can follow
    //if (!deathmatch->value)
    //    if (!visible(ent, SVG_PlayerTrail_LastSpot()))
    //        SVG_PlayerTrail_Add(ent->state.oldOrigin);

    // Reset the latched buttons.
    client->latchedButtons = 0;
}


//===============
// DefaultGamemode::ClientEndServerFrame
// 
// Called for each player at the end of the server frame and right 
// after spawning.
// 
// Used to set the latest view offsets
//===============
void DefaultGamemode::ClientEndServerFrame(SVGBasePlayer* clientEntity, ServerClient* client) {
    // Acquire server entity.
    Entity* serverEntity = clientEntity->GetServerEntity();
    // Fetch the bobMove state.
    SVGBasePlayer::BobMoveCycle& bobMoveCycle = clientEntity->GetBobMoveCycle();

    //
    // If the origin or velocity have changed since ClientThink(),
    // update the pmove values.  This will happen when the client
    // is pushed by a bmodel or kicked by an explosion.
    //
    // If it wasn't updated here, the view position would lag a frame
    // behind the body position when pushed -- "sinking into plats"
    //
    client->playerState.pmove.origin = clientEntity->GetOrigin();
    client->playerState.pmove.velocity = clientEntity->GetVelocity();

    //
    // If the end of unit layout is displayed, don't give
    // the player any normal movement attributes
    //
    if (level.intermission.time) {
        // FIXME: add view drifting here?
        client->playerState.blend[3] = 0;
        client->playerState.fov = 90;
        SVG_HUD_SetClientStats(serverEntity);
        return;
    }

    vec3_vectors(client->aimAngles, &clientEntity->GetBobMoveCycle().forward, &clientEntity->GetBobMoveCycle().right, &clientEntity->GetBobMoveCycle().up);

    // Burn from lava, etc
    clientEntity->CheckWorldEffects();

    //
    // Set model angles from view angles so other things in
    // the world can tell which direction you are looking
    //
    vec3_t newPlayerAngles = serverEntity->state.angles;

    if (client->aimAngles[vec3_t::Pitch] > 180)
        newPlayerAngles[vec3_t::Pitch] = (-360 + client->aimAngles[vec3_t::Pitch]) / 3;
    else
        newPlayerAngles[vec3_t::Pitch] = client->aimAngles[vec3_t::Pitch] / 3;
    newPlayerAngles[vec3_t::Yaw] = client->aimAngles[vec3_t::Yaw];
    newPlayerAngles[vec3_t::Roll] = 0;
    newPlayerAngles[vec3_t::Roll] = clientEntity->CalculateRoll(newPlayerAngles, clientEntity->GetVelocity()) * 4;

    // Last but not least, after having calculated the Pitch, Yaw, and Roll, set the new angles.
    clientEntity->SetAngles(newPlayerAngles);

    //
    // Calculate the player its X Y axis' speed and calculate the cycle for
    // bobbing based on that.
    //
    vec3_t playerVelocity = clientEntity->GetVelocity();
    // Without * FRAMETIME = XYSpeed = std::sqrtf(playerVelocity[0] * playerVelocity[0] + playerVelocity[1] * playerVelocity[1]);
    bobMoveCycle.XYSpeed = std::sqrtf(playerVelocity[0] * playerVelocity[0] + playerVelocity[1] * playerVelocity[1]);

    if (bobMoveCycle.XYSpeed < 5 || !(client->playerState.pmove.flags & PMF_ON_GROUND)) {
        // Special handling for when not on ground.
        bobMoveCycle.move = 0;

        // Start at beginning of cycle again (See the else if statement.)
        client->bobTime = 0;
    } else if (clientEntity->GetGroundEntity() || clientEntity->GetWaterLevel() == 2) {
        // So bobbing only cycles when on ground.
        if (bobMoveCycle.XYSpeed > 450)
            bobMoveCycle.move = 0.25;
        else if (bobMoveCycle.XYSpeed > 210)
            bobMoveCycle.move = 0.125;
        else if (!clientEntity->GetGroundEntity() && clientEntity->GetWaterLevel() == 2 && bobMoveCycle.XYSpeed > 100)
            bobMoveCycle.move = 0.225;
        else if (bobMoveCycle.XYSpeed > 100)
            bobMoveCycle.move = 0.0825;
        else if (!clientEntity->GetGroundEntity() && clientEntity->GetWaterLevel() == 2)
            bobMoveCycle.move = 0.1625;
        else
            bobMoveCycle.move = 0.03125;
    }

    // Generate bob time.
    bobMoveCycle.move /= 3.5;
    float bobTime = (client->bobTime += bobMoveCycle.move);

    if (client->playerState.pmove.flags & PMF_DUCKED)
        bobTime *= 1.5;

    bobMoveCycle.cycle = (int)bobTime;
    bobMoveCycle.fracSin = fabs(sin(bobTime * M_PI));

    // Detect hitting the floor, and apply damage appropriately.
    clientEntity->CheckFallingDamage();

    // Apply all other the damage taken this frame
    clientEntity->ApplyDamageFeedback();

    // Determine the new frame's view offsets
    clientEntity->CalculateViewOffset();

    // Determine the gun offsets
    clientEntity->CalculateGunOffset();

    // Determine the full screen color blend
    // must be after viewOffset, so eye contents can be
    // accurately determined
    // FIXME: with client prediction, the contents
    // should be determined by the client
    clientEntity->CalculateScreenBlend();

    // Set the stats to display for this client (one of the chase isSpectator stats or...)
    if (client->respawn.isSpectator)
        SVG_HUD_SetSpectatorStats(serverEntity);
    else
        SVG_HUD_SetClientStats(serverEntity);

    SVG_HUD_CheckChaseStats(serverEntity);

    clientEntity->SetEvent();

    clientEntity->SetEffects();

    clientEntity->SetSound();

    clientEntity->SetAnimationFrame();

    // Store velocity and view angles.
    client->oldVelocity = clientEntity->GetVelocity();
    client->oldViewAngles = client->playerState.pmove.viewAngles;

    // Reset weapon kicks to zer0.
    client->kickOrigin = vec3_zero();
    client->kickAngles = vec3_zero();

    // if the scoreboard is up, update it
    if (client->showScores && !(level.frameNumber & 31)) {
        SVG_HUD_GenerateDMScoreboardLayout(clientEntity, clientEntity->GetEnemy());
        gi.Unicast(serverEntity, false);
    }
}

//===============
// DefaultGamemode::ClientConnect
// 
// Client is connecting, what do? :)
//===============
qboolean DefaultGamemode::ClientConnect(Entity* serverEntity, char *userinfo) {
    if (!serverEntity) {
        gi.DPrintf("ClientConnect executed with invalid (nullptr) serverEntity");
        return false;
    }
    char    *value;

    // check to see if they are on the banned IP list
    value = Info_ValueForKey(userinfo, "ip");
    if (SVG_FilterPacket(value)) {
        Info_SetValueForKey(userinfo, "rejmsg", "Banned.");
        return false;
    }

    // check for a spectator
    //value = Info_ValueForKey(userinfo, "spectator");
    //if (deathmatch->value && *value && strcmp(value, "0")) {
    //    int i, numspec;

    //    if (*spectator_password->string &&
    //        strcmp(spectator_password->string, "none") &&
    //        strcmp(spectator_password->string, value)) {
    //        Info_SetValueForKey(userinfo, "rejmsg", "Spectator password required or incorrect.");
    //        return qfalse;
    //    }

    //    // count spectators
    //    for (i = numspec = 0; i < maxclients->value; i++)
    //        if (g_edicts[i + 1].inuse && g_edicts[i + 1].client->pers.spectator)
    //            numspec++;

    //    if (numspec >= maxspectators->value) {
    //        Info_SetValueForKey(userinfo, "rejmsg", "Server spectator limit is full.");
    //        return qfalse;
    //    }
    //} else {
        // check for a password
        value = Info_ValueForKey(userinfo, "password");
        if (*password->string && strcmp(password->string, "none") &&
            strcmp(password->string, value)) {
            Info_SetValueForKey(userinfo, "rejmsg", "Password required or incorrect.");
            return false;
        }
    //}
    
    // they can connect
    serverEntity->client = game.clients + (serverEntity - g_entities - 1);

    // if there is already a body waiting for us (a loadgame), just
    // take it, otherwise spawn one from scratch
    if (serverEntity->inUse == false) {
        // clear the respawning variables
        InitializeClientRespawnData(serverEntity->client);
        if (!game.autoSaved || !serverEntity->client->persistent.activeWeapon)
            InitializeClientPersistentData(serverEntity->client);
    }

    ClientUserinfoChanged(serverEntity, userinfo);

    // This is default behaviour for this function.
    if (game.GetMaxClients() > 1)
        gi.DPrintf("%s connected\n", serverEntity->client->persistent.netname);

    // Make sure we start with clean serverFlags.
    serverEntity->serverFlags = 0;
    serverEntity->client->persistent.isConnected = true;

    return true;
}

//===============
// DefaultGamemode::ClientBegin
// 
// Called when a client is ready to be placed in the game after connecting.
//===============
void DefaultGamemode::ClientBegin(Entity* serverEntity) {
    if (!serverEntity) {
        gi.DPrintf("ClientBegin executed with invalid (nullptr) serverEntity");
        return;
    }

    if (serverEntity->client) {
        int32_t entityIndex = serverEntity - g_entities;
        int32_t stateNumber = serverEntity->state.number;

        if (serverEntity == g_entities) {
            gi.DPrintf("serverEntity == g_entities!!!\n");
        }
        gi.DPrintf("ClientBegin - serverEntity#: %i has a client#: %i\n", serverEntity->state.number, serverEntity->client - game.clients);
    } else {
        int32_t entityIndex = serverEntity - g_entities;
        int32_t stateNumber = serverEntity->state.number;

        gi.DPrintf("ClientBegin - serverEntity#: %i has no client.\n", serverEntity->state.number, serverEntity->client - game.clients);
    }
//    gi.DPrintf("ClientBegin executed with entity: %i client: %i\n", serverEntity->state.number, (serverEntity - g_entities - 1));
    // Setup the client for the server entity.
    serverEntity->client = game.clients + (serverEntity - g_entities - 1);

    // We got our own deathmatch mode class, it should copy this function and modify it to tis needs.
    //if (deathmatch->value) {
    //    ClientBeginDeathmatch(ent);
    //    return;
    //}

    // If there is already a body waiting for us (a loadgame), just
    // take it, otherwise spawn one from scratch
    if (serverEntity->inUse == true) {
        // The client has cleared the client side viewAngles upon
        // connecting to the server, which is different than the
        // state when the game is saved, so we need to compensate
        // with deltaangles
        for (int32_t i = 0; i < 3; i++) {
            serverEntity->client->playerState.pmove.deltaAngles[i] = serverEntity->client->playerState.pmove.viewAngles[i];
        }
    } else {
        // Initialize a clean serverEntity.
        SVG_InitEntity(serverEntity);
        
        // Delete previous classentity, if existent (older client perhaps).
        qboolean foundClassEntity = SVG_FreeClassFromEntity(serverEntity);

        // Recreate class SVGBasePlayer entity.
        serverEntity->classEntity = SVG_CreateClassEntity<SVGBasePlayer>(serverEntity, false);

        // Initialize client respawn data.
        InitializeClientRespawnData(serverEntity->client);

        // Put into our server and blast away! (Takes care of spawning classEntity).
        PlaceClientInWorld(serverEntity);
    }

    if (level.intermission.time) {
        HUD_MoveClientToIntermission(serverEntity);
    } else {
        // send effect if in a multiplayer game
        if (game.GetMaxClients() > 1) {
            gi.WriteByte(ServerGameCommands::MuzzleFlash);
            gi.WriteShort(serverEntity - g_entities);
            gi.WriteByte(MuzzleFlashType::Login);
            gi.Multicast(serverEntity->state.origin, MultiCast::PVS);

            gi.BPrintf(PRINT_HIGH, "%s entered the game\n", serverEntity->client->persistent.netname);
        }
    }

    // Call ClientEndServerFrame to update him through the beginning frame.
    ClientEndServerFrame(dynamic_cast<SVGBasePlayer*>(serverEntity->classEntity), serverEntity->client);
}

//===============
// DefaultGamemode::ClientDisconnect.
// 
// Does logic checking for a client's start of a server frame. In case there
// is a "level.intermission.time" set, it'll flat out return.
// 
// This basically allows for the game to disable fetching user input that makes
// our movement tick. And/or shoot weaponry while in intermission time.
//===============
void DefaultGamemode::ClientDisconnect(SVGBasePlayer* player) {
    // Fetch the client.
    ServerClient* client = player->GetClient();

    // Print who disconnected.
    gi.BPrintf(PRINT_HIGH, "%s disconnected\n", client->persistent.netname);

    // Send effect
    if (player->IsInUse()) {
        gi.WriteByte(ServerGameCommands::MuzzleFlash);
        //gi.WriteShort(ent - g_entities);
        gi.WriteShort(player->GetNumber());
        gi.WriteByte(MuzzleFlashType::Logout);
        gi.Multicast(player->GetOrigin(), MultiCast::PVS);
    }

    // Unset this entity, after all, it's about to disconnect so.
    // We don't want it having any model, collision, sound, event, effects...
    // and ensure it is not in use anymore, also change its classname.
    player->UnlinkEntity();
    player->SetModelIndex(0);
    player->Base::SetSound(0);
    player->SetEventID(0);
    player->Base::SetEffects(0);
    player->SetSolid(Solid::Not);
    player->SetInUse(false);
    player->SetClassname("disconnected");

    // Ensure a state is stored for that this client is not connected anymore.
    client->persistent.isConnected = false;

    // FIXME: don't break skins on corpses, etc
    //playernum = ent-g_entities-1;
    //gi.configstring (ConfigStrings::PlayerSkins+playernum, "");
}

//===============
// DefaultGamemode::ClientUserinfoChanged
// 
//===============
void DefaultGamemode::ClientUserinfoChanged(Entity* ent, char* userinfo) {
    char    *s;
    int     playernum;

    // check for malformed or illegal info strings
    if (!Info_Validate(userinfo)) {
        strcpy(userinfo, "\\name\\badinfo\\skin\\male/grunt");
    }

    // set name
    s = Info_ValueForKey(userinfo, "name");
    strncpy(ent->client->persistent.netname, s, sizeof(ent->client->persistent.netname) - 1);

    // Set spectator to false.
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
// DefaultGamemode::ClientUpdateObituary
// 
//===============
void DefaultGamemode::ClientUpdateObituary(SVGBaseEntity* self, SVGBaseEntity* inflictor, SVGBaseEntity* attacker) {
    std::string message = ""; // String stating what happened to whichever entity. "suicides", "was squished" etc.
    std::string messageAddition = ""; // String stating what is additioned to it, "'s shrapnell" etc. Funny stuff.

                                      // Goes to COOP GAME MODE.
                                      //if (coop->value && attacker->GetClient())
                                      //    meansOfDeath |= MeansOfDeath::FriendlyFire;

    qboolean friendlyFire = meansOfDeath & MeansOfDeath::FriendlyFire;
    int32_t finalMeansOfDeath = meansOfDeath & ~MeansOfDeath::FriendlyFire;

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
    if (message != "") {
        gi.BPrintf(PRINT_MEDIUM, "%s %s.\n", self->GetClient()->persistent.netname, message.c_str());
        // WID: We can uncomment these in case we end up making a SinglePlayerMode after all.
        //if (deathmatch->value)
        //    self->GetClient()->respawn.score--;
        self->SetEnemy(NULL);
        return;
    }

    // Set the attacker to self.
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
        if (message != "") {
            gi.BPrintf(PRINT_MEDIUM, "%s %s %s%s\n", self->GetClient()->persistent.netname, message.c_str(), attacker->GetClient()->persistent.netname, messageAddition.c_str());
            // WID: We can uncomment these in case we end up making a SinglePlayerMode after all.
            //if (deathmatch->value) {
            //    if (friendlyFire)
            //        attacker->GetClient()->respawn.score--;
            //    else
            //        attacker->GetClient()->respawn.score++;
            //}
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
        //    if (deathmatch->value) {
        //        if (friendlyFire)
        //            attacker->GetClient()->respawn.score--;
        //        else
        //            attacker->GetClient()->respawn.score++;
        //    }
        //    return;
        }
    }

    // 
    gi.BPrintf(PRINT_MEDIUM, "%s died.\n", self->GetClient()->persistent.netname);

    // WID: We can uncomment these in case we end up making a SinglePlayerMode after all.
    //if (deathmatch->value)
    //    self->GetClient()->respawn.score--;
}


void DefaultGamemode::InitializeClientPersistentData(ServerClient* client) {
    // Can't go on without a valid client.
    if (!client) {
        return;
    }

    // Reset its persistent data, we're initializing it for this client.
    client->persistent = {};

    // Give the client a default starter weapon to spawn with.
    //gitem_t *item = SVG_FindItemByPickupName("Blaster");
    client->persistent.selectedItem = 0;//ITEM_INDEX(item);
    client->persistent.inventory[client->persistent.selectedItem] = 1;

    // Reset active weapon to nothing.
    client->persistent.activeWeapon = nullptr;

    // Reset health and max health.
    client->persistent.health       = 100;
    client->persistent.maxHealth    = 100;

    // Reset maximum values of inventory.
    client->persistent.maxBullets   = 200;
    client->persistent.maxShells    = 100;
    client->persistent.maxRockets   = 50;
    client->persistent.maxGrenades  = 50;
    client->persistent.maxCells     = 200;
    client->persistent.maxSlugs     = 50;

    // This client is connected.
    client->persistent.isConnected = true;
}

void DefaultGamemode::InitializeClientRespawnData(ServerClient* client) {
    if (!client)
        return;

    client->respawn = {};
    client->respawn.enterGameFrameNumber = level.frameNumber;
    client->respawn.persistentCoopRespawn = client->persistent;
}

//===============
// DefaultGamemode::SelectClientSpawnPoint
//
// Choose any info_player_start or its derivates, it'll do a subclassof check, so the only valid classnames are
// those who have inherited from info_player_start. (info_player_deathmatch, etc).
//===============
void DefaultGamemode::SelectClientSpawnPoint(Entity* ent, vec3_t& origin, vec3_t& angles, const std::string& classname) {
    SVGBaseEntity *spawnPoint = nullptr;

    //// Find a single player start spot
    if (!spawnPoint) {
        // Find a spawnpoint that has a target:
        for (auto* result : g_baseEntities | bef::Standard | bef::IsSubclassOf<InfoPlayerStart>()) {
            // Continue in case there is no comparison to it with the possible target
            // of the InfoPlayerStart
            if (!game.spawnpoint[0])
                continue;

            if (result->GetTargetName() == game.spawnpoint) {
                spawnPoint = result;
                break;
            }
        }
    }

    // Since we still haven't found one with a target, do it again, but this time without
    // a target requirement.
    if (!spawnPoint) {
        for (auto* result : g_baseEntities | bef::Standard | bef::IsSubclassOf<InfoPlayerStart>()) {
            if (result) {
                spawnPoint = result;
                break;
            }
        }
    }

    // Setup player origin and angles, also raise him 9 units above the ground to be sure it fits.
    if (spawnPoint) {
        origin = spawnPoint->GetOrigin();
        origin.z += 9;
        angles = spawnPoint->GetAngles();
    } else {
        gi.Error("Couldn't find spawn point %s", game.spawnpoint);
    }
}

//===============
// DefaultGamemode::PlaceClientInWorld
// 
// Called when a player connects to a single and multiplayer. 
// 
// #1. In the case of a SP mode death, the loadmenu pops up and selecting a load game
// will restart the server.
// #2. In thecase of a MP mode death however, after a small intermission time, it'll
// call this function again to respawn our player.
//===============
void DefaultGamemode::PlaceClientInWorld(Entity *ent) {
    // Find a spawn point
    vec3_t  spawnOrigin = vec3_zero();
    vec3_t  spawnAngles = vec3_zero();

    SelectClientSpawnPoint(ent, spawnOrigin, spawnAngles, "info_player_start");

    // Fetch the client index, and the client right off the bat.
    int32_t clientIndex = ent - g_entities - 1;
    ServerClient* client = ent->client;

    // Client user info.
    char userinfo[MAX_INFO_STRING];
    // Store a copy of our respawn data for later use. 
    ClientRespawnData respawnData = client->respawn;
    // Copy over client's user info into our userinfo buffer.
    memcpy(userinfo, client->persistent.userinfo, sizeof(userinfo));

    // If we had persistent coop respawn data, be sure to back it up right now.
    client->persistent = respawnData.persistentCoopRespawn;
    // Inform of a client user info change.
    ClientUserinfoChanged(ent, userinfo);
    // In case the score was higher, be sure to update it.
    if (respawnData.score > client->persistent.score) {
	    client->persistent.score = respawnData.score;
    }

    // Backup the current client persistent data.
    ClientPersistentData persistentData = client->persistent;
    // Reset the client's information.
    *client = {};
    // Now move its persistent data back into the client's information.
    client->persistent = persistentData;
    // In case the persistent data consists of a dead client, reinitialize it.
    if (client->persistent.health <= 0) {
	    InitializeClientPersistentData(client);
    }
    // Last but not least, set its respawn data.
    client->respawn = respawnData;

    // Copy some data from the client to the entity
    FetchClientEntityData(ent);

    // Spawn the client again using spawn instead of respawn. (Respawn serves a different use.)
    SVGBasePlayer* clientEntity = dynamic_cast<SVGBasePlayer*>(ent->classEntity);
    clientEntity->Spawn();

    // Update the client pointer this entity belongs to.
    client = &game.clients[clientIndex];
    clientEntity->SetClient(client);
 
    // Clear playerstate values.
    client->playerState = {};

    // Setup player move origin to spawnpoint origin.
    client->playerState.pmove.origin = spawnOrigin;

    if (((int)gamemodeflags->value & GamemodeFlags::FixedFOV)) {
        client->playerState.fov = 90;
    } else {
        client->playerState.fov = atoi(Info_ValueForKey(client->persistent.userinfo, "fov"));
        if (client->playerState.fov < 1) {
            client->playerState.fov = 90;
        } else if (client->playerState.fov > 160) {
            client->playerState.fov = 160;
        }
    }

    // Set entity state origins and angles.
    clientEntity->SetOrigin(spawnOrigin + vec3_t { 0.f, 0.f, 1.f });
    clientEntity->SetOldOrigin(clientEntity->GetOrigin());
    clientEntity->SetAngles(vec3_t { 0.f, spawnAngles[vec3_t::Yaw], 0.f });

    // Set client and player move state angles.
    client->playerState.pmove.deltaAngles = spawnAngles - client->respawn.commandViewAngles;
    client->playerState.pmove.viewAngles = clientEntity->GetAngles();
    client->aimAngles = clientEntity->GetAngles();

    // spawn a spectator in case the client was/is one.
    if (client->persistent.isSpectator) {
        // Nodefault chase target.
        client->chaseTarget = nullptr;

        // Well we knew this but store it in respawn data too.
        client->respawn.isSpectator = true;

        // Movement type is the obvious spectator.
        clientEntity->SetMoveType(MoveType::Spectator);

        // No solid.
        clientEntity->SetSolid(Solid::BoundingBox);

        // NoClient flag, aka, do not send this entity to other clients. It is invisible to them.
    	clientEntity->SetServerFlags(clientEntity->GetServerFlags() | EntityServerFlags::NoClient);

        // Ensure it has no gun index, spectators can't shoot after all.
        client->playerState.gunIndex = 0;

        // Last but not least link our entity.
        clientEntity->LinkEntity();
        
        // We're done in case of spawning a spectator.
        return;
    } else {
        // Let it be known to respawn that we are not in spectator mode.
        client->respawn.isSpectator = false;
    }

    // Make sure we can spawn.
    if (!SVG_KillBox(clientEntity)) {
        // could't spawn in?
    }

    // Link our entity.
    clientEntity->LinkEntity();

    // Set player state gun index to whichever was persistent in the previous map (if there was one).
    client->playerState.gunIndex = gi.ModelIndex("models/weapons/v_mark23/tris.iqm");  //gi.ModelIndex(client->persistent.activeWeapon->viewModel);

    // Set its current new weapon to the one that was stored in persistent and activate it.
    client->newWeapon = client->persistent.activeWeapon;
    SVG_ChangeWeapon(clientEntity);
}

//===============
// DefaultGamemode::RespawnClient
// 
// Since the default game mode is intended to be a single player mode,
// there is no respawning and we show a loadgame menu instead.
//===============
void DefaultGamemode::RespawnClient(SVGBasePlayer* ent) {
    // Kept around here to port later to other gamemodes.
    //if (deathmatch->value || coop->value) {
    //    // Spectator's don't leave bodies
    //    if (self->classEntity->GetMoveType() != MoveType::NoClip && self->classEntity->GetMoveType() != MoveType::Spectator)
    //        game.GetCurrentGamemode()->SpawnClientCorpse(self->classEntity);

    //    self->serverFlags &= ~EntityServerFlags::NoClient;
    //    game.GetCurrentGamemode()->PlaceClientInWorld((SVGBasePlayer*)self->classEntity);

    //    // add a teleportation effect
    //    self->state.eventID = EntityEvent::PlayerTeleport;

    //    // hold in place briefly
    //    self->client->playerState.pmove.flags = PMF_TIME_TELEPORT;
    //    self->client->playerState.pmove.time = 14;

    //    self->client->respawnTime = level.time;

    //    return;
    //}

    // Restart the entire server by letting them pick a loadgame.
    // This is for singleplayer mode.
    gi.AddCommandString("pushmenu loadgame\n");
}

//===============
// DefaultGamemode::RespawnAllClients
//
//===============
void DefaultGamemode::RespawnAllClients() {
    // Do nothing for default game mode.
}

//===============
// DefaultGamemode::ClientDeath
// 
// Does nothing for this game mode.
//===============
void DefaultGamemode::ClientDeath(SVGBasePlayer *clientEntity) {

}

//===============
// DefaultGamemode::SaveClientEntityData
// 
// Some information that should be persistant, like health,
// is still stored in the edict structure, so it needs to
// be mirrored out to the client structure before all the
// edicts are wiped.
//===============
void DefaultGamemode::SaveClientEntityData(void) {
    for (int32_t i = 0 ; i < game.GetMaxClients(); i++) {
        Entity *entity = &g_entities[1 + i];
        if (!entity->inUse)
            continue;
        if (!entity->classEntity)
            continue;
        game.clients[i].persistent.health = entity->classEntity->GetHealth();
        game.clients[i].persistent.maxHealth = entity->classEntity->GetMaxHealth();
        game.clients[i].persistent.savedFlags = (entity->classEntity->GetFlags() & (EntityFlags::GodMode | EntityFlags::NoTarget | EntityFlags::PowerArmor));
    }
}

//===============
// DefaultGamemode::RespawnClient
// 
// // Fetch client data that was stored between previous entity wipe session
//===============
void DefaultGamemode::FetchClientEntityData(Entity* ent) {
    if (!ent)
        return;

    if (!ent->classEntity)
        return;

    ent->classEntity->SetHealth(ent->client->persistent.health);
    ent->classEntity->SetMaxHealth(ent->client->persistent.maxHealth);
    ent->classEntity->SetFlags(ent->classEntity->GetFlags() | ent->client->persistent.savedFlags);
}