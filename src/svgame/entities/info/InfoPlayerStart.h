/*
// LICENSE HERE.

//
// InfoPlayerStart.h
//
//
*/
#pragma once

//class SVGBaseEntity;
#include ""

class InfoPlayerStart : public SVGBaseEntity {
public:
    // Constructor/Deconstructor.
    InfoPlayerStart(ServerEntity* svServerEntity);
    virtual ~InfoPlayerStart();

    DefineMapClass( "info_player_start", InfoPlayerStart, SVGBaseEntity );

    // Interface functions. 
    void Precache();    // Precaches data.
    void Spawn();       // Spawns the ServerEntity.
    void PostSpawn();   // PostSpawning is for handling ServerEntity references, since they may not exist yet during a spawn period.
    void Think();       // General ServerEntity thinking routine.

    void SpawnKey(const std::string& key, const std::string& value)  override;

private:

};
