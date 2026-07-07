#pragma once
#include "tier1/framealloc.h"
#include "studiorender/studio_defines.h"

//-----------------------------------------------------------------------------
// Studio frame allocator
//-----------------------------------------------------------------------------
extern CFrameAlloc<STUDIO_FRAMEALLOC_BLOCK_SIZE, STUDIO_FRAMEALLOC_NUM_POOLS> g_studioFrameAlloc;
using studioStlFrameAlloc_t = CStlFrameAlloc<g_studioFrameAlloc>;

//-----------------------------------------------------------------------------
// Aliases to use `eastl::vector`, `eastl::unordered_map` and another
// containers with `studioStlFrameAlloc_t`
//-----------------------------------------------------------------------------
template<typename Value>
using studioFrameVector_t = eastl::vector<Value, studioStlFrameAlloc_t>;
template<typename Value, typename Hash = eastl::hash<Value>, typename Predicate = eastl::equal_to<Value>>
using studioFrameHashSet_t = eastl::unordered_set<Value, Hash, Predicate, studioStlFrameAlloc_t>;
template<typename Key, typename Value, typename Hash = eastl::hash<Key>, typename Predicate = eastl::equal_to<Key>>
using studioFrameHashMap_t = eastl::unordered_map<Key, Value, Hash, Predicate, studioStlFrameAlloc_t>;
