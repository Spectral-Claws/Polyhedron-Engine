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

#include "g_local.h"          // Include SVGame header.
#include "entities.h"         // Entities.
#include "player/client.h"    // Include Player Client header.

typedef struct {
    const char    *name; // C++20: STRING: Added const
    void (*spawn)(ServerEntity *ent);
} spawn_func_t;

typedef struct {
    const char    *name; // C++20: STRING: Added const
    size_t  ofs;
    fieldtype_t type;
} spawn_field_t;

static const spawn_func_t spawn_funcs[] = {
    //{"item_health", SP_item_health},
    //{"item_health_small", SP_item_health_small},

    {NULL, NULL}
};

static const spawn_field_t spawn_fields[] = {
//    {"classname", FOFS(className), F_LSTRING},
////    {"model", FOFS(model), F_LSTRING},
////    {"spawnflags", FOFS(spawnFlags), F_INT},
//    {"speed", FOFS(speed), F_FLOAT},
//    {"acceleration", FOFS(acceleration), F_FLOAT},
//    {"deceleration", FOFS(deceleration), F_FLOAT},
//    {"target", FOFS(target), F_LSTRING},
//    {"targetName", FOFS(targetName), F_LSTRING},
//    {"pathtarget", FOFS(pathTarget), F_LSTRING},
//    {"killtarget", FOFS(killTarget), F_LSTRING},
//    {"message", FOFS(message), F_LSTRING},
//    {"team", FOFS(team), F_LSTRING},
////    {"wait", FOFS(wait), F_FLOAT},
////    {"delay", FOFS(delay), F_FLOAT},
//    {"random", FOFS(random), F_FLOAT},
//    {"moveorigin", FOFS(moveOrigin), F_VECTOR},
//    {"moveangles", FOFS(moveAngles), F_VECTOR},
//    {"style", FOFS(style), F_INT},
//    {"customLightStyle", FOFS(customLightStyle), F_LSTRING},
//    {"count", FOFS(count), F_INT},
////    {"health", FOFS(health), F_INT},
////    {"sounds", FOFS(sounds), F_INT},
//    {"light", 0, F_IGNORE},
////    {"damage", FOFS(damage), F_INT},
////    {"mass", FOFS(mass), F_INT},
//    {"volume", FOFS(volume), F_FLOAT},
//    {"attenuation", FOFS(attenuation), F_FLOAT},
//    {"map", FOFS(map), F_LSTRING},
//    //{"origin", FOFS(state.origin), F_VECTOR},
//    //{"angles", FOFS(state.angles), F_VECTOR},
//    //{"angle", FOFS(state.angles), F_ANGLEHACK},

    {NULL}
};

// temp spawn vars -- only valid when the spawn function is called
static const spawn_field_t temp_fields[] = {
    //{"lip", STOFS(lip), F_INT},
    //{"distance", STOFS(distance), F_INT},
    //{"height", STOFS(height), F_INT},
    //{"noise", STOFS(noise), F_LSTRING},
    //{"pausetime", STOFS(pausetime), F_FLOAT},
    //{"item", STOFS(item), F_LSTRING},

    //{"gravity", STOFS(gravity), F_LSTRING},
    //{"sky", STOFS(sky), F_LSTRING},
    //{"skyrotate", STOFS(skyrotate), F_FLOAT},
    //{"skyaxis", STOFS(skyaxis), F_VECTOR},
    //{"minyaw", STOFS(minyaw), F_FLOAT},
    //{"maxyaw", STOFS(maxyaw), F_FLOAT},
    //{"minpitch", STOFS(minpitch), F_FLOAT},
    //{"maxpitch", STOFS(maxpitch), F_FLOAT},
    //{"nextmap", STOFS(nextMap), F_LSTRING},

    {NULL}
};

//
// SVG_SpawnServerGameEntity
//
//
#include "shared/entities/Server/ServerGameEntity.h"

/*
=============
ED_NewString
=============
*/
static char* ED_NewString(const char* string) {
    char* newb, * new_p;
    int     i, l;

    l = strlen(string) + 1;

    newb = (char*)gi.TagMalloc(l, TAG_LEVEL); // CPP: Cast

    new_p = newb;

    for (i = 0; i < l; i++) {
        if (string[i] == '\\' && i < l - 1) {
            i++;
            if (string[i] == 'n')
                *new_p++ = '\n';
            else
                *new_p++ = '\\';
        } else
            *new_p++ = string[i];
    }

    return newb;
}

/*
===============
ED_CallSpawn

Allocates the proper server game ServerEntity class. Then spawns the ServerEntity.
===============
*/
void ED_CallSpawn(ServerEntity *ent)
{
    // Fetch key/value dictionary.
    auto dictionary = &ent->serverEntityDictionary;
    ent->className = ED_NewString( ent->serverEntityDictionary["classname"].c_str() );
    
    ServerGameEntity *gameEntity = SVG_SpawnServerGameEntity( ent, ent->className );

    // If we did not find the classname, then give up
    if ( gameEntity != nullptr ) {
        SVG_FreeServerEntity( ent );
        return;
    }
    // Initialise the ServerEntity with its respected keyvalue properties
    for ( const auto& keyValueEntry : ent->serverEntityDictionary ) {
        gameEntity->SpawnKey( keyValueEntry.first, keyValueEntry.second );
    }
    // Precache and spawn, to set the ServerEntity up
    gameEntity->Precache();
    gameEntity->Spawn();
}

