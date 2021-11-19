/*
// LICENSE HERE.

//
// BlasterBolt.h
//
//
*/
#ifndef __SVGAME_ENTITIES_WEAPONRY_BLASTERBOLT_H__
#define __SVGAME_ENTITIES_WEAPONRY_BLASTERBOLT_H__

class SVGBaseEntity;

class BlasterBolt : public SVGBaseEntity {
public:
    // Constructor/Deconstructor.
    BlasterBolt(ServerEntity* svServerEntity);
    virtual ~BlasterBolt();

    DefineClass( BlasterBolt, SVGBaseEntity );

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
    // Callback functions.
    //
    //
    void BlasterBoltTouch(SVGBaseEntity* self, SVGBaseEntity* other, cplane_t* plane, csurface_t* surf);

    //
    // Get/Set
    //
    

protected:

private:

};

#endif // __SVGAME_ENTITIES_WEAPONRY_BLASTERBOLT_H__