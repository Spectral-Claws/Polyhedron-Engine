/*
// LICENSE HERE.

//
// DebrisEntity.h
//
// Base entity for debris.
//
*/
#ifndef __SVGAME_ENTITIES_BASE_DEBRISENTITY_H__
#define __SVGAME_ENTITIES_BASE_DEBRISENTITY_H__

class SynchedEntityBase;

class DebrisEntity : public SynchedEntityBase{
public:
    // Constructor/Deconstructor.
    DebrisEntity(ServerEntity* svEntity);
    virtual ~DebrisEntity();

    DefineClass(DebrisEntity, SynchedEntityBase);

    //
    // Interface functions. 
    //
    void Precache() override;    // Precaches data.
    void Spawn() override;       // Spawns the entity.
    void Respawn() override;     // Respawns the entity.
    void PostSpawn() override;   // PostSpawning is for handling entity references, since they may not exist yet during a spawn period.
    void Think() override;       // General entity thinking routine.

    void SpawnKey(const std::string& key, const std::string& value)  override;

    //
    // DebrisEntity functions.
    // 
    void CalculateVelocity(SynchedEntityBase * other, const int32_t& damage);

    //
    // Callback functions.
    //
    //
    void DebrisEntityDie(SynchedEntityBase * inflictor, SynchedEntityBase * attacker, int damage, const vec3_t& point);
   
protected:

private:

};

#endif // __SVGAME_ENTITIES_BASE_DEBRISENTITY_H__