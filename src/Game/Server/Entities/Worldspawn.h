/*
// LICENSE HERE.

//
// Worldspawn.h
//
// Worldspawn entity definition.
//
*/
#pragma once

class SVGBaseEntity;

class Worldspawn : public SVGBaseEntity {
public:
    //! Constructor/Deconstructor.
    Worldspawn(PODEntity *svEntity);
    virtual ~Worldspawn() = default;

    //! Register worldspawn class as a map entity.
    DefineMapClass( "worldspawn", Worldspawn, SVGBaseEntity );

    //! Interface functions. 
    void Precache() override;    // Precaches data.
    void Spawn() override;       // Spawns the entity.
    void PostSpawn() override;   // PostSpawning is for handling entity references, since they may not exist yet during a spawn period.
    void Think() override;       // General entity thinking routine.

    void SpawnKey(const std::string& key, const std::string& value) override;

    //! Callback functions.
    void WorldspawnThink(void);

    //! Default gravity constant.
    static constexpr int32_t DEFAULT_GRAVITY = 875;

private:
    //! Parsed gravity from key/values. (Named globalGravity to prevent collision with baseentity gravity var.)
    int32_t globalGravity = 0;
};








/**
*   @brief  Local Client entity. Acts like a POD type similar to the server entity.
**/
//struct PODEntity {
//	PODEntity() = default;
//
//	PODEntity(PODEntity&) = default;
//	PODEntity(const PODEntity&) = default;
//	virtual ~PODEntity() = default;
//
//    /**
//    * 
//    *   @brief  Entity Data matching that from the last received server frame.
//    * 
//    **/
//    //! The frame number that this entity was received at.
//    //! Needs to be identical to the current frame number, or else this entity isn't in this frame anymore.
//    int32_t serverFrame = 0;
//
//    //! The last received state of this entity.
//    EntityState current = {};
//    //! The previous last valid state. In worst case scenario might be a copy of current state.
//    EntityState prev = {};
//        
//    //! This entity's server state flags.
//    //int32_t serverFlags = 0; // TODO: Not sure if we need this yet.
//	//! Clipping mask this entity belongs to.
//    int32_t clipMask = 0;
//    //! Min and max bounding box.
//    vec3_t mins = vec3_zero(), maxs = vec3_zero();
//    //! Absolute world transform bounding box. (Note that these are calculated with the link/unlink functions.)
//    vec3_t absMin = vec3_zero(), absMax = vec3_zero(), size = vec3_zero();
//
//
//    /**
//    *
//    *   @brief  Entity Data local to the client only.
//    * 
//    **/
//	//! NOTE: It's never transfered by state, which might be interesting to do however.
//	int32_t linkCount = 0;
//
//	//! An entity's client state flags.
//    int32_t clientFlags = 0;
//
//    //! For diminishing grenade trails
//    int32_t trailCount = 0;
//    //! for trails (variable hz)
//    vec3_t lerpOrigin = vec3_zero();
//
//    //! This is the actual server entity number.
//    int32_t serverEntityNumber = 0;
//    //! This is a unique client entity id, determined by an incremental static counter.
//    int32_t clientEntityNumber = 0;
//
//    //! Pointer to the owning entity (if any.)
//    IClientGameEntity *owner = nullptr;
//
//    //! Pointer to the game entity object that belongs to this client entity.
//    IClientGameEntity *gameEntity;
//
//    //! Key/Value entity dictionary.
//    EntityDictionary entityDictionary;
//
//    //! Actual sv.time when this entity was freed.
//    GameTime freeTime = GameTime::zero();
//
//    // Move this to clientInfo?
//    int32_t lightLevel = 0;
//};
