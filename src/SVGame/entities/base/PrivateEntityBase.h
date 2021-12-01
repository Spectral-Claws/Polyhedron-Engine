/*
// LICENSE HERE.

//
// PrivateEntityBase.h
//
//
*/
#pragma once

#include "Shared/Entities/Base/EntityBase.h"
#include "Shared/Entities/Base/ServerEntity.h"

//---------------------------------------------------------
// Used for entities that have no need to take up an entity
// slot. For example, consider a path_corner or info_notnull
// they are required, and useful, but a waste of consuming
// an entity slot.
//----------------------------------------------------------
class PrivateEntityBase abstract : public EntityBase { 
protected:
    // Internal memory 
    ServerEntity handleMemory;

public:
    // Constructor.
    PrivateEntityBase();
};