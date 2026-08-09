#pragma once
#include <glm.hpp>

//-----------------------------------------------------------------------------
// Math types
//-----------------------------------------------------------------------------
using vector2_t	   = glm::vec2;
using vector2i_t   = glm::ivec2;
using vector2b_t   = glm::bvec2;
using vector3_t	   = glm::vec3;
using vector3i_t   = glm::ivec3;
using vector3b_t   = glm::bvec3;
using vector4_t	   = glm::vec4;
using vector4i_t   = glm::ivec4;
using vector4b_t   = glm::bvec4;
using matrix3x3_t  = glm::mat3;
using matrix4x4_t  = glm::mat4;
using matrix4x3_t  = glm::mat4x3;
using matrix3x4_t  = glm::mat3x4;
using quaternion_t = glm::quat;

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

template<typename TMatrixType>
struct matrixTransposeType_t
{
};

template<>
struct matrixTransposeType_t<matrix3x3_t>
{
	using type_t = matrix3x3_t;
};
template<>
struct matrixTransposeType_t<matrix4x4_t>
{
	using type_t = matrix4x4_t;
};
template<>
struct matrixTransposeType_t<matrix4x3_t>
{
	using type_t = matrix3x4_t;
};
template<>
struct matrixTransposeType_t<matrix3x4_t>
{
	using type_t = matrix4x3_t;
};
