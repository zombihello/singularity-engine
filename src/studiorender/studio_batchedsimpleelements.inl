#pragma once

/*
==================
studioSimplePrimitive_t::studioSimplePrimitive_t
==================
*/
FORCEINLINE studioSimplePrimitive_t::studioSimplePrimitive_t()
{
}

/*
==================
CStudioBatchedSimpleElements::R_Draw
==================
*/
FORCEINLINE void CStudioBatchedSimpleElements::R_Draw( IStudioAPICmdList* pCmdList, studioRenderPassType_t renderPassType )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	for ( uint32 batchType = 0; batchType < BATCH_NUM_TYPES; ++batchType )
	{
		R_DrawBatch( pCmdList, renderPassType, (batchType_t)batchType );
	}
}

/*
==================
CStudioBatchedSimpleElements::R_IsEmpty
==================
*/
FORCEINLINE bool CStudioBatchedSimpleElements::R_IsEmpty() const
{
	for ( uint32 batchType = 0; batchType < BATCH_NUM_TYPES; ++batchType )
	{
		if ( !batches[batchType].empty() )
		{
			return false;
		}
	}
	return true;
}
