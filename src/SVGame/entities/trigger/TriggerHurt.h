/*
// LICENSE HERE.

//
// TriggerHurt.h
//
// Trigger Hurt brush entity.
//
*/
#ifndef __SVGAME_ENTITIES_TRIGGER_TRIGGERHURT_H__
#define __SVGAME_ENTITIES_TRIGGER_TRIGGERHURT_H__

class TriggerHurt : public BaseTrigger {
public:
    //
    // Constructor/Deconstructor.
    //
    TriggerHurt(ServerEntity* svEntity);
    virtual ~TriggerHurt();

    DefineMapClass( "trigger_hurt", TriggerHurt, BaseTrigger );

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
    // Trigger functions.
    //

    // Callback functions.
    void TriggerHurtTouch(SynchedEntityBase * self, SynchedEntityBase * other, cplane_t* plane, csurface_t* surf);
    void TriggerHurtUse(SynchedEntityBase * other, SynchedEntityBase * activator);

    //
    // Get/Set
    // 

protected:
    //
    // Other base entity members. (These were old fields in edict_t back in the day.)
    //
    // The time this entity has last been hurting anyone else. It is used for the slow damage flag.
    float lastHurtTime;

    //
    // ServerEntity pointers.
    // 

public:

};

#endif // __SVGAME_ENTITIES_TRIGGER_TRIGGERHURT_H__