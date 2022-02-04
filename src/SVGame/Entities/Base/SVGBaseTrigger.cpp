/*
// LICENSE HERE.

//
// SVGBaseTrigger.cpp
//
//
*/
#include "../../ServerGameLocal.h"	// SVGame.
#include "../../Effects.h"			// Effects.
#include "../../Entities.h"			// Entities.
#include "../../Utilities.h"		// Util funcs.

// Class Entities.
#include "SVGBaseTrigger.h"

// Included for delayed use.
#include "../Trigger/TriggerDelayedUse.h"

// Constructor/Deconstructor.
SVGBaseTrigger::SVGBaseTrigger(Entity* svEntity) : Base(svEntity) {
	//
	// All callback functions best be nullptr.
	//
	//thinkFunction = nullptr;


	//
	// Set all entity pointer references to nullptr.
	//
	//activatorEntity = nullptr;
	//activatorEntity = nullptr;
	//enemyEntity = nullptr;
	//groundEntity = nullptr;
	//oldEnemyEntity = nullptr;
	//teamChainEntity = nullptr;
	//teamMasterEntity = nullptr;

	//
	// Default values for members.
	//


	//moveType = MoveType::None;

	//// Velocity.
	//velocity = vec3_zero();
	//angularVelocity = vec3_zero();
	//mass = 0;
	//groundEntityLinkCount = 0;
	//health = 0;
	//maxHealth = 0;
	//deadFlag = DEAD_NO;
}

// Interface functions. 
//
//===============
// SVGBaseTrigger::Precache
//
//===============
//
void SVGBaseTrigger::Precache() {
	Base::Precache();
}

//
//===============
// SVGBaseTrigger::Spawn
//
//===============
//
void SVGBaseTrigger::Spawn() {
	Base::Spawn();
}

//
//===============
// SVGBaseTrigger::Respawn
// 
//===============
//
void SVGBaseTrigger::Respawn() {
	Base::Respawn();
}

//
//===============
// SVGBaseTrigger::PostSpawn
// 
//===============
//
void SVGBaseTrigger::PostSpawn() {
	Base::PostSpawn();
}

//
//===============
// SVGBaseTrigger::Think
//
//===============
//
void SVGBaseTrigger::Think() {
	Base::Think();
}

//
//===============
// SVGBaseTrigger::InitBrushTrigger
//
//===============
//
void SVGBaseTrigger::InitBrushTrigger() {
	SetModel(GetModel());
	SetMoveType(MoveType::None);
	SetSolid(Solid::Trigger);
	
	SetServerFlags(EntityServerFlags::NoClient);
}

//
//===============
// SVGBaseTrigger::InitPointTrigger
//
//===============
//
void SVGBaseTrigger::InitPointTrigger() {
	const vec3_t HULL_MINS = { -16.f, -16.f, -36.f };
	const vec3_t HULL_MAXS = { 16.f,  16.f,  36.f };

	SetSize(HULL_MINS + HULL_MAXS);
	SetMoveType(MoveType::None);
	SetSolid(Solid::Trigger);

	// Ensure we got the proper no client flags.
	SetServerFlags(EntityServerFlags::NoClient);
}

//
//===============
// SVGBaseTrigger::SpawnKey
//
//===============
//
void SVGBaseTrigger::SpawnKey(const std::string& key, const std::string& value) {
	// Wait.
	if (key == "wait") {
		// Parsed float.
		float parsedFloat = 0.f;

		// Parse.
		ParseFloatKeyValue(key, value, parsedFloat);

		// Assign.
		SetWaitTime(parsedFloat);
	}
	// Parent class spawnkey.
	else {
		Base::SpawnKey(key, value);
	}
}

