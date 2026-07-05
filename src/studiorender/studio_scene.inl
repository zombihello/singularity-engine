#pragma once

/*
==================
CStudioScene::GetEntities
==================
*/
FORCEINLINE const eastl::vector<studioEntity_t>& CStudioScene::GetEntities() const
{
	return entities;
}