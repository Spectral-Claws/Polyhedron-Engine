//
// This file is rather annoying/complicated, we'll have to seek for some way to replace it.
// 
// For now though, it works so... keep it simple stupid, don't change it if it ain't broken :)
//
#include "g_local.h"
#include "effects.h"
#include "entities.h"
#include "utils.h"

// 
// ---- generated by grep & sed, do not edit = WRONG!!! Haha
// This wasn't generated anymore at all in our code, ah well :P
//
#include "functionpointers.h"

//extern void door_blocked(ServerEntity* self, ServerEntity* other);
//extern void plat_blocked(ServerEntity* self, ServerEntity* other);
//extern void rotating_blocked(ServerEntity* self, ServerEntity* other);
//extern void train_blocked(ServerEntity* self, ServerEntity* other);
//extern void body_die(ServerEntity* self, ServerEntity* inflictor, ServerEntity* attacker, int damage, const vec3_t& point);
//extern void button_killed(ServerEntity* self, ServerEntity* inflictor, ServerEntity* attacker, int damage, const vec3_t& point);
//extern void debris_die(ServerEntity* self, ServerEntity* inflictor, ServerEntity* attacker, int damage, const vec3_t& point);
//extern void door_killed(ServerEntity* self, ServerEntity* inflictor, ServerEntity* attacker, int damage, const vec3_t& point);
//extern void func_explosive_explode(ServerEntity* self, ServerEntity* inflictor, ServerEntity* attacker, int damage, const vec3_t& point);
//extern void gib_die(ServerEntity* self, ServerEntity* inflictor, ServerEntity* attacker, int damage, const vec3_t& point);
//
//extern void Brush_AngleMove_Begin(ServerEntity* self);
//extern void Brush_AngleMove_Done(ServerEntity* self);
//extern void Brush_AngleMove_Final(ServerEntity* self);
//
//extern void button_return(ServerEntity* self);
//
//extern void door_go_down(ServerEntity* self);
//
//extern void DoRespawn(ServerEntity* self);
//extern void drop_make_touchable(ServerEntity* self);
//extern void droptofloor(ServerEntity* self);
//
//extern void func_object_release(ServerEntity* self);
//extern void func_timer_think(ServerEntity* self);
//extern void func_train_find(ServerEntity* self);
//
//extern void gib_think(ServerEntity* self);
//
//extern void MegaHealth_think(ServerEntity* self);
//
////extern void Brush_Move_Begin(ServerEntity* self);
////extern void Brush_Move_Done(ServerEntity* self);
////extern void Brush_Move_Final(ServerEntity* self);
//extern void multi_wait(ServerEntity* self);
//extern void plat_go_down(ServerEntity* self);
////extern void SP_CreateCoopSpots(ServerEntity* self);
//extern void SP_FixCoopSpots(ServerEntity* self);
//extern void target_crosslevel_target_think(ServerEntity* self);
//extern void target_earthquake_think(ServerEntity* self);
//extern void target_explosion_explode(ServerEntity* self);
//extern void target_lightramp_think(ServerEntity* self);
//extern void Think_AccelMove(ServerEntity* self);
//
//extern void Think_CalcMoveSpeed(ServerEntity* self);
//extern void Think_Delay(ServerEntity* self);
//extern void Think_SpawnDoorTrigger(ServerEntity* self);
//extern void train_next(ServerEntity* self);
//extern void trigger_elevator_init(ServerEntity* self);
////extern void blaster_touch(ServerEntity* self, ServerEntity* other, cplane_t* plane, csurface_t* surf);
//extern void button_touch(ServerEntity* self, ServerEntity* other, cplane_t* plane, csurface_t* surf);
//extern void door_touch(ServerEntity* self, ServerEntity* other, cplane_t* plane, csurface_t* surf);
//extern void drop_temp_touch(ServerEntity* self, ServerEntity* other, cplane_t* plane, csurface_t* surf);
//extern void func_object_touch(ServerEntity* self, ServerEntity* other, cplane_t* plane, csurface_t* surf);
//extern void gib_touch(ServerEntity* self, ServerEntity* other, cplane_t* plane, csurface_t* surf);
//extern void hurt_touch(ServerEntity* self, ServerEntity* other, cplane_t* plane, csurface_t* surf);
//
//extern void rotating_touch(ServerEntity* self, ServerEntity* other, cplane_t* plane, csurface_t* surf);
//
//extern void teleporter_touch(ServerEntity* self, ServerEntity* other, cplane_t* plane, csurface_t* surf);
//extern void Touch_DoorTrigger(ServerEntity* self, ServerEntity* other, cplane_t* plane, csurface_t* surf);
//extern void Touch_Multi(ServerEntity* self, ServerEntity* other, cplane_t* plane, csurface_t* surf);
//extern void Touch_Plat_Center(ServerEntity* self, ServerEntity* other, cplane_t* plane, csurface_t* surf);
//extern void trigger_gravity_touch(ServerEntity* self, ServerEntity* other, cplane_t* plane, csurface_t* surf);
//extern void trigger_monsterjump_touch(ServerEntity* self, ServerEntity* other, cplane_t* plane, csurface_t* surf);
//extern void trigger_push_touch(ServerEntity* self, ServerEntity* other, cplane_t* plane, csurface_t* surf);
//
//extern void button_use(ServerEntity* self, ServerEntity* other, ServerEntity* activator);
//extern void door_use(ServerEntity* self, ServerEntity* other, ServerEntity* activator);
//extern void func_conveyor_use(ServerEntity* self, ServerEntity* other, ServerEntity* activator);
//extern void func_explosive_spawn(ServerEntity* self, ServerEntity* other, ServerEntity* activator);
//extern void func_explosive_use(ServerEntity* self, ServerEntity* other, ServerEntity* activator);
//extern void func_object_use(ServerEntity* self, ServerEntity* other, ServerEntity* activator);
//extern void func_timer_use(ServerEntity* self, ServerEntity* other, ServerEntity* activator);
//extern void func_wall_use(ServerEntity* self, ServerEntity* other, ServerEntity* activator);
//extern void hurt_use(ServerEntity* self, ServerEntity* other, ServerEntity* activator);
////extern void light_use(ServerEntity* self, ServerEntity* other, ServerEntity* activator);
//extern void rotating_use(ServerEntity* self, ServerEntity* other, ServerEntity* activator);
//extern void target_earthquake_use(ServerEntity* self, ServerEntity* other, ServerEntity* activator);
//extern void target_lightramp_use(ServerEntity* self, ServerEntity* other, ServerEntity* activator);
//extern void train_use(ServerEntity* self, ServerEntity* other, ServerEntity* activator);
//extern void trigger_counter_use(ServerEntity* self, ServerEntity* other, ServerEntity* activator);
//extern void trigger_crosslevel_trigger_use(ServerEntity* self, ServerEntity* other, ServerEntity* activator);
//extern void trigger_elevator_use(ServerEntity* self, ServerEntity* other, ServerEntity* activator);
//extern void trigger_enable(ServerEntity* self, ServerEntity* other, ServerEntity* activator);
//extern void trigger_key_use(ServerEntity* self, ServerEntity* other, ServerEntity* activator);
//extern void trigger_relay_use(ServerEntity* self, ServerEntity* other, ServerEntity* activator);
//extern void Use_Areaportal(ServerEntity* self, ServerEntity* other, ServerEntity* activator);
//
//extern void Use_Item(ServerEntity* ent, ServerEntity* other, ServerEntity* activator);
//extern void use_killbox(ServerEntity* ent, ServerEntity* other, ServerEntity* activator);
//extern void Use_Multi(ServerEntity* ent, ServerEntity* other, ServerEntity* activator);
//extern void Use_Plat(ServerEntity* ent, ServerEntity* other, ServerEntity* activator);
//extern void use_target_blaster(ServerEntity* ent, ServerEntity* other, ServerEntity* activator);
//extern void use_target_changelevel(ServerEntity* ent, ServerEntity* other, ServerEntity* activator);
//extern void use_target_explosion(ServerEntity* ent, ServerEntity* other, ServerEntity* activator);
//extern void use_target_spawner(ServerEntity* ent, ServerEntity* other, ServerEntity* activator);
//extern void Use_Target_Speaker(ServerEntity* ent, ServerEntity* other, ServerEntity* activator);
//extern void use_target_splash(ServerEntity* ent, ServerEntity* other, ServerEntity* activator);
//extern void Use_Target_Tent(ServerEntity* ent, ServerEntity* other, ServerEntity* activator);
//extern void plat_hit_bottom(ServerEntity* self);
//extern void plat_hit_top(ServerEntity* self);
//extern void button_done(ServerEntity* self);
//extern void button_wait(ServerEntity* self);
//extern void door_hit_bottom(ServerEntity* self);
//extern void door_hit_top(ServerEntity* self);
//extern void train_wait(ServerEntity* self);