/*
====================
ED_ParseEdict

Parses an edict out of the given string, returning the new position
ed should be a properly initialized empty edict.
====================
*/
void ED_ParseServerEntity(const char** data, ServerEntity* ent) {
    qboolean    init;
    char* key, * value;

    init = false;
    st = {};

    // go through all the dictionary pairs
    while (1) {
        // parse key
        key = COM_Parse(data);
        if (key[0] == '}')
            break;
        if (!*data)
            gi.Error("%s: EOF without closing brace", __func__);

        // parse value
        value = COM_Parse(data);
        if (!*data)
            gi.Error("%s: EOF without closing brace", __func__);

        if (value[0] == '}')
            gi.Error("%s: closing brace without data", __func__);

        init = true;

        // keynames with a leading underscore are used for utility comments,
        // and are immediately discarded by quake
        if (key[0] == '_')
            continue;

        ent->serverEntityDictionary[key] = value;
        //if (!ED_ParseField(spawn_fields, key, value, (byte *)ent)) {
        //    if (!ED_ParseField(temp_fields, key, value, (byte *)&st)) {
        //        gi.DPrintf("%s: %s is not a field\n", __func__, key);
        //    }
        //}
    }

    if (!init)
        ent = {};
}


/*
===============
ED_ParseField

Takes a key/value pair and sets the binary values
in an edict
===============
*/
//static qboolean ED_ParseField(const spawn_field_t *fields, const char *key, const char *value, byte *b)
//{
//    const spawn_field_t *f;
//    float   v;
//    vec3_t  vec;
//
//    for (f = fields ; f->name ; f++) {
//        if (!Q_stricmp(f->name, key)) {
//            // found it
//            switch (f->type) {
//            case F_LSTRING:
//                *(char **)(b + f->ofs) = ED_NewString(value);
//                break;
//            case F_VECTOR:
//                if (sscanf(value, "%f %f %f", &vec[0], &vec[1], &vec[2]) != 3) {
//                    gi.DPrintf("%s: couldn't parse '%s'\n", __func__, key);
//                    VectorClear(vec);
//                }
//                ((float *)(b + f->ofs))[0] = vec[0];
//                ((float *)(b + f->ofs))[1] = vec[1];
//                ((float *)(b + f->ofs))[2] = vec[2];
//                break;
//            case F_INT:
//                *(int *)(b + f->ofs) = atoi(value);
//                break;
//            case F_FLOAT:
//                *(float *)(b + f->ofs) = atof(value);
//                break;
//            case F_ANGLEHACK:
//                v = atof(value);
//                ((float *)(b + f->ofs))[0] = 0;
//                ((float *)(b + f->ofs))[1] = v;
//                ((float *)(b + f->ofs))[2] = 0;
//                break;
//            case F_IGNORE:
//                break;
//            default:
//                break;
//            }
//            return true;
//        }
//    }
//    return false;
//}

/*
================
SVG_FindTeams

Chain together all entities with a matching team field.

All but the first will have the ServerEntityFlags::TeamSlave flag set.
All but the last will have the teamchain field set to the next one
================
*/
void SVG_FindTeams(void)
{
    ServerEntity* e, * e2;
    ServerGameEntity *chain;
    int     i, j;
    int     c, c2;

    c = 0;
    c2 = 0;
    for (i = 1, e = g_entities + i; i < globals.numberOfEntities; i++, e++) {
        // Fetch class ServerEntity.
        ServerGameEntity *classServerEntity = g_baseEntities[e->state.number];

        if (classServerEntity == NULL)
            continue;

        if (!classServerEntity->IsInUse())
            continue;
        if (!classServerEntity->GetTeam())
            continue;
        if (classServerEntity->GetFlags() & ServerEntityFlags::TeamSlave)
            continue;
        chain = classServerEntity;
        classServerEntity->SetTeamMasterServerEntity(classServerEntity);
        c++;
        c2++;
        for (j = i + 1, e2 = e + 1 ; j < globals.numberOfEntities ; j++, e2++) {
            // Fetch class ServerEntity.
            ServerGameEntity* classServerEntity2 = g_baseEntities[e->state.number];

            if (classServerEntity2 == NULL)
                continue;

            if (!classServerEntity2->IsInUse())
                continue;
            if (!classServerEntity2->GetTeam())
                continue;
            if (classServerEntity2->GetFlags() & ServerEntityFlags::TeamSlave)
                continue;
            if (!strcmp(classServerEntity->GetTeam(), classServerEntity2->GetTeam())) {
                c2++;
                chain->SetTeamChainServerEntity(classServerEntity2);
                classServerEntity2->SetTeamMasterServerEntity(classServerEntity);
                chain = classServerEntity2;
                classServerEntity2->SetFlags(classServerEntity2->GetFlags() | ServerEntityFlags::TeamSlave);
            }
        }
    }

    gi.DPrintf("%i teams with %i entities\n", c, c2);
}


