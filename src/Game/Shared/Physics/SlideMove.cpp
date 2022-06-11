/***
*
*	License here.
*
*	@file
*
*	BoxSlide Movement implementation for SharedGame Physics.
*
*	NOTE: The GroundEntity info has to be up to date before pulling off a SlideMove.
* 
***/
#pragma once

// Shared Game.
#include "../SharedGame.h"

#ifdef SHAREDGAME_SERVERGAME 
	#include "../../Server/ServerGameLocals.h"
	#include "../../Server/World/ServerGameWorld.h"
#endif
#ifdef SHAREDGAME_CLIENTGAME
	#include "../../Client/ClientGameLocals.h"
	#include "../../Client/World/ClientGameWorld.h"
#endif

// Physics.
#include "Physics.h"
#include "SlideMove.h"



/***
*
*	
*
*	Touch Entities.
*
*	
* 
***/
/**
*	@brief	If within limits: Adds the geToucher GameEntity to the SlideMoveState's touching entities list.
*	@return	The touch entity number. -1 if it's invalid, or has already been added.
**/
static int32_t SM_AddTouchEntity( SlideMoveState *moveState, GameEntity *geToucher ) {
	// Get the touch entity number for storing in our touch entity list.
	int32_t touchEntityNumber = (geToucher ? geToucher->GetNumber() : -1);

	if( !moveState || !geToucher || moveState->numTouchEntities >= 32 || touchEntityNumber < 0) {
		// Warn print:
		if (!geToucher) {
			SG_Physics_PrintWarning( std::string(__func__) + "Trying to add a(nullptr) GameEntity" );
		} else if (geToucher->GetNumber() < 0) {
			SG_Physics_PrintWarning( std::string(__func__) + "Trying to add an invalid GameEntity(#" + std::to_string(geToucher->GetNumber()) + "% i) number" );
		} else if (!moveState) {
			SG_Physics_PrintWarning( std::string(__func__) + "moveState == (nullptr) while trying to add GameEntity(#" + std::to_string(geToucher->GetNumber()) + "% i)" );
		} else if (moveState->numTouchEntities >= 32) {
			SG_Physics_PrintWarning( std::string(__func__) + "moveState->numTouchEntities >= 32 while trying to add GameEntity(#" + std::to_string(geToucher->GetNumber()) + "% i)" );
		}
		
		return -1;
	}

	// See if it is already added.
	for( int32_t i = 0; i < moveState->numTouchEntities; i++ ) {
		if( moveState->touchEntites[i] == touchEntityNumber ) {
			return -1;
		}
	}

	// Otherwise, add the entity to our touched list.
	moveState->touchEntites[moveState->numTouchEntities] = touchEntityNumber;
	moveState->numTouchEntities++;

	// Last but not least, return its number.
	return touchEntityNumber;
}

/**
*	@brief	Clears the SlideMoveState's clipping plane list. 
*			(Does not truly vec3_zero them, just sets numClipPlanes to 0. Adding a 
*			new clipping plane will overwrite the old values.)
**/
static void SM_ClearTouchEntities( SlideMoveState *moveState ) {
	if (!moveState) {
		return;
	}
	
	moveState->numTouchEntities = 0;
}


/***
*
*	
*
*	Clipping Planes.
*
*	
* 
***/
static const int32_t SM_SlideClipMove( SlideMoveState *moveState, const bool stepping );

/**
*	@brief	If the list hasn't exceeded MAX_SLIDEMOVE_CLIP_PLANES: Adds the plane normal to the SlideMoveState's clipping plane normals list.
**/
static void SM_AddClippingPlane( SlideMoveState *moveState, const vec3_t &planeNormal ) {
	// Ensure we stay within limits of MAX_SLIDEMOVE_CLIP_PLANES . Warn if we don't.
	if( moveState->numClipPlanes + 1 == MAX_SLIDEMOVE_CLIP_PLANES ) {
		SG_Physics_PrintWarning( std::string(__func__) + "MAX_SLIDEMOVE_CLIP_PLANES reached" );
		return;
	}

	// See if we are already clipping to this plane.
	for( int32_t i = 0; i < moveState->numClipPlanes; i++ ) {
		if( vec3_dot( planeNormal, moveState->clipPlaneNormals[i] ) > (FLT_EPSILON - 1.0f) ) {
			return;
		}
	}

	// Add the plane.
	moveState->clipPlaneNormals[moveState->numClipPlanes] = planeNormal;
	moveState->numClipPlanes++;
}

/**
*	@return	Clipped by normal velocity.
**/
inline vec3_t SM_ClipVelocity( const vec3_t &inVelocity, const vec3_t &normal, const float overbounce ) {
	float backoff = vec3_dot( inVelocity, normal );

	if( backoff <= 0 ) {
		backoff *= overbounce;
	} else {
		backoff /= overbounce;
	}

	// Calculate out velocity vector.
	vec3_t outVelocity = ( inVelocity - vec3_scale( normal, backoff ) );

	// SlideMove clamp it.
#if defined(SM_SLIDEMOVE_CLAMPING) && SM_SLIDEMOVE_CLAMPING == 1
	{
		float oldSpeed = vec3_length(inVelocity);
		float newSpeed = vec3_length(outVelocity);

		if (newSpeed > oldSpeed) {
			outVelocity = vec3_scale(vec3_normalize(outVelocity), oldSpeed);
		}
	}
#endif
	return outVelocity;
}

