#pragma once

/*
==================
CTransform::CopyTranslation
==================
*/
FORCEINLINE void CTransform::CopyTranslation( const CTransform& other )
{
	translation	 = other.translation;
	bDirtyMatrix = true;
}

/*
==================
CTransform::AddToTranslation
==================
*/
FORCEINLINE void CTransform::AddToTranslation( const vec3_t& deltaTranslation )
{
	translation += deltaTranslation;
	bDirtyMatrix = true;
}

/*
==================
CTransform::SubtractFromTranslation
==================
*/
FORCEINLINE void CTransform::SubtractFromTranslation( const vec3_t& deltaTranslation )
{
	translation -= deltaTranslation;
	bDirtyMatrix = true;
}

/*
==================
CTransform::AddToRotation
==================
*/
FORCEINLINE void CTransform::AddToRotation( const quat_t& deltaRotation )
{
	rotation	 = deltaRotation * rotation;
	bDirtyMatrix = true;
}

/*
==================
CTransform::SubtractFromRotation
==================
*/
FORCEINLINE void CTransform::SubtractFromRotation( const quat_t& deltaRotation )
{
	rotation	 = S_QuaternionInverse( deltaRotation ) * rotation;
	bDirtyMatrix = true;
}

/*
==================
CTransform::AddToScale
==================
*/
FORCEINLINE void CTransform::AddToScale( const vec3_t& deltaScale )
{
	scale += deltaScale;
	bDirtyMatrix = true;
}

/*
==================
CTransform::SubtractFromScale
==================
*/
FORCEINLINE void CTransform::SubtractFromScale( const vec3_t& deltaScale )
{
	scale -= deltaScale;
	bDirtyMatrix = true;
}

/*
==================
CTransform::CopyRotation
==================
*/
FORCEINLINE void CTransform::CopyRotation( const CTransform& other )
{
	rotation	 = other.rotation;
	bDirtyMatrix = true;
}

/*
==================
CTransform::CopyScale
==================
*/
FORCEINLINE void CTransform::CopyScale( const CTransform& other )
{
	scale		 = other.scale;
	bDirtyMatrix = true;
}

/*
==================
CTransform::Add
==================
*/
FORCEINLINE void CTransform::Add( const CTransform& other )
{
	translation += other.translation;
	rotation = other.rotation * rotation;
	scale += other.scale;
	bDirtyMatrix = true;
}

/*
==================
CTransform::Subtract
==================
*/
FORCEINLINE void CTransform::Subtract( const CTransform& other )
{
	translation -= other.translation;
	rotation = S_QuaternionInverse( other.rotation ) * rotation;
	scale -= other.scale;
	bDirtyMatrix = true;
}

/*
==================
CTransform::RotateVector
==================
*/
FORCEINLINE vec3_t CTransform::RotateVector( const vec3_t& vector ) const
{
	return rotation * vector;
}

/*
==================
CTransform::MatchesNoScale
==================
*/
FORCEINLINE bool CTransform::MatchesNoScale( const CTransform& otherTransform ) const
{
	return translation == otherTransform.translation && rotation == otherTransform.rotation;
}

/*
==================
CTransform::Matches
==================
*/
FORCEINLINE bool CTransform::Matches( const CTransform& otherTransform ) const
{
	return MatchesNoScale( otherTransform ) && scale == otherTransform.scale;
}

/*
==================
CTransform::SetLocation
==================
*/
FORCEINLINE void CTransform::SetLocation( const vec3_t& newLocation )
{
	translation	 = newLocation;
	bDirtyMatrix = true;
}

/*
==================
CTransform::SetRotation
==================
*/
FORCEINLINE void CTransform::SetRotation( const quat_t& newRotation )
{
	rotation	 = newRotation;
	bDirtyMatrix = true;
}

/*
==================
CTransform::SetScale
==================
*/
FORCEINLINE void CTransform::SetScale( const vec3_t& newScale )
{
	scale		 = newScale;
	bDirtyMatrix = true;
}

/*
==================
CTransform::SetIdentity
==================
*/
FORCEINLINE void CTransform::SetIdentity()
{
	translation	 = g_vectorZero;
	rotation	 = g_quaternionZero;
	scale		 = g_vectorOne;
	bDirtyMatrix = true;
}

/*
==================
CTransform::GetUnitAxis
==================
*/
FORCEINLINE vec3_t CTransform::GetUnitAxis( axis_t axis ) const
{
	switch ( axis )
	{
	case AXIS_X: return RotateVector( vec3_t( 1.f, 0.f, 0.f ) ); break;
	case AXIS_Y: return RotateVector( vec3_t( 0.f, 1.f, 0.f ) ); break;
	case AXIS_Z: return RotateVector( vec3_t( 0.f, 0.f, 1.f ) ); break;
	default: Assert( false ); break;
	}

	return g_vectorZero;
}

/*
==================
CTransform::GetLocation
==================
*/
FORCEINLINE vec3_t CTransform::GetLocation() const
{
	return translation;
}

/*
==================
CTransform::GetRotation
==================
*/
FORCEINLINE quat_t CTransform::GetRotation() const
{
	return rotation;
}

/*
==================
CTransform::GetScale
==================
*/
FORCEINLINE vec3_t CTransform::GetScale() const
{
	return scale;
}

/*
==================
CTransform::AsMatrix
==================
*/
FORCEINLINE const mat4_t& CTransform::AsMatrix() const
{
	if ( bDirtyMatrix )
	{
		AsMatrix( matrix );
		bDirtyMatrix = false;
	}
	return matrix;
}

/*
==================
CTransform::AsMatrix
==================
*/
FORCEINLINE void CTransform::AsMatrix( mat4_t& destMatrix ) const
{
	if ( bDirtyMatrix )
	{
		matrix		 = S_MatrixTranslate( translation ) * S_QuaternionToMatrix( rotation ) * S_MatrixScale( scale );
		bDirtyMatrix = false;
	}
	destMatrix = matrix;
}

/*
==================
CTransform::operator+
==================
*/
FORCEINLINE CTransform CTransform::operator+( const CTransform& other ) const
{
	return CTransform( other.rotation * rotation, translation + other.translation, scale + other.scale );
}

/*
==================
CTransform::operator-
==================
*/
FORCEINLINE CTransform CTransform::operator-( const CTransform& other ) const
{
	return CTransform( S_QuaternionInverse( other.rotation ) * rotation, translation - other.translation, scale - other.scale );
}
