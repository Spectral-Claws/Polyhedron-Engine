// LICENSE HERE.

//
// svgame/entities/misc_gib_arm.c
//
//
// misc_gib_arm entity implementation.
//

// Include local game header.
#include "../../g_local.h"

// Declared in misc.c
extern void gib_die(Entity* self, Entity* inflictor, Entity* attacker, int damage, const vec3_t& point);

//=====================================================
/*QUAKED misc_gib_arm (1 0 0) (-8 -8 -8) (8 8 8)
Intended for use with the target_spawner
*/
void SP_misc_gib_arm(Entity* ent)
{
    gi.SetModel(ent, "models/objects/gibs/arm/tris.md2");
    ent->solid = Solid::Not;
    ent->state.effects |= EntityEffectType::Gib;
    ent->takeDamage = TakeDamage::Yes;
    ent->Die = gib_die;
    ent->moveType = MoveType::Toss;
    ent->serverFlags |= EntityServerFlags::Monster;
    ent->deadFlag = DEAD_DEAD;
    ent->angularVelocity[0] = random() * 200;
    ent->angularVelocity[1] = random() * 200;
    ent->angularVelocity[2] = random() * 200;
    ent->Think = SVG_FreeEntity;
    ent->nextThinkTime = level.time + 30;
    gi.LinkEntity(ent);
}