/*
==============
SVG_SpawnEntities

Creates a server's ServerEntity / program execution context by
parsing textual ServerEntity definitions out of an ent file.
==============
*/
extern void SVG_AllocateGamePlayerClientEntities();

void SVG_SpawnEntities(const char *mapName, const char *entities, const char *spawnpoint)
{
    ServerEntity     *ent;
    int         inhibit;
    char        *com_token;
    int         i;
    float       skill_level;

    // Do a skill check.
    skill_level = floor(skill->value);
    if (skill_level < 0)
        skill_level = 0;
    if (skill_level > 3)
        skill_level = 3;
    if (skill->value != skill_level)
        gi.cvar_forceset("skill", va("%f", skill_level));

    // Save client data.
    SVG_SaveClientData();

    // Free level tag allocated data.
    gi.FreeTags(TAG_LEVEL);

    // Clear level state.
    level = {};

    // Clear out entities.
    for (int32_t i = 0; i < game.maxEntities; i++) {
        // Delete class entities, if any.
        if (gameEntities[i]) {
            delete gameEntities[i];
            gameEntities[i] = NULL;
        }

        //g_entities[i] = {};
        //gi.ResetServerEntity(i);
    }

    strncpy(level.mapName, mapName, sizeof(level.mapName) - 1);
    strncpy(game.spawnpoint, spawnpoint, sizeof(game.spawnpoint) - 1);

    // Set client fields on player ents
    for (i = 0; i < game.maximumClients; i++)
        gameEntities[i + 1]->SetClient(game.clients[i]);

    ent = NULL;
    inhibit = 0;

// parse ents
    while (1) {
        // parse the opening brace
        com_token = COM_Parse(&entities);
        if (!entities)
            break;
        if (com_token[0] != '{')
            gi.Error("ED_LoadFromFile: found %s when expecting {", com_token);

        if (!ent)
            ent = gameEntities;
        else
            ent = SVG_Spawn();
        ED_ParseServerEntity(&entities, ent);

        //// yet another map hack
        //if (!Q_stricmp(level.mapName, "command") && !Q_stricmp(ent->className, "trigger_once") && !Q_stricmp(ent->model, "*27"))
        //    ent->spawnFlags &= ~ServerEntitySpawnFlags::NotHard;

        //// remove things (except the world) from different skill levels or deathmatch
        //if (ent != g_entities) {
        //    // Do a check for deathmatch, in case the ServerEntity isn't allowed there.
        //    if (deathmatch->value) {
        //        if (ent->spawnFlags & ServerEntitySpawnFlags::NotDeathMatch) {
        //            SVG_FreeServerEntity(ent);
        //            inhibit++;
        //            continue;
        //        }
        //    } else {
        //        if ( /* ((coop->value) && (ent->spawnFlags & ServerEntitySpawnFlags::NotCoop)) || */
        //            ((skill->value == 0) && (ent->spawnFlags & ServerEntitySpawnFlags::NotEasy)) ||
        //            ((skill->value == 1) && (ent->spawnFlags & ServerEntitySpawnFlags::NotMedium)) ||
        //            (((skill->value == 2) || (skill->value == 3)) && (ent->spawnFlags & ServerEntitySpawnFlags::NotHard))
        //        ) {
        //            SVG_FreeServerEntity(ent);
        //            inhibit++;
        //            continue;
        //        }
        //    }

        //    ent->spawnFlags &= ~(ServerEntitySpawnFlags::NotEasy | ServerEntitySpawnFlags::NotMedium | ServerEntitySpawnFlags::NotHard | ServerEntitySpawnFlags::NotCoop | ServerEntitySpawnFlags::NotDeathMatch);
        //}

        // Allocate the class ServerEntity, and call its spawn.
        ED_CallSpawn(ent);
    }

    // Post spawn entities.
    for (int32_t i = 0; i < MAX_EDICTS; i++) {
        if (gameEntities[i])
            gameEntities[i]->PostSpawn();
    }

    // Spawn PlayerClient entities first.
    // WID: LAME HACK...
    SVG_AllocateGamePlayerClientEntities();

    gi.DPrintf("%i entities inhibited\n", inhibit);

#ifdef DEBUG
    i = 1;
    ent = EDICT_NUM(i);
    while (i < globals.pool.numberOfEntities) {
        if (ent->inUse != 0 || ent->inUse != 1)
            Com_DPrintf("Invalid ServerEntity %d\n", i);
        i++, ent++;
    }
#endif

    SVG_FindTeams();

    SVG_PlayerTrail_Init();
}


//===================================================================

#if 0
// cursor positioning
xl <value>
xr <value>
yb <value>
yt <value>
xv <value>
yv <value>

// drawing
statpic <name>
pic <stat>
num <fieldwidth> <stat>
string <stat>

// control
if <stat>
ifeq <stat> <value>
ifbit <stat> <value>
endif

#endif
