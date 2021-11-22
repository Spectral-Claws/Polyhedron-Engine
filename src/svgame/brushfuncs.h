// LICENSE HERE.

//
// brushfuncs.h
//
//
// Contains basic weapons functionalities.
//
#ifndef __SVGAME_PLAYER_WEAPONS_H__
#define __SVGAME_PLAYER_WEAPONS_H__

// LICENSE HERE.

//
// svgame/brushfuncs.c
//
//
// Brush utility functionality implementation.
//

// Include local game header.
#include "g_local.h"

//=====================================================

//
//=========================================================
//
//  PLATS
//
//  movement options:
//
//  linear
//  smooth start, hard stop
//  smooth start, smooth stop
//
//  start
//  end
//  acceleration
//  speed
//  deceleration
//  begin sound
//  end sound
//  target fired when reaching end
//  wait at end
//
//  object characteristics that use move segments
//  ---------------------------------------------
//  movetype_push, or movetype_stop
//  action when touched
//  action when Blocked
//  action when used
//    disabled?
//  auto trigger spawning
//
//
//=========================================================
//

#define PLAT_LOW_TRIGGER    1

#define STATE_TOP           0
#define STATE_BOTTOM        1
#define STATE_UP            2
#define STATE_DOWN          3



// Admer: The functions below are obsolete
// They are in the process of moving to a mover base class

//
// Support routines for movement (changes in origin using velocity)
//
//void Brush_Move_Done(ServerEntity* ent);
//void Brush_Move_Final(ServerEntity* ent);
//void Brush_Move_Begin(ServerEntity* ent);
//void Brush_Move_Calc(ServerEntity* ent, const vec3_t &dest, void(*func)(ServerEntity*));

//
// Support routines for angular movement (changes in angle using angularVelocity)
//
//void Brush_AngleMove_Done(ServerEntity* ent);
//void Brush_AngleMove_Final(ServerEntity* ent);
//void Brush_AngleMove_Begin(ServerEntity* ent);
//void Brush_AngleMove_Calc(ServerEntity* ent, void(*func)(ServerEntity*));

//void plat_CalcAcceleratedMove(PushMoveInfo* moveinfo);
//void plat_Accelerate(PushMoveInfo* moveinfo);
//void Think_AccelMove(ServerEntity* ent);

#endif // __SVGAME_PLAYER_WEAPONS_H__