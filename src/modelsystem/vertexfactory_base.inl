#pragma once

/*
==================
CVertexDeclarationBase::GetStudioAPIVertexDeclaration
==================
*/
FORCEINLINE IStudioAPIVertexDeclaration* CVertexDeclarationBase::GetStudioAPIVertexDeclaration() const
{
	return pStudioAPIVertexDeclaration;
}
