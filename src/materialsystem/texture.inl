#pragma once

/*
==================
CTextureResource::GetRenderCmdFence
==================
*/
FORCEINLINE CStudioRenderCmdFence& CTextureResource::GetRenderCmdFence()
{
	return renderCmdFence;
}