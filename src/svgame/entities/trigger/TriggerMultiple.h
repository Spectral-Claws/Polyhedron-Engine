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

class TriggerMultiple : public SVGBaseTrigger {
public:
    //
    // Constructor/Deconstructor.
    //
    TriggerMultiple(ServerEntity* svServerEntity);
    virtual ~TriggerMultiple();

    DefineMapClass( "trigger_multiple", TriggerMultiple, SVGBaseTrigger );

    //
    // Interface functions. 
    //
    void Precache() override;    // Precaches data.
    void Spawn() override;       // Spawns the ServerEntity.
    void Respawn() override;     // Respawns the ServerEntity.
    void PostSpawn() override;   // PostSpawning is for handling ServerEntity references, since they may not exist yet during a spawn period.
    void Think() override;       // General ServerEntity thinking routine.

    void SpawnKey(const std::string& key, const std::string& value)  override;

    //
    // Trigger functions.
    //

    // Callback functions.
    void TriggerMultipleThinkWait(void);
    void TriggerMultipleTouch(SVGBaseEntity* self, SVGBaseEntity* other, cplane_t* plane, csurface_t* surf);
    void TriggerMultipleEnable(SVGBaseEntity* other, SVGBaseEntity* activator);
    void TriggerMultipleUse(SVGBaseEntity* other, SVGBaseEntity* activator);

    //
    // Get/Set
    // 

protected:
    //
    // Trigger function.
    //
    void Trigger(SVGBaseEntity* activator);

    //
    // Other base ServerEntity members. (These were old fields in edict_T back in the day.)
    //
    // The time this ServerEntity has last been hurting anyone else. It is used for the slow damage flag.
    //float lastHurtTime;

    //
    // ServerEntity pointers.
    // 

public:

};

#endif // __SVGAME_ENTITIES_TRIGGER_TRIGGERMULTIPLE_H__