//const save_ptr_t save_ptrs[] = {
//{ P_blocked, door_blocked },
//{ P_blocked, plat_blocked },
//{ P_blocked, rotating_blocked },
//{ P_blocked, train_blocked },
//
//{ P_die, body_die },
//{ P_die, button_killed },
//{ P_die, debris_die },
//{ P_die, door_killed },
//
//{ P_die, func_explosive_explode },
//
//{ P_die, gib_die },
//{ P_die, SVG_Client_Die },
//
//{ P_pain, SVG_Client_Pain },
//
//{ P_think, Brush_AngleMove_Begin },
//{ P_think, Brush_AngleMove_Done },
//{ P_think, Brush_AngleMove_Final },
//
//{ P_think, button_return },
//
//{ P_think, door_go_down },
//
//{ P_think, DoRespawn },
//{ P_think, drop_make_touchable },
//{ P_think, droptofloor },
//{ P_think, func_object_release },
//{ P_think, func_timer_think },
//{ P_think, func_train_find },
//{ P_think, SVG_FreeServerEntity },
//{ P_think, gib_think },
//
//{ P_think, MegaHealth_think },
//{ P_think, Brush_Move_Begin },
//{ P_think, Brush_Move_Done },
//{ P_think, Brush_Move_Final },
//{ P_think, multi_wait },
//{ P_think, plat_go_down },
////{ P_think, SP_CreateCoopSpots },
//{ P_think, SP_FixCoopSpots },
//{ P_think, target_crosslevel_target_think },
//{ P_think, target_earthquake_think },
//{ P_think, target_explosion_explode },
//{ P_think, target_lightramp_think },
//{ P_think, Think_AccelMove },
//
//{ P_think, Think_CalcMoveSpeed },
//{ P_think, Think_Delay },
//{ P_think, Think_SpawnDoorTrigger },
//{ P_think, train_next },
//{ P_think, trigger_elevator_init },
//{ P_touch, blaster_touch },
//{ P_touch, button_touch },
//{ P_touch, door_touch },
//{ P_touch, drop_temp_touch },
//{ P_touch, func_object_touch },
//{ P_touch, gib_touch },
//{ P_touch, hurt_touch },
//
//{ P_touch, rotating_touch },
//
//
//{ P_touch, teleporter_touch },
//
//{ P_touch, Touch_DoorTrigger },
//{ P_touch, SVG_TouchItem },
//{ P_touch, Touch_Multi },
//{ P_touch, Touch_Plat_Center },
//
//{ P_touch, trigger_gravity_touch },
//{ P_touch, trigger_monsterjump_touch },
//{ P_touch, trigger_push_touch },
//
//{ P_use, button_use },
//
//{ P_use, door_use },
//
//{ P_use, func_conveyor_use },
//{ P_use, func_explosive_spawn },
//{ P_use, func_explosive_use },
//{ P_use, func_object_use },
//{ P_use, func_timer_use },
//{ P_use, func_wall_use },
//
//{ P_use, hurt_use },
////{ P_use, light_use },
//
//{ P_use, rotating_use },
//
//{ P_use, target_earthquake_use },
//{ P_use, target_lightramp_use },
//{ P_use, train_use },
//
//{ P_use, trigger_counter_use },
//{ P_use, trigger_crosslevel_trigger_use },
//{ P_use, trigger_elevator_use },
//{ P_use, trigger_enable },
//{ P_use, trigger_key_use },
//{ P_use, trigger_relay_use },
//
//{ P_use, Use_Areaportal },
//{ P_use, Use_Item },
//{ P_use, use_killbox },
//{ P_use, Use_Multi },
//{ P_use, Use_Plat },
//{ P_use, use_target_blaster },
//{ P_use, use_target_changelevel },
//{ P_use, use_target_explosion },
//{ P_use, use_target_spawner },
//{ P_use, Use_Target_Speaker },
//{ P_use, use_target_splash },
//{ P_use, Use_Target_Tent },
//
//{ P_moveinfo_endfunc, plat_hit_bottom },
//{ P_moveinfo_endfunc, plat_hit_top },
//
//{ P_moveinfo_endfunc, button_done },
//{ P_moveinfo_endfunc, button_wait },
//
//{ P_moveinfo_endfunc, door_hit_bottom },
//{ P_moveinfo_endfunc, door_hit_top },
//
//{ P_moveinfo_endfunc, train_wait },
//};
//const int num_save_ptrs = sizeof(save_ptrs) / sizeof(save_ptrs[0]);
