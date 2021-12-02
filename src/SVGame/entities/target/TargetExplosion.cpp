/*
// LICENSE HERE.

// TargetExplosion.cpp
*/

#include "../../ServerGameLocal.h"
#include "../../Effects.h"
#include "../../Entities.h"
#include "../../Utilities.h"

#include "../base/SynchedEntityBase.h"

#include "TargetExplosion.h"

//===============
// TargetExplosion::ctor
//===============
TargetExplosion::TargetExplosion( ServerEntity* entity )
	: Base( entity ) {

}

//===============
// TargetExplosion::Spawn
//===============
void TargetExplosion::Spawn() {
	SetUseCallback( &TargetExplosion::ExplosionUse );
	SetServerFlags( EntityServerFlags::NoClient );
}

//===============
// TargetExplosion::ExplosionUse
//===============
void TargetExplosion::ExplosionUse( SynchedEntityBase * other, SynchedEntityBase * activator ) {
	this->activator = activator;

	if ( !GetDelayTime() ) {
		ExplosionThink();
		return;
	}

	SetThinkCallback( &TargetExplosion::ExplosionThink );
	SetNextThinkTime( level.time + GetDelayTime() );
}

//===============
// TargetExplosion::ExplosionThink
//===============
void TargetExplosion::ExplosionThink() {
	gi.WriteByte( SVG_CMD_TEMP_ENTITY );
	gi.WriteByte( TempEntityEvent::Explosion1 );
	gi.WriteVector3( GetOrigin() );
	gi.Multicast( GetOrigin(), MultiCast::PHS );

	SVG_InflictRadiusDamage( this, activator, GetDamage(), nullptr, GetDamage() + 40.0f, MeansOfDeath::Explosive );

	float save;
	save = GetDelayTime();
	SetDelayTime( 0.0f );
	UseTargets();
	SetDelayTime( save );
}
