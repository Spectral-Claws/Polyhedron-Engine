/***
*
*	License here.
*
*	@file
*
*	ClientGame Tracing Utility. Takes care of handling entities appropriately.
*
***/
#include "../ClientGameLocals.h"
#include "../Exports/Entities.h"



/**
* @brief Constructs a ClientGame Trace Result from the engine's Common trace result.
* @param traceResult A reference to the trace result.
**/
CLGTraceResult::CLGTraceResult(TraceResult& traceResult) :
	allSolid(traceResult.allSolid),
	contents(traceResult.contents),
	endPosition(traceResult.endPosition),
	fraction(traceResult.fraction),
	plane(traceResult.plane),
	startSolid(traceResult.startSolid),
	surface(traceResult.surface)
{
	offsets[0] = traceResult.offsets[0];
	offsets[1] = traceResult.offsets[1];
	offsets[2] = traceResult.offsets[2];
	offsets[3] = traceResult.offsets[3];
	offsets[4] = traceResult.offsets[4];
	offsets[5] = traceResult.offsets[5];
	offsets[6] = traceResult.offsets[6];
	offsets[7] = traceResult.offsets[7];

	// If an entity has been found, look it up in our client entities and assign it.
	GameEntityVector &gameEntities = clge->entities->GetGameEntities();

	if (traceResult.ent) {
		// Cast to PODEntity.
		PODEntity *podEntity = reinterpret_cast<PODEntity*>(traceResult.ent);

		// Acquire number.
		const uint32_t index = podEntity->currentState.number;

		// Look for entity. (Should be done using gameworld get by index...)
		if (index < gameEntities.size() && gameEntities[index] != NULL) {
			gameEntity = gameEntities[index];
		} else {
			// Default to Worldspawn instead.
			gameEntity = gameEntities[0];
		}

		// Assign the podEntity.
		this->podEntity = podEntity;
	} else {
		// Default to Worldspawn instead.
		gameEntity = gameEntities[0];

		// POD Entity still needs to be set to worldspawn.
		podEntity = nullptr;
	}
}

/**
* @brief Constructs a ClientGame Trace Result from the engine's Common trace result.
* @param traceResult A const reference to the trace result.
**/
CLGTraceResult::CLGTraceResult(const TraceResult& traceResult) :
		allSolid(traceResult.allSolid),
	contents(traceResult.contents),
	endPosition(traceResult.endPosition),
	fraction(traceResult.fraction),
	plane(traceResult.plane),
	startSolid(traceResult.startSolid),
	surface(traceResult.surface)
{
	offsets[0] = traceResult.offsets[0];
	offsets[1] = traceResult.offsets[1];
	offsets[2] = traceResult.offsets[2];
	offsets[3] = traceResult.offsets[3];
	offsets[4] = traceResult.offsets[4];
	offsets[5] = traceResult.offsets[5];
	offsets[6] = traceResult.offsets[6];
	offsets[7] = traceResult.offsets[7];

	// If an entity has been found, look it up in our client entities and assign it.
	GameEntityVector &gameEntities = clge->entities->GetGameEntities();

	if (traceResult.ent) {
		// Cast to PODEntity.
		PODEntity *podEntity = reinterpret_cast<PODEntity*>(traceResult.ent);

		// Acquire number.
		const uint32_t index = podEntity->currentState.number;

		// Look for entity. (Should be done using gameworld get by index...)
		if (index < gameEntities.size() && gameEntities[index] != NULL) {
			gameEntity = gameEntities[index];
		} else {
			// Default to Worldspawn instead.
			gameEntity = gameEntities[0];
		}

		// Assign the podEntity.
		this->podEntity = podEntity;
	} else {
		// Default to Worldspawn instead.
		gameEntity = gameEntities[0];

		// POD Entity still needs to be set to worldspawn.
		podEntity = nullptr;
	}
}

/**
*	@brief	ClientGame Trace function. Supports Game Entities.
**/
CLGTraceResult CLG_Trace(const vec3_t& start, const vec3_t& mins, const vec3_t& maxs, const vec3_t& end, GameEntity* skipGameEntity, const int32_t& contentMask) {
    // Fetch POD Entity to use for pass entity testing.
    PODEntity* clientPODEntity = (skipGameEntity ? skipGameEntity->GetPODEntity() : NULL);

	// Execute and return the actual trace.
    return clgi.Trace(start, mins, maxs, end, (struct PODEntity*)clientPODEntity, contentMask);
}