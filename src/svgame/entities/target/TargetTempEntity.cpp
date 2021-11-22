/*
// LICENSE HERE.

// TargetTempEntity.cpp
*/

#include "../../g_local.h"
#include "../../effects.h"
#include "../../entities.h"
#include "../../utils.h"

#include "../base/ServerGameEntity.h"

#include "TargetTempEntity.h"

//===============
// TargetTempEntity::ctor
//===============
TargetTempEntity::TargetTempEntity( ServerEntity* entity )
	: Base( entity ) {

}

//===============
// TargetTempEntity::Spawn
//===============
void TargetTempEntity::Spawn() {
	SetUseCallback( &TargetTempEntity::TempEntityUse );
}

//===============
// TargetTempEntity::TempEntityUse
//===============
void TargetTempEntity::TempEntityUse( ServerGameEntity* other, ServerGameEntity* activator ) {
	gi.WriteByte( SVG_CMD_TEMP_ENTITY );
	gi.WriteByte( GetStyle() );
	gi.WriteVector3( GetOrigin() );
	gi.Multicast( GetOrigin(), MultiCast::PVS );
}
