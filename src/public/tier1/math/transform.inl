#pragma once

/*
==================
CTransform::CTransform
==================
*/
FORCEINLINE CTransform::CTransform()
	: rotation( g_quaternionIdentity )
	, location( g_vector000 )
	, scale( g_vector111 )
{
}

/*
==================
CTransform::CTransform
==================
*/
FORCEINLINE CTransform::CTransform( const vector3_t& location, const quaternion_t& rotation /* = g_quaternionIdentity */, const vector3_t& scale /* = g_vector111 */ )
	: rotation( rotation )
	, location( location )
	, scale( scale )
{
}

/*
==================
CTransform::CTransform
==================
*/
FORCEINLINE CTransform::CTransform( const CTransform& copy )
	: rotation( copy.rotation )
	, location( copy.location )
	, scale( copy.scale )
{
}

/*
==================
CTransform::Make
==================
*/
FORCEINLINE CTransform CTransform::Make( const vector3_t& location, const quaternion_t& rotation /* = g_quaternionIdentity */, const vector3_t& scale /* = g_vector111 */ )
{
	return CTransform( location, rotation, scale );
}

/*
==================
CTransform::Make
==================
*/
FORCEINLINE CTransform CTransform::Make( const quaternion_t& rotation )
{
	return CTransform( rotation );
}

/*
==================
CTransform::Translate
==================
*/
FORCEINLINE void CTransform::Translate( const vector3_t& deltaLocation )
{
	location += deltaLocation;
}

/*
==================
CTransform::Rotate
==================
*/
FORCEINLINE void CTransform::Rotate( const quaternion_t& deltaRotation )
{
	rotation = deltaRotation * rotation;
}

/*
==================
CTransform::Scale
==================
*/
FORCEINLINE void CTransform::Scale( const vector3_t& deltaScale )
{
	scale *= deltaScale;
}

/*
==================
CTransform::TranslateVector
==================
*/
FORCEINLINE vector3_t CTransform::TranslateVector( const vector3_t& vector ) const
{
	vector3_t result;
	TranslateVector( vector, result );
	return result;
}

/*
==================
CTransform::TranslateVector
==================
*/
FORCEINLINE void CTransform::TranslateVector( const vector3_t& vector, vector3_t& result ) const
{
	result = location + vector;
}

/*
==================
CTransform::RotateVector
==================
*/
FORCEINLINE vector3_t CTransform::RotateVector( const vector3_t& vector ) const
{
	vector3_t result;
	RotateVector( vector, result );
	return result;
}

/*
==================
CTransform::RotateVector
==================
*/
FORCEINLINE void CTransform::RotateVector( const vector3_t& vector, vector3_t& result ) const
{
	result = rotation * vector;
}

/*
==================
CTransform::ScaleVector
==================
*/
FORCEINLINE vector3_t CTransform::ScaleVector( const vector3_t& vector ) const
{
	vector3_t result;
	ScaleVector( vector, result );
	return result;
}

/*
==================
CTransform::ScaleVector
==================
*/
FORCEINLINE void CTransform::ScaleVector( const vector3_t& vector, vector3_t& result ) const
{
	result = scale * vector;
}

/*
==================
CTransform::TransformVector
==================
*/
FORCEINLINE vector3_t CTransform::TransformVector( const vector3_t& vector ) const
{
	vector3_t result;
	TransformVector( vector, result );
	return result;
}

/*
==================
CTransform::TransformVector
==================
*/
FORCEINLINE void CTransform::TransformVector( const vector3_t& vector, vector3_t& result ) const
{
	result = rotation * ( scale * vector ) + location;
}

/*
==================
CTransform::Identity
==================
*/
FORCEINLINE void CTransform::Identity()
{
	location = g_vector000;
	rotation = g_quaternionIdentity;
	scale	 = g_vector111;
}

