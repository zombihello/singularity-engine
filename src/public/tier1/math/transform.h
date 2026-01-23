#pragma once
#include "tier1/math/math.h"
#include "tier0/debug.h"

//-----------------------------------------------------------------------------
// Class for work with transform
//-----------------------------------------------------------------------------
class CTransform
{
public:
	CTransform()
		: bDirtyMatrix( true )
		, translation( g_vectorZero )
		, rotation( g_quaternionZero )
		, scale( g_vectorOne )
		, matrix( g_matrixIdentity )
	{
	}
	explicit CTransform( const vec3_t& translation )
		: bDirtyMatrix( true )
		, translation( translation )
		, rotation( g_quaternionZero )
		, scale( g_vectorOne )
		, matrix( g_matrixIdentity )
	{
	}
	explicit CTransform( const quat_t& rotation )
		: bDirtyMatrix( true )
		, translation( g_vectorZero )
		, rotation( rotation )
		, scale( g_vectorOne )
		, matrix( g_matrixIdentity )
	{
	}
	CTransform( const quat_t& rotation, const vec3_t& translation, const vec3_t& scale = g_vectorOne )
		: bDirtyMatrix( true )
		, translation( translation )
		, rotation( rotation )
		, scale( scale )
		, matrix( g_matrixIdentity )
	{
	}

	void AddToTranslation( const vec3_t& deltaTranslation );
	void AddToRotation( const quat_t& deltaRotation );
	void AddToScale( const vec3_t& deltaScale );
	void Add( const CTransform& other );

	void SubtractFromTranslation( const vec3_t& deltaTranslation );
	void SubtractFromRotation( const quat_t& deltaRotation );
	void SubtractFromScale( const vec3_t& deltaScale );
	void Subtract( const CTransform& other );

	void CopyTranslation( const CTransform& other );
	void CopyRotation( const CTransform& other );
	void CopyScale( const CTransform& other );

	vec3_t RotateVector( const vec3_t& vector ) const;

	bool MatchesNoScale( const CTransform& otherTransform ) const;
	bool Matches( const CTransform& otherTransform ) const;

	void SetIdentity();
	void SetLocation( const vec3_t& newLocation );
	void SetRotation( const quat_t& newRotation );
	void SetScale( const vec3_t& newScale );

	const matrix_t& AsMatrix() const;
	void			AsMatrix( matrix_t& destMatrix ) const;
	vec3_t			GetUnitAxis( axis_t axis ) const;
	vec3_t			GetLocation() const;
	quat_t			GetRotation() const;
	vec3_t			GetScale() const;

	CTransform operator+( const CTransform& other ) const;
	CTransform operator-( const CTransform& other ) const;

protected:
	mutable bool	 bDirtyMatrix;
	vec3_t			 translation;
	quat_t			 rotation;
	vec3_t			 scale;
	mutable matrix_t matrix;
};

#include "tier1/math/transform.inl"
