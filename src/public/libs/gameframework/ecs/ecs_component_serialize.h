#ifndef ECS_COMPONENT_SERIALIZE_H
#define ECS_COMPONENT_SERIALIZE_H

#include "sentdoc/sentvar.h"

//-----------------------------------------------------------------------------
// Read a data from a SENT variable
//-----------------------------------------------------------------------------
template<typename TType>
void EcsReadData( TType& data, const CSENTEntityDescVar& sentVar, const TType& defaultValue = TType() );

// Here you can add your override function to read a data from a SENT variable

#include "gameframework/ecs/ecs_component_serialize.inl"

#endif // !ECS_COMPONENT_SERIALIZE_H
