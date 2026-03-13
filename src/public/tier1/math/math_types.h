#pragma once
#include <glm.hpp>

//-----------------------------------------------------------------------------
// Math types
//-----------------------------------------------------------------------------
typedef glm::vec2  vector2_t;
typedef glm::ivec2 vector2i_t;
typedef glm::bvec2 vector2b_t;
typedef glm::vec3  vector3_t;
typedef glm::ivec3 vector3i_t;
typedef glm::bvec3 vector3b_t;
typedef glm::vec4  vector4_t;
typedef glm::ivec4 vector4i_t;
typedef glm::bvec4 vector4b_t;
typedef glm::mat4  matrix4x4_t;
typedef glm::quat  quaternion_t;

//-----------------------------------------------------------------------------
// Template helper structs
//-----------------------------------------------------------------------------
template<typename TVectorType>
struct vectorBoolType_t
{
};

template<>
struct vectorBoolType_t<vector2_t>
{
	typedef vector2b_t type_t;
};
template<>
struct vectorBoolType_t<vector2i_t>
{
	typedef vector2b_t type_t;
};
template<>
struct vectorBoolType_t<vector2b_t>
{
	typedef vector2b_t type_t;
};

template<>
struct vectorBoolType_t<vector3_t>
{
	typedef vector3b_t type_t;
};
template<>
struct vectorBoolType_t<vector3i_t>
{
	typedef vector3b_t type_t;
};
template<>
struct vectorBoolType_t<vector3b_t>
{
	typedef vector3b_t type_t;
};

template<>
struct vectorBoolType_t<vector4_t>
{
	typedef vector4b_t type_t;
};
template<>
struct vectorBoolType_t<vector4i_t>
{
	typedef vector4b_t type_t;
};
template<>
struct vectorBoolType_t<vector4b_t>
{
	typedef vector4b_t type_t;
};
