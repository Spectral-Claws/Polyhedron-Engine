/*
// LICENSE HERE.

//
// ItemMegaHealth.h
//
//
*/
#pragma once

class SVGBaseEntity;
class SVGBaseTrigger;
class SVGBaseItem;

class ItemHealthMega : public SVGBaseItem {
public:
    // Constructor/Deconstructor.
    ItemHealthMega(Entity* svEntity);
    virtual ~ItemHealthMega();

    DefineMapClass( "item_health_mega", ItemHealthMega, SVGBaseItem);

    // Item flags
    static constexpr int32_t IF_IgnoreMaxHealth     = 1 << 0;
    static constexpr int32_t IF_TimedHealth         = 1 << 1;

    //
    // Interface functions. 
    //
    virtual void Precache() override;    // Precaches data.
    virtual void Spawn() override;       // Spawns the entity.
    virtual void Respawn() override;     // Respawns the entity.
    virtual void PostSpawn() override;   // PostSpawning is for handling entity references, since they may not exist yet during a spawn period.
    virtual void Think() override;       // General entity thinking routine.

    //
    // Callback Functions.
    //
    //void HealthMegaUse( SVGBaseEntity* caller, SVGBaseEntity* activator );
    void HealthMegaThink(void);
    //void HealthMegaDie(SVGBaseEntity* inflictor, SVGBaseEntity* attacker, int damage, const vec3_t& point);
    //void HealthMegaTouch(SVGBaseEntity* self, SVGBaseEntity* other, cplane_t* plane, csurface_t* surf);
    qboolean HealthMegaPickup(SVGBaseEntity* other);

private:

};