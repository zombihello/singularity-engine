#pragma once

/*
==================
CMaterialResource::IsValid
==================
*/
FORCEINLINE bool CMaterialResource::IsValid() const
{
	return pShader && pContextData;
}

/*
==================
CMaterialResource::GetRenderCmdFence
==================
*/
FORCEINLINE CStudioRenderCmdFence& CMaterialResource::GetRenderCmdFence()
{
	return renderCmdFence;
}