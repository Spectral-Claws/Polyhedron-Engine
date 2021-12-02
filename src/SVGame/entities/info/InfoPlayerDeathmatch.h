/*
// LICENSE HERE.

//
// InfoPlayerDeathmatch.h
//
//
*/
#pragma once

struct ServerEntity;
class EntityBase;
class SynchedEntityBase;
class InfoPlayerStart;

class InfoPlayerDeathmatch : public InfoPlayerStart {
public:
    // Constructor/Deconstructor.
    InfoPlayerDeathmatch(ServerEntity* svEntity);
    virtual ~InfoPlayerDeathmatch();

    DefineMapClassSelfConstruct( "info_player_deathmatch", InfoPlayerDeathmatch, InfoPlayerStart );

    // Interface functions. 
    void Precache();    // Precaches data.
    void Spawn();       // Spawns the entity.
    void PostSpawn();   // PostSpawning is for handling entity references, since they may not exist yet during a spawn period.
    void Think();       // General entity thinking routine.

    void SpawnKey(const std::string& key, const std::string& value)  override;

private:

};