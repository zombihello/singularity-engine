#pragma once

/*
==================
CResource::SetData
==================
*/
FORCEINLINE void CResource::SetData( IRefCounted* pData, resourceType_t type )
{
	CResource::type = type;
	CResource::pData = pData;
}

/*
==================
CResource::IsProcedural
==================
*/
FORCEINLINE bool CResource::IsProcedural() const
{
	return bProcedural;
}

/*
==================
CResource::GetPath
==================
*/
FORCEINLINE const achar* CResource::GetPath() const
{
	return path.c_str();
}