/**
*	@brief	Clips the moveState's velocity to all the normals stored in its current clipping plane normal list.
**/
static void SM_ClipVelocityToClippingPlanes( SlideMoveState *moveState ) {
	for( int32_t i = 0; i < moveState->numClipPlanes; i++ ) {
		// Get const ref to our clip plane normal.
		const vec3_t &clipPlaneNormal = moveState->clipPlaneNormals[i];

		// Skip if its looking in the same direction than the velocity,
		if( vec3_dot( moveState->velocity, clipPlaneNormal ) > (FLT_EPSILON - 1.0f) ) {
			continue;
		}

		// Clip velocity to the clipping plane normal.
		moveState->velocity = SM_ClipVelocity( moveState->velocity, clipPlaneNormal, moveState->slideBounce );
	}
}

/**
*	@brief	Clears the SlideMoveState's clipping plane list. 
*			(Does not truly vec3_zero them, just sets numClipPlanes to 0. Adding a 
*			new clipping plane will overwrite the old values.)
**/
static void SM_ClearClippingPlanes( SlideMoveState *moveState ) {
	if (!moveState) {
		return;
	}
	
	moveState->numClipPlanes = 0;
}



/***
*
*	
*
*	Sliding Internal API.
*
*	
* 
***/
/**
*	@brief	Traces whether there has been an edge, instead of a stair, or
*			no difference at all, between two set positions.
*
*	@return	Mask containing the SlideMoveFlags.
**/
static const int32_t SM_CheckForEdge(const vec3_t& start, const vec3_t& end, const vec3_t &mins, const vec3_t &maxs, GameEntity *geSkip, float stepHeight, int32_t contentMask, int32_t groundEntityNumber) {
	// Mask that gets set the resulting move flags.
	int32_t blockedMask = 0;

	//// Calculate the end point for the start ground trace.
	//const vec3_t  startTraceEnd  = vec3_fmaf( start, SLIDEMOVE_STEP_HEIGHT + SLIDEMOVE_GROUND_DISTANCE, vec3_down() );
	//SGTraceResult startTrace = SG_Trace( start, mins, maxs, startTraceEnd, geSkip, contentMask );

	//const vec3_t  endTraceEnd  = vec3_fmaf( end, SLIDEMOVE_STEP_HEIGHT + SLIDEMOVE_GROUND_DISTANCE, vec3_down() );
	//SGTraceResult endTrace = SG_Trace( end, mins, maxs, endTraceEnd, geSkip, contentMask );

	//// If we had ground entity.
	//if ( groundEntityNumber != -1 ) {
	//	// Check if it is a different one from the current ground.
	//	GameEntity *geStartGroundEntity = SGGameWorld::ValidateEntity( startTrace.gameEntity );
	//	GameEntity *geEndGroundEntity = SGGameWorld::ValidateEntity( endTrace.gameEntity );

	//	// See if the ground that we had, still matches with our start point.
	//	if ( !( geStartGroundEntity && groundEntityNumber != geStartGroundEntity->GetNumber() ) ) {
	//		// blockedMask |= SlideMoveFlags::StartedOnNewGround
	//	}

	//	if ( !( geEndGroundEntity && groundEntityNumber != geEndGroundEntity->GetNumber() ) ) {
	//		if (startTrace.plane.dist != endTrace.plane.dist) {
	//			// If the fraction is less than 1, it means we could step on it.
	//			if ( IsWalkablePlane( endTrace.plane ) && endTrace.fraction < 1.0f ) {
	//				blockedMask |= SlideMoveFlags::CanStepDown;
	//			}
	//			if ( !IsWalkablePlane( endTrace.plane) ) {
	//				blockedMask |= SlideMoveFlags::EdgeMoved;
	//			}
	//		}
	//	}
	//}

	return blockedMask;
}

/**
*	@brief	Performs a ground/step-move trace to determine whether we can step, or fall off an edge.
**/
static SGTraceResult SM_Trace( SlideMoveState* moveState, const vec3_t *origin = nullptr, const vec3_t *mins = nullptr, const vec3_t *maxs = nullptr, const vec3_t *end = nullptr, const int32_t skipEntityNumber = -1, const int32_t contentMask = -1  ) {
	/**
	*	#0: Determine whether to use move state or custom values.
	**/
	const vec3_t traceOrigin	= ( origin != nullptr ? *origin : moveState->origin );
	const vec3_t traceMins		= ( mins != nullptr ? *mins: moveState->mins );
	const vec3_t traceMaxs		= ( maxs != nullptr ? *maxs : moveState->maxs );
	const vec3_t traceEnd		= ( end != nullptr ? *end : moveState->origin );

	const int32_t traceSkipEntityNumber = ( skipEntityNumber != -1 ? skipEntityNumber : moveState->skipEntityNumber );
	const int32_t traceContentMask = ( contentMask != -1 ? contentMask : moveState->contentMask );

	/**
	*	#1: Fetch needed skip entity.
	**/
	// Get Gameworld.
	SGGameWorld *gameWorld = GetGameWorld();
	// Acquire our skip trace entity.
	GameEntity *geSkip = SGGameWorld::ValidateEntity( gameWorld->GetGameEntityByIndex( traceSkipEntityNumber ) );

	// Perform and return trace results.
	return SG_Trace( traceOrigin, traceMins, traceMaxs, traceEnd, geSkip, traceContentMask );
}

