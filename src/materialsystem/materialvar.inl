#pragma once

/*
==================
CMaterialVar::IsResourceVarType
==================
*/
FORCEINLINE bool CMaterialVar::IsResourceVarType( materialVarType_t varType )
{
	return varType == MATERIALVAR_TYPE_TEXTURE || varType == MATERIALVAR_TYPE_MATERIAL;
}

/*
==================
CMaterialVar::GetId
==================
*/
FORCEINLINE uint32 CMaterialVar::GetId() const
{
	return id;
}