//
//===============
// SVGBaseTrigger::UseTargets
//
// The activator is the entity who is initiating the firing. If not set as
// a function argument, it will use whichever is set in the entity itself.
//
// If a delay is set, a DelayedUse entity will be created that will actually
// call the UseTargets after a set amount of time(delay) has passed.
//
// Centerprints any self.message to the activator.
//
// Search for (string)targetName in all entities, if it matches, calls their
// Use function.
//===============
//
//void SVGBaseTrigger::UseTargets(SVGBaseEntity* activator) {
//	//
//	// Check for a delay
//	//
//    if (GetDelayTime()) {
//		// Create a temporary DelayedTrigger entity, to fire at a latter time.
//	    SVGBaseTrigger *triggerDelay = SVG_CreateClassEntity<TriggerDelayedUse>();
//		if (!activator)
//			gi.DPrintf("TriggerDelayThink with no activator\n");
//		triggerDelay->SetActivator(activator);
//		triggerDelay->SetMessage(GetMessage());
//		triggerDelay->SetTarget(GetTarget());
//		triggerDelay->SetKillTarget(GetKillTarget());
//		triggerDelay->SetNextThinkTime(level.time + GetDelayTime());
//		triggerDelay->SetThinkCallback(&TriggerDelayedUse::TriggerDelayedUseThink);
//
//		// Return, the rest happens by delay.
//		return;
//	}
//	
//	//
//	// Print the "message"
//	//
//	if (GetMessage().length() && !(activator->GetServerFlags() & EntityServerFlags::Monster)) {
//		// Fetch noise index.
//		int32_t noiseIndex = GetNoiseIndex();
//
//		// Print the message.
//		SVG_CenterPrint(activator, GetMessage());
//
//		// Play specific noise sound, in case one is set. Default talk1.wav otherwise.
//		if (noiseIndex) {
//			SVG_Sound(activator, CHAN_AUTO, noiseIndex, 1, ATTN_NORM, 0);
//		} else {
//			SVG_Sound(activator, CHAN_AUTO, gi.SoundIndex("misc/talk1.wav"), 1, ATTN_NORM, 0);
//		}
//	}
//
//	//
//	// Kill killtargets
//	//
//	if (GetKillTarget().length()) {
//		//while (triggerEntity = SVG_FindEntityByKeyValue("targetname", GetKillTarget(), triggerEntity))
//		// Loop over the total entity range, ensure that we're checking for the right filters.
//		qboolean foundKillTarget = false;
//
//		for (auto* triggerEntity : GetBaseEntityRange<0, MAX_EDICTS>()
//			| bef::IsValidPointer
//			| bef::HasServerEntity
//			| bef::InUse
//			| bef::HasKeyValue("targetname", GetKillTarget())) {
//
//			// We found at least one killtarget matching this targetname.
//			foundKillTarget = true;
//
//			// It is going to die, remove it for next frame.
//			triggerEntity->Remove();
//		}
//
//		// Warn that the killtarget entity hasn't been triggered.
//		if (!foundKillTarget) {
//			gi.DPrintf("WARNING: killtarget entity '%s' couldn't be found.\n");
//			return;
//		}
//	}
//	
//	//
//	// Fire targets
//	//
//	if (GetTarget().length()) {
//		// Loop over the total entity range, ensure that we're checking for the right filters.
//		for (auto* triggerEntity : GetBaseEntityRange<0, MAX_EDICTS>()
//			| bef::IsValidPointer
//			| bef::HasServerEntity
//			| bef::InUse
//			| bef::HasKeyValue("targetname", GetTarget())) {
//
//			// Make sure it is in use, if not, debug.
//			if (!triggerEntity->IsInUse()) {
//				gi.DPrintf("WARNING: Target entity: {target(%s) ID(#%i)} is not in use.\n", GetTarget(), GetServerEntity()->state.number);
//				continue;
//			}
//
//			// Doors fire area portals in a special way. So we skip those.
//			if (triggerEntity->GetClassname() == "func_areaportal"
//				&& (GetClassname() == "func_door" || GetClassname() == "func_door_rotating")) {
//				continue;
//			}
//
//			// Do NOT ALLOW an entity to USE ITSELF. :)
//			if (triggerEntity == this) {
//				gi.DPrintf("WARNING: Entity #%i tries to use itself.\n", GetServerEntity()->state.number);
//			} else {
//				triggerEntity->Use(this, activator);
//			}
//		}
//	}
//}