/**
*	@brief	Checks whether the SlideMoveState is having any ground below its feet or not.
**/
static void SM_CheckGround( SlideMoveState *moveState ) {
    // If we jumped, or been pushed, do not attempt to seek ground
    if ( moveState->moveFlags & ( SlideMoveMoveFlags::Jumped ) ){// | SlideMoveMoveFlags::OnGround ) ) {
        return;
    }

    // Perform ground seek trace.
    const vec3_t downTraceEnd = moveState->origin + vec3_t{ 0.f, 0.f, -SLIDEMOVE_GROUND_DISTANCE };
	SGTraceResult downTraceResult = SM_Trace( moveState, &moveState->origin, &moveState->mins, &moveState->maxs, &downTraceEnd );
	    
	// Store our ground trace result.
	moveState->groundTrace = downTraceResult;

    // If we hit an upward facing plane, make it our ground
    if ( downTraceResult.gameEntity && IsWalkablePlane( downTraceResult.plane ) ) {

        // If we had no ground, then handle landing events
        if ( moveState->groundEntityNumber == -1 ) {
			// Any landing terminates the water jump
            if ( moveState->moveFlags & SlideMoveMoveFlags::TimeWaterJump ) {
                moveState->moveFlags &= ~SlideMoveMoveFlags::TimeWaterJump;
                moveState->moveFlagTime = 0;
            }

            // Hard landings disable jumping briefly
            if ( moveState->originalVelocity.z <= SLIDEMOVE_SPEED_LAND ) {
                moveState->moveFlags |= SlideMoveMoveFlags::TimeLand;
                moveState->moveFlagTime = 1;

                if ( moveState->originalVelocity.z <= SLIDEMOVE_SPEED_FALL ) {
                    moveState->moveFlagTime = 16;

                    if ( moveState->originalVelocity.z <= SLIDEMOVE_SPEED_FALL_FAR ) {
                        moveState->moveFlagTime = 256;
                    }
                }
            } else {
				// Nothing.
            }
        }

        // Save a reference to the ground
        moveState->moveFlags |= SlideMoveMoveFlags::OnGround;
		moveState->groundEntityNumber = SG_GetEntityNumber( downTraceResult.podEntity );

		// "Sink down" to our ground.
		moveState->origin = downTraceResult.endPosition;
		moveState->velocity = SM_ClipVelocity(moveState->velocity, downTraceResult.plane.normal, SLIDEMOVE_CLIP_BOUNCE);
	} else {
		// Definitely, NOT, on ground.
        moveState->moveFlags &= SlideMoveMoveFlags::OnGround;
		moveState->groundEntityNumber = -1;
    }

    // Always touch the entity, even if we couldn't stand on it
    SM_AddTouchEntity( moveState, downTraceResult.gameEntity );
}

/**
*	@return	True in case the trace result allows this moveState to step(down/up).
**/
static const int32_t SM_CheckStep( SlideMoveState* moveState, const SGTraceResult& stepTrace ) {
	// Get ground entity pointer and its number.
	PODEntity *podGroundEntity = stepTrace .podEntity;
	const int32_t groundEntityNumber = SG_GetEntityNumber( podGroundEntity );

	if ( !stepTrace.allSolid ) {
		if (	( podGroundEntity && IsWalkablePlane( stepTrace.plane ) )  
				&&
					(	( groundEntityNumber != moveState->groundEntityNumber )
						|| ( stepTrace.plane.dist != moveState->groundTrace.plane.dist )	
					)
			)
		{
			// Return true.
			return true;
		} else {
			//moveState->stepHeight = moveState->origin.z - moveState->originalOrigin.z;
		}
	}

	// Could not step.
	return false;
}


bool _SM_StepDownOrEdgeMove_StepDown( SlideMoveState* moveState, const SGTraceResult &traceResult ) {
    // Store pre-move parameters
    const vec3_t org0 = moveState->origin;
    const vec3_t vel0 = moveState->velocity;
				
	// Only return true if it was high enough to "step".
	const float stepHeight = moveState->origin.z - moveState->originalOrigin.z;
	const float absoluteStepHeight = fabs(stepHeight);


	SM_SlideClipMove( moveState, false );


	if (absoluteStepHeight>= SLIDEMOVE_STEP_HEIGHT_MIN) {
		moveState->origin = traceResult.endPosition;

		//moveState->stepHeight = stepHeight;
		return true;
	} else {
		//moveState->origin = traceResult.endPosition;
		//return true; // Return false to prevent animating.?
//		return true;
	}
	//} else {
	//	return false;
	//}
	return false;
}

bool _SM_StepUp_ToFloor( SlideMoveState *moveState, const SGTraceResult &traceResult ) {
	//// Set current origin to the trace end position.
	moveState->origin = traceResult.endPosition;
				
	// Only return true if it was high enough to "step".
	const float stepHeight = moveState->origin.z - traceResult.endPosition.z;
	const float absoluteStepHeight = fabs(stepHeight);

	if (absoluteStepHeight >= SLIDEMOVE_STEP_HEIGHT_MIN) {

		//if (stepHeight > 0) {
		//	// We'll add velocity to try and move to the step.
		//	moveState->velocity.z += absoluteStepHeight;

		//} else {
		//	moveState->velocity.z -= absoluteStepHeight;
		//}
		//// Move.
		//SM_SlideClipMove( moveState, false );

		//moveState->stepHeight = stepHeight;
		return true;
	} else {

		//moveState->origin = traceResult.endPosition;
		return true;
		//return true; // Return false to prevent animating.?
//		return true;
	}
	//} else {
	//	return false;
	//}
	return false;
}

