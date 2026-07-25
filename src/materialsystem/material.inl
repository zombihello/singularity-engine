#pragma once

/*
==================
CMaterialResource::IsValid
==================
*/
FORCEINLINE bool CMaterialResource::IsValid() const
{
	return pShader && pPerMaterialContextData;
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
