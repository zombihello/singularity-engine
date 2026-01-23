#pragma once
#include "utils/sentdoc/sentvar.h"
#include "utils/interfaces/interfaces.h"
#include "materialsystem/imaterial.h"
#include "materialsystem/itexture.h"
#include "game/ientitydesc.h"
#include "resourcesystem/iresource.h"
#include "resourcesystem/iresourcesystem.h"

//-----------------------------------------------------------------------------
// Read a data from a SENT variable
//-----------------------------------------------------------------------------
template<typename TType>
void EcsReadData( TType& data, const CSENTEntityDescVar& sentVar, const TType& defaultValue = TType() );

// Here you can add your override function to read a data from a SENT variable

#include "game/shared/ecs/ecs_component_serialize.inl"