/**
*	@brief	Checks whether we're stepping off a legit step, or moving off an edge and about to
*			fall down.
*	@return	Either 0 if no step or edge move could be made. Otherwise, SlideMoveFlags::SteppedDown 
*			or SlideMoveFlags::EdgeMoved.
**/
static int32_t _SM_EdgeStepMove( SlideMoveState *moveState ) {
    // Store pre-move parameters
    const vec3_t org0 = moveState->origin;
    const vec3_t vel0 = moveState->velocity;

	SM_SlideClipMove( moveState, false );

	if ( !(moveState->moveFlags & SlideMoveMoveFlags::OnGround) && vel0.z <= SLIDEMOVE_STOP_EPSILON  ) {
		// Try to settle to the ground.
		const vec3_t downTraceEnd = vec3_fmaf( moveState->origin, SLIDEMOVE_STEP_HEIGHT + SLIDEMOVE_GROUND_DISTANCE, vec3_down() );
		const SGTraceResult downTraceResult = SM_Trace( moveState, &moveState->origin, &moveState->mins, &moveState->maxs, &downTraceEnd );
		
		if ( SM_CheckStep( moveState, downTraceResult) ) {
			// In case we'd like to not step for register min step height steps.
			if (_SM_StepDownOrEdgeMove_StepDown( moveState, downTraceResult ) ) {
				// If we were falling after an edge move, return a fall down instead.
				//if ( ) {
				//return SlideMoveFlags::FallSteppedDown;
				//} else {

				return SlideMoveFlags::SteppedDown;
				//}
			} else {
								if ( !(moveState->moveFlags & SlideMoveMoveFlags::TimeLand) ) {
					moveState->moveFlags &= ~SlideMoveMoveFlags::TimeLand;
					return SlideMoveFlags::SteppedDownFall;
				}
				// Step here, but without animation so dun return flag?
				//return SlideMoveFlags::EdgeMoved;

			}
		} else {
			if ( !(moveState->moveFlags & SlideMoveMoveFlags::TimeLand) ) {
				moveState->moveFlags |= SlideMoveMoveFlags::TimeLand;

				// We moved over the edge, about to drop down.
				return SlideMoveFlags::EdgeMoved;
			} else {

			}
		}
	} else {
		moveState->origin = org0;
		moveState->velocity = vel0;
	}

	// Can't step or move off an edge.
	return 0;
}

/**
*	@brief	Handles checking whether an entity can step up or not.
*	@return	True if the move stepped up.
**/
static bool _SM_StepUp( SlideMoveState *moveState ) {
	/**
	*	#0: Perform an upwards trace and return false if it is allSolid.
	**/
	// Store origin and velocity in case of failure.
	const vec3_t org0 = moveState->origin;
	const vec3_t vel0 = moveState->velocity;

	// Up Wards trace end point.
	const vec3_t upTraceEnd = vec3_fmaf( org0, SLIDEMOVE_STEP_HEIGHT, vec3_up() );
	// Perform trace.
	const SGTraceResult upTraceResult = SM_Trace( moveState, &org0, &moveState->mins, &moveState->maxs, &upTraceEnd );
	
	// If it is all solid, we can skip stepping up from the higher position.
	if (upTraceResult.allSolid) {
		return false;
	}

	/**
	*	#1: Setup the move state at the upTrace resulting origin and attempt 
	*		to move using original velocity.
	**/
	// Prepare state for moving using original velocity.
	moveState->origin	= upTraceResult.endPosition;
	moveState->velocity = vel0;

	// Perform move.
	SM_SlideClipMove( moveState, false );

	/**
	*	#2: Try and settle to the ground by testing with SM_CheckStep:
	*			- When not complying: Return true and settle to the ground.
	*			- When not complying: Return false and revert to original origin & velocity.
	**/
	// Up Wards trace end point.
	const vec3_t downTraceEnd = vec3_fmaf( moveState->origin, SLIDEMOVE_STEP_HEIGHT + SLIDEMOVE_GROUND_DISTANCE, vec3_down() );
	// Perform Trace.
	const SGTraceResult downTraceResult = SM_Trace( moveState, &moveState->origin, &moveState->mins, &moveState->maxs, &downTraceEnd );
		
	// If the ground trace result complies to all conditions, step to it.
	if ( SM_CheckStep( moveState, downTraceResult ) ) {
		if ( (moveState->moveFlags & SlideMoveMoveFlags::OnGround) || moveState->velocity.z < SLIDEMOVE_SPEED_UP ) {
			_SM_StepUp_ToFloor(moveState, downTraceResult);
			return true;
		}
	}
	
	// Move failed, reset original origin and velocity.
	moveState->origin = org0;
	moveState->velocity = vel0;
			
	return false;
}


