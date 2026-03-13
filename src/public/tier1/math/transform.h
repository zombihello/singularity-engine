#pragma once
#include "tier1/math/math.h"
#include "tier1/math/axis.h"

//-----------------------------------------------------------------------------
// A transform
//-----------------------------------------------------------------------------
class CTransform
{
public:
	CTransform();
	CTransform( const vector3_t& location, const quaternion_t& rotation = g_quaternionIdentity, const vector3_t& scale = g_vector111 );
	CTransform( const quaternion_t& rotation );
	CTransform( const CTransform& copy );

	static CTransform Make( const vector3_t& location, const quaternion_t& rotation = g_quaternionIdentity, const vector3_t& scale = g_vector111 );
	static CTransform Make( const quaternion_t& rotation );

	void Translate( const vector3_t& deltaLocation );
	void Rotate( const quaternion_t& deltaRotation );
	void Scale( const vector3_t& deltaScale );

	vector3_t TranslateVector( const vector3_t& vector ) const;
	void	  TranslateVector( const vector3_t& vector, vector3_t& result ) const;
	vector3_t RotateVector( const vector3_t& vector ) const;
	void	  RotateVector( const vector3_t& vector, vector3_t& result ) const;
	vector3_t ScaleVector( const vector3_t& vector ) const;
	void	  ScaleVector( const vector3_t& vector, vector3_t& result ) const;
	vector3_t TransformVector( const vector3_t& vector ) const;
	void	  TransformVector( const vector3_t& vector, vector3_t& result ) const;

	void Identity();
	void Inverse();
	void SetLocation( const vector3_t& location );
	void SetRotation( const quaternion_t& rotation );
	void SetScale( const vector3_t& scale );

	matrix4x4_t ToMatrix() const;
	void		ToMatrix( matrix4x4_t& result ) const;

	vector3_t			GetUnitAxis( axis_t axis ) const;
	void				GetUnitAxis( axis_t axis, vector3_t& result ) const;
	CTransform			GetInverse() const;
	void				GetInverse( CTransform& result ) const;
	const vector3_t&	GetLocation() const;
	const quaternion_t& GetRotation() const;
	const vector3_t&	GetScale() const;

	CTransform& operator=( const CTransform& right );
	vector3_t	operator*( const vector3_t& right ) const;	// This will be equivalent to a transform vector
	CTransform	operator*( const CTransform& right ) const;
	bool		operator==( const CTransform& right ) const;
	bool		operator!=( const CTransform& right ) const;

private:
	quaternion_t rotation;
	vector3_t	 location;
	vector3_t	 scale;
};

#include "tier1/math/transform.inl"