/*
==================
CTransform::Inverse
==================
*/
FORCEINLINE void CTransform::Inverse()
{
	quaternion_t invRotation;
	vector3_t	 invScale( 1.f / scale.x, 1.f / scale.y, 1.f / scale.z );
	S_QuaternionInverse( rotation, invRotation );

	vector3_t newLocation = invScale * location;
	newLocation			  = invRotation * newLocation;

	newLocation = -newLocation;
	rotation	= invRotation;
	scale		= invScale;
}

/*
==================
CTransform::SetLocation
==================
*/
FORCEINLINE void CTransform::SetLocation( const vector3_t& location )
{
	CTransform::location = location;
}

/*
==================
CTransform::SetRotation
==================
*/
FORCEINLINE void CTransform::SetRotation( const quaternion_t& rotation )
{
	CTransform::rotation = rotation;
}

/*
==================
CTransform::SetScale
==================
*/
FORCEINLINE void CTransform::SetScale( const vector3_t& scale )
{
	CTransform::scale = scale;
}

/*
==================
CTransform::ToMatrix
==================
*/
FORCEINLINE matrix4x4_t CTransform::ToMatrix() const
{
	matrix4x4_t result;
	ToMatrix( result );
	return result;
}

/*
==================
CTransform::ToMatrix
==================
*/
FORCEINLINE void CTransform::ToMatrix( matrix4x4_t& result ) const
{
	result = S_MatrixTranslate( location ) * S_QuaternionToMatrix( rotation ) * S_MatrixScale( scale );
}

/*
==================
CTransform::GetUnitAxis
==================
*/
FORCEINLINE vector3_t CTransform::GetUnitAxis( axis_t axis ) const
{
	vector3_t result;
	GetUnitAxis( axis, result );
	return result;
}

/*
==================
CTransform::GetUnitAxis
==================
*/
FORCEINLINE void CTransform::GetUnitAxis( axis_t axis, vector3_t& result ) const
{
	switch ( axis )
	{
	case AXIS_X: RotateVector( g_vector100, result ); break;
	case AXIS_Y: RotateVector( g_vector010, result ); break;
	case AXIS_Z: RotateVector( g_vector001, result ); break;
	default:
		AssertMsg( false, "Unknown axis 0x%X", axis );
		result = g_vector000;
		break;
	}
}

/*
==================
CTransform::GetInverse
==================
*/
FORCEINLINE CTransform CTransform::GetInverse() const
{
	CTransform result;
	GetInverse( result );
	return result;
}

/*
==================
CTransform::GetInverse
==================
*/
FORCEINLINE void CTransform::GetInverse( CTransform& result ) const
{
	result = *this;
	result.Inverse();
}

/*
==================
CTransform::GetLocation
==================
*/
FORCEINLINE const vector3_t& CTransform::GetLocation() const
{
	return location;
}

/*
==================
CTransform::GetRotation
==================
*/
FORCEINLINE const quaternion_t& CTransform::GetRotation() const
{
	return rotation;
}

/*
==================
CTransform::GetScale
==================
*/
FORCEINLINE const vector3_t& CTransform::GetScale() const
{
	return scale;
}

/*
==================
CTransform::operator=
==================
*/
FORCEINLINE CTransform& CTransform::operator=( const CTransform& right )
{
	location = right.location;
	rotation = right.rotation;
	scale	 = right.scale;
	return *this;
}

/*
==================
CTransform::operator*
==================
*/
FORCEINLINE vector3_t CTransform::operator*( const vector3_t& right ) const
{
	vector3_t result;
	TransformVector( right, result );
	return result;
}

/*
==================
CTransform::operator*
==================
*/
FORCEINLINE CTransform CTransform::operator*( const CTransform& right ) const
{
	return CTransform( right.rotation * ( right.scale * location ) + right.location, right.rotation * rotation, right.scale * scale );
}

/*
==================
CTransform::operator==
==================
*/
FORCEINLINE bool CTransform::operator==( const CTransform& right ) const
{
	return location == right.location && rotation == right.rotation && scale == right.scale;
}

/*
==================
CTransform::operator!=
==================
*/
FORCEINLINE bool CTransform::operator!=( const CTransform& right ) const
{
	return location != right.location || rotation != right.rotation || scale != right.scale;
}