/**
*	@brief	Performs the move, adding its colliding plane(s) to our clipping last.
**/
static const int32_t SM_SlideClipMove( SlideMoveState *moveState, const bool stepping ) {
	// Returned containing all move flags that were set during this move.
	int32_t blockedMask = 0;
	// Calculate the wished for move end position.
	const vec3_t endPosition = vec3_fmaf( moveState->origin, moveState->remainingTime, moveState->velocity );
	// Trace the move.
	SGTraceResult traceResult = SM_Trace( moveState, &moveState->origin, &moveState->mins, &moveState->maxs, &endPosition );
	
	// When this happens, chances are you got a shitty day coming up.
	if( traceResult.allSolid ) {
		// Add the entity trapping us to our touch list.
		if( traceResult.gameEntity ) {
			// Touched an other entity.
			SM_AddTouchEntity( moveState, traceResult.gameEntity );
		}

		// Notify we're trapped.
		blockedMask |= SlideMoveFlags::Trapped;
		return blockedMask;
	}

	// This move had no obstalces getting in its way, however...
	if( traceResult.fraction == 1.0f ) {
		// We know we've moved.
		blockedMask |= SlideMoveFlags::Moved;

		// Move ourselves, and see whether we are moving onto a step, or off an edge.
		moveState->origin = traceResult.endPosition;

		// Test for Step/Edge if requested.
		const int32_t edgeStepMask = ( stepping == true ? _SM_EdgeStepMove( moveState) : 0 );

		// Stepping solution:
		if ( stepping && edgeStepMask != 0) {
				// Revert move origin.
				//moveState->origin = traceResult.endPositio
			// If we are edge moving, revert move and clip to normal?
			if ( edgeStepMask & SlideMoveFlags::EdgeMoved ) {
				//moveState->origin = moveState->originalOrigin;
				// Keep our velocity, and clip it to our directional normal.
				//const vec3_t direction = 
			} else {
			//	moveState->origin = traceResult.endPosition;
			}

			// Move all the way.
			//moveState->remainingTime -= ( traceResult.fraction * moveState->remainingTime );	

			return edgeStepMask;
		// Non stepping solution:
		} else {
			moveState->origin = traceResult.endPosition;
			moveState->remainingTime -= ( traceResult.fraction * moveState->remainingTime );	
			return blockedMask | SlideMoveFlags::Moved;
		}
	}

	// Wasn't able to make the full move.
	if( traceResult.fraction < 1.0f ) {
		// Add the touched entity to our list of touched entities.
		const int32_t entityNumber = SM_AddTouchEntity( moveState, traceResult.gameEntity );
		
		// -1 = invalid, 0 = world entity, anything above is a legit touchable entity.
		if (entityNumber > 0) {
			blockedMask |= SlideMoveFlags::EntityTouched;
		}

		// Add a specific flag to our blockedMask stating we bumped into something.
		blockedMask |= SlideMoveFlags::PlaneTouched;

		// Move up covered fraction which is up till the trace result's endPosition.
		if( traceResult.fraction > 0.0 ) {
			moveState->origin = traceResult.endPosition;
			moveState->remainingTime -= ( traceResult.fraction * moveState->remainingTime );
			blockedMask |= SlideMoveFlags::Moved;
		}

		// If we bumped into a non walkable plane(We'll consider it to be a wall.), try end up on top of it.
		if( !IsWalkablePlane( traceResult.plane ) ) {
			vec3_t oldOrg = moveState->origin;
			vec3_t oldVel = moveState->velocity;
			if (stepping && _SM_StepUp(moveState)) {
//				moveState->origin = vec3_mix(oldOrg, moveState->origin, 1.0f * (FRAMETIME.count() / Frametime(moveState->remainingTime).count()));
				blockedMask |= SlideMoveFlags::SteppedUp;
				return blockedMask;
			} else {
				blockedMask |= SlideMoveFlags::WallBlocked;
			}
			//moveState->origin = oldOrg;
			//moveState->velocity = oldVel;
		}

		// Add plane normal to clipping plane list: We didn't step, the plane(assuming wall) blocked us.
		SM_AddClippingPlane( moveState, traceResult.plane.normal );
	}

	return blockedMask;
}



