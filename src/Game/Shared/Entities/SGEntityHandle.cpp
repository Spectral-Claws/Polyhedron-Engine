/***
*
*	License here.
*
*	@file
*
*	EntityHandle implementation.
*
***/
// Shared Game include.
#include "../SharedGame.h"

#ifdef SHAREDGAME_SERVERGAME
#define GAME_INCLUDE
#include "Shared/SVGame.h"
#endif


/**
*	@brief	Helper function to acquire the ClassEntity pointer from a server entity.
* 
*	@return	A valid pointer if the POD Entity is pointing to one. nullptr otherwise.
**/
static ClassEntity* GetClassEntity(PODEntity* podEntity) {
    // Reinterpret cast the classEntity pointer.
    if (podEntity) {
	    if (podEntity->classEntity != nullptr) {
	        return podEntity->classEntity;
	    } else {
	        return nullptr;
	    }
    }

    // Return nullptr.
    return nullptr;
}




/**
*	@brief Simple constructor of an entity handle that will accept a
*	class entity.
**/
SGEntityHandle::SGEntityHandle(ISharedGameEntity* classEntity) : podEntity(nullptr), entityID(0) {
    // Exploits the assignment operator.
    *this = classEntity;
}

/**
*	@brief Simple constructor that will accept a reference to another handle entity.
**/
SGEntityHandle::SGEntityHandle(SGEntityHandle& other) : podEntity(other.podEntity), entityID(other.entityID) { }


/**
*	@brief Simple constructor that will accept a const reference to another handle entity.
**/
SGEntityHandle::SGEntityHandle(const SGEntityHandle& other) : podEntity(other.podEntity), entityID(other.entityID) { }

/**
*	@brief Simple constructor that will accept a server entity.
**/
SGEntityHandle::SGEntityHandle(PODEntity* podEntity) { 
    if (podEntity) {
        // Assign POD Entity.
        this->podEntity = podEntity;

        // Assign Entity ID.
#if defined(SHAREDGAME_CLIENTGAME)
        entityID    = podEntity->clientEntityNumber;
#elif defined(SHAREDGAME_SERVERGAME)
        entityID    = podEntity->state.number;
#endif
    } else {
        podEntity   = nullptr;
        entityID    = 0;
    }
}

/**
*	@brief Simple constructor that will accept a server entity and an entity number.
**/
SGEntityHandle::SGEntityHandle(PODEntity* _podEntity, const uint32_t number) : podEntity(_podEntity), entityID(number) { }

/**
*	@brief	Acquire a pointer to the server entity.
*
*	@return	If the entityIDs match, a pointer to the server entity. 
*			Nullptr otherwise.
**/
PODEntity* SGEntityHandle::Get() const {
    // Ensure we got a valid server entity pointer.
    if (podEntity) {
#if defined(SHAREDGAME_CLIENTGAME)
        if (podEntity->clientEntityNumber == entityID) {
#elif defined(SHAREDGAME_SERVERGAME)
		if (podEntity->state.number == entityID) {
#endif
            return podEntity;
		} else {
		    return nullptr;
		}
    }

    // Immediately return a nullptr since we never made it to the inside of the
    // if clausule.
    return nullptr;
}

/**
*	@brief	Sets the server entity pointer and assigns its entity ID to this
*			handle in case of a non nullptr.
*
*	@return	The pointer passed to the function name.
**/
PODEntity* SGEntityHandle::Set(PODEntity* entity) {
    // Set new pointer.
    podEntity = entity;

    // In case of a valid entity ptr assign its number to our entityID.
    if (entity) {
#if defined(SHAREDGAME_CLIENTGAME)
        entityID = entity->clientEntityNumber;
#elif defined(SHAREDGAME_SERVERGAME)
    	entityID = entity->state.number;
#endif
    }

    // Return entity ptr.
    return entity;
}

/**
*	@return	The entityID stored in this handle.
**/
const uint32_t SGEntityHandle::ID() { return entityID; }

/**
*	@brief * operator implementations.
**/
ClassEntity* SGEntityHandle::operator*() { return (ClassEntity*)GetClassEntity(Get()); }
const ClassEntity* SGEntityHandle::operator*() const { return (ClassEntity*)GetClassEntity(Get()); }

/**
*	@brief	Assigns the ClassEntity to this handle if it has a valid server entity.
*			If no valid ClassEntity and server entity pointer are passed it unsets
*			this current handle to nullptr and entityID = 0.
**/
ISharedGameEntity* SGEntityHandle::operator=(ISharedGameEntity* classEntity) {
	// Ensure ClassEntity pointer is valid.
    if (classEntity) {
		// Acquire server entity pointer.
		podEntity = classEntity->GetPODEntity();

		// In case of a valid server entity pointer, assign entityID to its number.
		if (podEntity) {
#if defined(SHAREDGAME_CLIENTGAME)
            entityID = podEntity->clientEntityNumber;
#elif defined(SHAREDGAME_SERVERGAME)
    	    entityID = podEntity->state.number;
#endif
		}
	} else {
		// No valid ClassEntity pointer, so reset this entity handle.
		podEntity = nullptr;
		entityID = 0;
	}

	// Last but not least, return.
	return classEntity;
}


/**
*	@brief	Used to access the class entity its methods.
**/
ClassEntity* SGEntityHandle::operator->() const { return (ClassEntity*)GetClassEntity(Get()); }

/**
*   @brief  Comparison check for whether this handle points to the same POD Entity as 
*           the ClassEntity pointer does.
* 
*   @return Returns true if ClassEntity* != nullptr, its podEntity pointer 
*           != nullptr, and their entity index number matches.
**/
bool SGEntityHandle::operator==(const ISharedGameEntity* classEntity) {
    if (!classEntity) {
	    return false;
    }

    PODEntity* podEntity = const_cast<ISharedGameEntity*>(classEntity)->GetPODEntity();

    if (!podEntity) {
		return false;
    }

#if defined(SHAREDGAME_CLIENTGAME)
    if (podEntity->clientEntityNumber != entityID) {
#elif defined(SHAREDGAME_SERVERGAME)
    if (podEntity->state.number != entityID) {
#endif
		return false;
    }

    return true;
}

/**
*   @brief Used to check whether this entity handle has a valid server entity.
**/
SGEntityHandle::operator bool() { return (podEntity != nullptr); }