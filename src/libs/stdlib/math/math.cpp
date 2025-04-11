#include "pch_stdlib.h"
#include "stdlib/math/rotator.h"
#include "stdlib/math/transform.h"
#include "stdlib/math/math.h"

const vec3_t			math_t::vectorZero( 0.f, 0.f, 0.f );
const vec3_t			math_t::vectorOne( 1.f, 1.f, 1.f );
const quat_t			math_t::quaternionZero( math_t::AnglesToQuaternion( 0.f, 0.f, 0.f ) );
const CRotator			math_t::rotatorZero( 0.f, 0.f, 0.f );
const matrix_t			math_t::matrixIdentity( 1.f );
const CTransform		math_t::transformZero( math_t::quaternionZero, math_t::vectorOne, math_t::vectorOne );
const vec3_t			math_t::vectorForward( 0.f, 0.f, 1.f );
const vec3_t			math_t::vectorRight( 1.f, 0.f, 0.f );
const vec3_t			math_t::vectorUp( 0.f, 1.f, 0.f );