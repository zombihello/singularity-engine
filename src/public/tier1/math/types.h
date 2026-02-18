#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm.hpp>
#include <gtx/quaternion.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/transform.hpp>
#include <gtx/compatibility.hpp>

//-----------------------------------------------------------------------------
// Math constants
//-----------------------------------------------------------------------------
#undef PI
#undef SMALL_NUMBER
#undef KINDA_SMALL_NUMBER
#undef BIG_NUMBER
#undef EULERS_NUMBER

#define PI				   ( 3.1415926535897932 )
#define SMALL_NUMBER	   ( 1.e-8 )
#define KINDA_SMALL_NUMBER ( 1.e-4 )
#define BIG_NUMBER		   ( 3.4e+38f )
#define EULERS_NUMBER	   ( 2.71828182845904523536 )

//-----------------------------------------------------------------------------
// Math types
//-----------------------------------------------------------------------------
typedef glm::vec2  vec2_t;
typedef glm::ivec2 ivec2_t;
typedef glm::vec3  vec3_t;
typedef glm::ivec3 ivec3_t;
typedef glm::vec4  vec4_t;
typedef glm::ivec4 ivec4_t;
typedef glm::mat4  mat4_t;
typedef glm::quat  quat_t;