/***
*
*	
*
*	Sliding External API.
*
*	
* 
***/
/**
*	@brief	Executes a SlideMove for the current moveState by clipping its velocity to the touching plane' normals.
*	@return	The blockedMask with the following possible flags, which when set mean:
*			- SlideMoveFlags::PlaneTouched	:	The move has touched a plane.
*			- SlideMoveFlags::WallBlocked	:	The move got blocked by a wall.
*			- SlideMoveFlags::Trapped		:	The move failed, and resulted in the moveState getting trapped.
*												When this is set the last valid Origin is stored in the SlideMoveState.
*			- SlideMoveFlags::EdgeMoved	:	The move got blocked by an edge. (In other words, there was no legitimate step to perform.)
*			- SlideMoveFlags::Moved			:	The move succeeded.
**/
int32_t SG_SlideMove( SlideMoveState *moveState ) {
	/**
	*	#0: Prepare the move.
	**/
	//! Maximum amount of planes to clip against.
	static constexpr int32_t MAX_CLIP_PLANES = 20;
	//! Maximum slide move attempt's we're about to try.
	static constexpr int32_t MAX_SLIDEMOVE_ATTEMPTS = MAX_CLIP_PLANES - 2;
	//! The final resulting blocked mask of this slide move.
	int32_t blockedMask = 0;

	// Store the original Origin and Velocity for Sliding and possible reversion.
	const vec3_t originalOrigin		= moveState->origin;
	const vec3_t originalVelocity	= moveState->velocity;

	// The 'last valid' origin and velocity, they are set after each valid slide attempt.
	vec3_t lastValidOrigin		= moveState->origin;
	vec3_t lastValidVelocity	= moveState->velocity;

	// Clear clipping planes.
	SM_ClearClippingPlanes( moveState );
	// Clear touched entities.
	SM_ClearTouchEntities( moveState );


	/**
	*	#1: Check, and possibly correct the current move state.
	**/
	SM_CheckGround( moveState );


	/**
	*	#2: Add clipping planes from ground (if on-ground), and our own velocity.
	**/
	//if ( moveState->moveFlags & SlideMoveMoveFlags::OnGround ) {
	//	SM_AddClippingPlane( moveState, moveState->groundTrace.plane.normal );
	//}

	// Also don't turn against our original velocity.
	//SM_AddClippingPlane( moveState, moveState->velocity );

	// If the velocity is too small, just stop.
	if( vec3_length( moveState->velocity ) < 0.01f) {// SLIDEMOVE_STOP_EPSILON ) {
		// Zero out its velocity.
		moveState->velocity = vec3_zero();
		moveState->remainingTime = 0;
		return 0;
	}

	for( int32_t count = 0; count < MAX_SLIDEMOVE_ATTEMPTS; count++ ) {
		// Get the original velocity and clip it to all the planes we got in the list.
		moveState->velocity = originalVelocity;

		// Clip the velocity to the current list of clipping planes.
		SM_ClipVelocityToClippingPlanes( moveState );

		// Slide move again, with stepping enabled.
		int32_t slideMoveResultMask = SM_SlideClipMove(moveState, true /*, stepping*/);
		
		// Add the slide move result mask to our blocked mask.
		blockedMask |= slideMoveResultMask;
		//blockedMask &= ~SlideMoveFlags::PlaneTouched;

#ifdef SG_SLIDEMOVE_DEBUG_TRAPPED_MOVES
		// Get GameWorld.
		SGGameWorld	*gameWorld		= GetGameWorld();
		// Get Skip Entity for trace testing.
		GameEntity	*geSkip			= SGGameWorld::ValidateEntity(gameWorld->GetPODEntityByIndex(moveState->skipEntityNumber));

		SGTraceResult traceResult = SG_Trace( moveState->origin, moveState->mins, moveState->maxs, moveState->origin, geSkip, moveState->contentMask );
		if( traceResult.startSolid ) {
			blockedMask |= SlideMoveFlags::Trapped;
		}
#endif

		// Can't continue.
		if( slideMoveResultMask & SlideMoveFlags::Trapped ) {
#ifdef SM_SLIDEMOVE_DEBUG_TRAPPED_MOVES
			SG_Physics_PrintWarning( std::string(__func__) + "SlideMoveFlags::Trapped" );
#endif
			moveState->remainingTime = 0.0f;
			// Copy back in the last valid origin we had, because the move had failed.
			moveState->origin = lastValidOrigin;
			return blockedMask | slideMoveResultMask;
		}

		// Update the last validOrigin to the current moveState origin.
		lastValidOrigin = moveState->origin;

		if ( (slideMoveResultMask & SlideMoveFlags::EdgeMoved) ||
			( slideMoveResultMask & SlideMoveFlags::SteppedDown ) || 
			( slideMoveResultMask & SlideMoveFlags::SteppedUp ) ) {
			continue;
		}

		// Touched a plane, re-clip velocity and retry.
		if( slideMoveResultMask & SlideMoveFlags::PlaneTouched ) {
			//blockedMask &= ~SlideMoveFlags::PlaneTouched;
			continue;
		}

		// If it didn't touch anything the move should be completed
		if( moveState->remainingTime > 0.0f ) {
			SG_Physics_PrintWarning( std::string(__func__) + "Slidemove finished with remaining time" );
			moveState->remainingTime = 0.0f;
		}
		break;
	}

	/**
	*	#:
	**/
	// Once again, check for ground.
	SM_CheckGround( moveState );

	/**
	*	#: DONE! Return the results, at last.
	**/
	return blockedMask;
}




//void SlideMove_FixCheckBottom( GameEntity *geCheck ) {
//	geCheck->SetFlags( geCheck->GetFlags() | EntityFlags::PartiallyOnGround );
//}
//#define STEPSIZE 18
//const bool SlideMove_CheckBottom( SlideMoveState *moveState ) {
//	// Get the moveEntity.
//	SGGameWorld *gameWorld = GetGameWorld();
//	GameEntity *geCheck = SGGameWorld::ValidateEntity( gameWorld->GetPODEntityByIndex( moveState->moveEntityNumber ) );
//	
//	//vec3_t	mins, maxs, start, stop;
//	SGTraceResult trace;
//	int32_t 		x, y;
//	float	mid, bottom;
//
//	const vec3_t origin = moveState->origin;
//	vec3_t mins = origin + moveState->mins;
//	vec3_t maxs = origin + moveState->maxs;
//	
//	// if all of the points under the corners are solid world, don't bother
//	// with the tougher checks
//	// the corners must be within 16 of the midpoint
//	vec3_t start, stop;
//
//	start[2] = mins[2] - 1;
//	for	(x=0 ; x<=1 ; x++)
//		for	(y=0 ; y<=1 ; y++)
//		{
//			start[0] = x ? maxs[0] : mins[0];
//			start[1] = y ? maxs[1] : mins[1];
//			if (SM_PointContents (start) != BrushContents::Solid) {
//				goto realcheck;
//			}
//		}
//
//	return true;		// we got out easy
//
//realcheck:
//	//
//	// check it for real...
//	//
//	start[2] = mins[2];
//	
//	// the midpoint must be within 16 of the bottom
//	start[0] = stop[0] = (mins[0] + maxs[0])*0.5;
//	start[1] = stop[1] = (mins[1] + maxs[1])*0.5;
//	stop[2] = start[2] - 2*STEPSIZE;
//	trace = SG_Trace (start, vec3_zero(), vec3_zero(), stop, geCheck, BrushContentsMask::MonsterSolid);
//
//	if (trace.fraction == 1.0) {
//		return false;
//	}
//	mid = bottom = trace.endPosition[2];
//	
//	// the corners must be within 16 of the midpoint	
//	for	(x=0 ; x<=1 ; x++)
//		for	(y=0 ; y<=1 ; y++)
//		{
//			start[0] = stop[0] = x ? maxs[0] : mins[0];
//			start[1] = stop[1] = y ? maxs[1] : mins[1];
//			
//			trace = SG_Trace (start, vec3_zero(), vec3_zero(), stop, geCheck, BrushContentsMask::MonsterSolid);
//			
//			if (trace.fraction != 1.0 && trace.endPosition[2] > bottom)
//				bottom = trace.endPosition[2];
//			if (trace.fraction == 1.0 || mid - trace.endPosition[2] > STEPSIZE)
//				return false;
//		}
//
//	return true;
//}


