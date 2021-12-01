/*
// LICENSE HERE.

//
// TriggerMultiple.h
//
// This trigger will always fire.  It is activated by the world.
//
*/
#ifndef __SVGAME_ENTITIES_TRIGGER_TRIGGERMULTIPLE_H__
#define __SVGAME_ENTITIES_TRIGGER_TRIGGERMULTIPLE_H__

class TriggerMultiple : public BaseTrigger {
public:
    //
    // Constructor/Deconstructor.
    //
    TriggerMultiple(ServerEntity* svEntity);
    virtual ~TriggerMultiple();

    DefineMapClass( "trigger_multiple", TriggerMultiple, BaseTrigger );

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
    void TriggerMultipleThinkWait(void);
    void TriggerMultipleTouch(SynchedEntityBase * self, SynchedEntityBase * other, cplane_t* plane, csurface_t* surf);
    void TriggerMultipleEnable(SynchedEntityBase * other, SynchedEntityBase * activator);
    void TriggerMultipleUse(SynchedEntityBase * other, SynchedEntityBase * activator);

    //
    // Get/Set
    // 

protected:
    //
    // Trigger function.
    //
    void Trigger(SynchedEntityBase * activator);

    //
    // Other base entity members. (These were old fields in edict_T back in the day.)
    //
    // The time this entity has last been hurting anyone else. It is used for the slow damage flag.
    //float lastHurtTime;

    //
    // ServerEntity pointers.
    // 

public:

};

#endif // __SVGAME_ENTITIES_TRIGGER_TRIGGERMULTIPLE_H__