///**
//*	@brief	Handles checking whether an entity can step up a brush or not. (Or entity, of course.)
//*	@return	True if the move stepped up.
//**/
//static const int32_t SM_SlideClipMove( SlideMoveState *moveState, const bool stepping );
//static bool SM_StepUp( SlideMoveState *moveState ) {
//    // Store pre-move parameters
//    const vec3_t org0 = moveState->origin;
//    const vec3_t vel0 = moveState->velocity;
//
//	// Slide move, but skip stair testing the plane that we skipped by calling into this function.
//	SM_SlideClipMove( moveState, false );
//
//	/**
//	*	Step Down.
//	**/
//	// Get GameWorld.
//	SGGameWorld	*gameWorld		= GetGameWorld();
//	// Get ground entity.
//	GameEntity	*geGroundEntity	= SGGameWorld::ValidateEntity(gameWorld->GetPODEntityByIndex(moveState->groundEntityNumber));
//	// Get Skip Entity for trace testing.
//	GameEntity	*geSkip			= SGGameWorld::ValidateEntity(gameWorld->GetPODEntityByIndex(moveState->skipEntityNumber));
//
//	// See if we should step down.
//	if ( geGroundEntity && moveState->velocity.z <= 0.1f ) {
//		// Trace downwards.
//		const vec3_t down = vec3_fmaf( org0 , SLIDEMOVE_STEP_HEIGHT + SLIDEMOVE_GROUND_DISTANCE, vec3_down( ) );
//		const SGTraceResult downTrace = SG_Trace(org0, moveState->mins, moveState->maxs, down, geSkip, moveState->contentMask );
//
//		// Check if we should step down or not.
//		if ( !downTrace.allSolid ) {
//			// Check if it is a legitimate stair case.
//			if (downTrace.podEntity && !(downTrace.plane.normal.z >= SLIDEMOVE_STEP_NORMAL) ) {
//				moveState->origin = downTrace.endPosition;
//			}
//		}
//	}
//
//
//	/**
//	*	Step Over.
//	**/
//    // If we are blocked, we will try to step over the obstacle.
//    const vec3_t org1 = moveState->origin;
//    const vec3_t vel1 = moveState->velocity;
//
//    const vec3_t up = vec3_fmaf( org1, SLIDEMOVE_STEP_HEIGHT, vec3_up() );
//    const SGTraceResult upTrace = SG_Trace( org1, moveState->mins, moveState->maxs, up, geSkip, moveState->contentMask );
//
//    if ( !upTrace.allSolid ) {
//        // Step from the higher position, with the original velocity
//        moveState->origin = upTrace.endPosition;
//        moveState->velocity = vel0;
//
//		// Slide move, but skip stair testing the plane that we skipped by calling into this function.
//		SM_SlideClipMove( moveState, true );
//
//		/**
//		*	Settle to Ground.
//		**/
//        // Settle to the new ground, keeping the step if and only if it was successful
//        const vec3_t down = vec3_fmaf( moveState->origin, SLIDEMOVE_STEP_HEIGHT + SLIDEMOVE_GROUND_DISTANCE, vec3_down() );
//        const SGTraceResult downTrace = SG_Trace( moveState->origin, moveState->mins, moveState->maxs, down, geSkip, moveState->contentMask );
//
//        if ( !downTrace.allSolid && downTrace.plane.normal.z >= SLIDEMOVE_STEP_NORMAL ) { //SLIDEMOVE_CheckStep(&downTrace)) {
//            // Quake2 trick jump secret sauce
////#if 0     
//			//SGGameWorld *gameWorld = GetGameWorld();
//			//GameEntity *geGroundEntity = SGGameWorld::ValidateEntity(gameWorld->GetPODEntityByIndex(moveState->groundEntityNumber));
//			//if ( (geGroundEntity) || vel0.z < SLIDEMOVE_SPEED_UP ) {
////#endif
//				//if ( !SlideMove_CheckBottom( moveState ) ) {
//					// Yeah... I knwo.
//					moveState->origin = downTrace.endPosition;
//					moveState->velocity = vel1;
//					if (geGroundEntity) {
//						moveState->groundEntityNumber = geGroundEntity->GetNumber();
//						moveState->groundEntityLinkCount = geGroundEntity->GetLinkCount();
//					}
//				//}
//				// Calculate step height.
//				//moveState->stepHeight = moveState->origin.z - moveState-> 
////#if 0
//			//} else {
//			//	//// Set it back?
//   // //            moveState->origin = org0;
//			//	//moveState->velocity = vel0;
//			//	//return false;
//			//	//pm->step = pm->state.origin.z - playerMoveLocals.previousOrigin.z;
//   //         }
////#endif
//			return true;
//		}
//    }
//	
//	// Save end results.
//    moveState->origin = org1;
//    moveState->velocity = vel1;
//
//	return false;
//}




//		{
//			/**
//			*	#0: Store previousGroundTrace reference and trace to new ground.
//			**/
//			// Get a reference to our previous ground trace.
//			SGTraceResult	&previousGroundTrace		= moveState->groundTrace;
//
//			// Trace for the current remainingTime, by MA-ing the move velocity.
//			const vec3_t	endPosition				= vec3_fmaf( moveState->origin, SLIDEMOVE_STEP_HEIGHT + SLIDEMOVE_GROUND_DISTANCE, vec3_down() );
//			SGTraceResult	newGroundTraceResult	= SG_Trace( moveState->origin, moveState->mins, moveState->maxs, endPosition, geSkip, moveState->contentMask );
//
//			/**
//			*	#1: Get and Validate Ground Game Entities. Check and store if they differed.
//			**/
//			// Validate both ground entities, previous and new.
//			GameEntity *gePreviousGround		= previousGroundTrace.gameEntity;
//			GameEntity *geNewGround				= newGroundTraceResult.gameEntity;
//			GameEntity *geValidPreviousGround	= (gePreviousGround ? SGGameWorld::ValidateEntity( gameWorld->GetGameEntityByIndex( gePreviousGround->GetNumber() ) ) : nullptr);
//			GameEntity *geValidNewGround		= (geNewGround ? SGGameWorld::ValidateEntity( gameWorld->GetGameEntityByIndex( geNewGround->GetNumber() ) ) : nullptr);
//
//			// If their numbers differ, it means we got new ground to step down to.
//			bool newGroundEntity = false;
//
//			if ( ( geValidPreviousGround && geValidNewGround ) && ( geValidPreviousGround->GetNumber() != geValidNewGround->GetNumber() ) ) {
//				newGroundEntity = true;
//			}
//
//			if ( moveState->groundEntityNumber != -1 && !SlideMove_CheckBottom( moveState ) ) {
//				
//
//float movedDistance		 = 0.f;
//float wishedDistance	= 0.f;
//const vec3_t normalizedDirection = vec3_normalize_length( oldOrigin - slideTraceEnd, wishedDistance );
//const vec3_t negatedNormal = vec3_negate( normalizedDirection );
//const vec3_t normalizedDirectionx = vec3_normalize_length( oldOrigin - moveState->origin, movedDistance);
//
//				moveState->velocity = vec3_cross( moveState->velocity, negatedNormal );
//				moveState->remainingTime = (moveState->remainingTime / wishedDistance) * movedDistance;
//					//SM_AddClippingPlane( moveState, negatedNormal );
//				moveState->entityFlags |= EntityFlags::PartiallyOnGround;
//				return blockedMask |= SlideMoveFlags::EdgeMoved;
//			}
//			/**
//			*	#3: Test for the new ground to see if we should step on it.
//			**/
//			if (newGroundEntity || newGroundTraceResult.plane.dist != previousGroundTrace.plane.dist) {
//				// We've got new ground. See if it's a walkable plane.
//				if ( IsWalkablePlane( newGroundTraceResult.plane ) ) {
//					// Subtract total fraction of remaining time.
//					moveState->remainingTime -= ( slideTrace.fraction * moveState->remainingTime );
//
//					// Step down to it.
//					moveState->origin = newGroundTraceResult.endPosition;
//
//					// Make sure to store the new ground entity information.
//					moveState->groundTrace = newGroundTraceResult;
//
//					// Update the entity number and aspect.
//					if (geNewGround) {
//						moveState->groundEntityNumber		= geValidNewGround->GetNumber();
//						moveState->groundEntityLinkCount	= geValidNewGround->GetLinkCount();
//					} else {
//						moveState->groundEntityNumber		= -1;
//						moveState->groundEntityLinkCount	= 0;
//					}
//
//					// Add SlideMoveFlags::SteppedDown flag to our blockedMask and return.
//					return blockedMask | SlideMoveFlags::SteppedDown | SlideMoveFlags::Moved;
//				} else {
//					// Since we're being blocked by an edge, meaning:
//					// "An empty space, something we can't "realistically" step down to."
//					//
//					// We take the direction normal we are moving into, and add that as a 
//					// "fake" clipping plane.
//
//					// Get the length, of the total wished for distance to move.
//	//				float movedDistance = 0.f;
//	//				const vec3_t normalizedDirection = vec3_normalize_length( oldOrigin - slideTraceEnd, movedDistance );
//	//				const vec3_t negatedNormal = vec3_negate( normalizedDirection );
//
//	//				// Adjust origin back to old.
//	//				//moveState->origin = oldOrigin;
//	//				//moveState->velocity = SM_ClipVelocity( moveState->velocity, negatedNormal, moveState->slideBounce );
//
//	//				// Adjust move time to the fraction of what we've moved.
//	//				moveState->remainingTime -= ( (movedDistance / moveState->remainingTime) * moveState->remainingTime );
//
//	//				// Get direction of the trace.
//	////				const vec3_t directionNormal = vec3_negate( vec3_normalize( oldOrigin - slideTraceEnd ) );
//
//	//				// If we've reached this point, it's clear this is one of our planes to clip against.
//	//				SM_AddClippingPlane( moveState, negatedNormal );
//
//	//				std::string debugstr = "Edge Clipped to plane normal: ";
//	//				debugstr += std::to_string(normalizedDirection.x) + " ";
//	//				debugstr += std::to_string(normalizedDirection.y) + " ";
//	//				debugstr += std::to_string(normalizedDirection.z) + " \n";
//	//				SG_Physics_PrintWarning(debugstr);
//					// Add SlideMoveFlags::EdgeMoved mask flag.
//					//return blockedMask |= SlideMoveFlags::EdgeMoved;
//					//// Calculate remaining time.
//					//moveState->remainingTime -= ( slideTrace.fraction * moveState->remainingTime );
//				}
//			} else {
//				// Subtract total fraction of remaining time.
//				moveState->remainingTime -= ( slideTrace.fraction * moveState->remainingTime );
//			}
//		}
