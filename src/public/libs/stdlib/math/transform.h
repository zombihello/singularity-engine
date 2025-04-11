/**
 * @file
 * @addtogroup stdlib stdlib
 */

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "stdlib/math/math.h"
#include "core/debug.h"

/**
 * @ingroup stdlib
 * @brief Class for work with transform
 */
class CTransform
{
public:
	/**
	 * @brief Constructor with initialization to the identity transform
	 */
	FORCEINLINE CTransform() 
		: bDirtyMatrix( true )
		, translation( math_t::vectorZero )
		, rotation( math_t::quaternionZero )
		, scale( math_t::vectorOne )
		, matrix( math_t::matrixIdentity )
	{}

	/**
	 * @brief Constructor with an initial translation
	 * @param translation	The value to use for the translation component
	 */
	FORCEINLINE explicit CTransform( const vec3_t& translation )
		: bDirtyMatrix( true )
		, translation( translation )
		, rotation( math_t::quaternionZero )
		, scale( math_t::vectorOne )
		, matrix( math_t::matrixIdentity )
	{}

	/**
	 * @brief Constructor with an initial rotation
	 * @param rotation		The value to use for rotation component
	 */
	FORCEINLINE explicit CTransform( const quat_t& rotation )
		: bDirtyMatrix( true )
		, translation( math_t::vectorZero )
		, rotation( rotation )
		, scale( math_t::vectorOne )
		, matrix( math_t::matrixIdentity )
	{}

	/**
	 * @brief Constructor with all components initialized
	 * 
	 * @param rotation		The value to use for rotation component
	 * @param translation	The value to use for the translation component
	 * @param scale			The value to use for the scale component
	 */
	FORCEINLINE CTransform( const quat_t& rotation, const vec3_t& translation, const vec3_t& scale = math_t::vectorOne ) 
		: bDirtyMatrix( true )
		, translation( translation )
		, rotation( rotation )
		, scale( scale )
		, matrix( math_t::matrixIdentity )
	{}

	/**
	 * @brief Copy translation from another CTransform
	 * @param other		Another transform
	 */
	FORCEINLINE void CopyTranslation( const CTransform& other )
	{
		translation		= other.translation;
		bDirtyMatrix	= true;
	}

	/**
	 * @brief Adjusts the translation component of this transformation
	 * @param deltaTranslation		The translation to add in the following fashion: Translation += deltaTranslation
	 */
	FORCEINLINE void AddToTranslation( const vec3_t& deltaTranslation )
	{
		translation		+= deltaTranslation;
		bDirtyMatrix	= true;
	}

	/**
	 * @brief Subtract from the translation component of this transformation
	 * @param deltaTranslation		Delta translation
	 */
	FORCEINLINE void SubtractFromTranslation( const vec3_t& deltaTranslation )
	{
		translation		-= deltaTranslation;
		bDirtyMatrix	= true;
	}

	/**
	 * @brief Add another rotation to this transformation 
	 * @param deltaRotation		The rotation to add in the following fashion: Rotation = Rotation * deltaRotation
	 */
	FORCEINLINE void AddToRotation( const quat_t& deltaRotation )
	{
		rotation		= deltaRotation * rotation;
		bDirtyMatrix	= true;
	}

	/**
	 * @brief Subtract another rotation from this transformation
	 * @param deltaRotation		Delta rotation
	 */
	FORCEINLINE void SubtractFromRotation( const quat_t& deltaRotation )
	{
		rotation		= math_t::InverseQuaternion( deltaRotation ) * rotation;
		bDirtyMatrix	= true;
	}

	/**
	 * @brief Adjusts the scale component of this transformation
	 * @param deltaScale	The scale to add in the following fashion: Scale += deltaScale
	 */
	FORCEINLINE void AddToScale( const vec3_t& deltaScale )
	{
		scale			+= deltaScale;
		bDirtyMatrix	= true;
	}

	/**
	 * @brief Subtract from the scale component of this transformation
	 * @param deltaScale	Delta scale
	 */
	FORCEINLINE void SubtractFromScale( const vec3_t& deltaScale )
	{
		scale			-= deltaScale;
		bDirtyMatrix	= true;
	}

	/**
	 * @brief Copy rotation from another CTransform
	 * @param other		Another transform
	 */
	FORCEINLINE void CopyRotation( const CTransform& other )
	{
		rotation		= other.rotation;
		bDirtyMatrix	= true;
	}

	/**
	 * @brief Copy scale from another CTransform
	 * @param other		Another transform
	 */
	FORCEINLINE void CopyScale( const CTransform& other )
	{
		scale			= other.scale;
		bDirtyMatrix	= true;
	}

	/**
	 * @brief Add transform to transform
	 * @param other		Another transform
	 */
	FORCEINLINE void Add( const CTransform& other )
	{
		translation	+= other.translation;
		rotation	= other.rotation * rotation;
		scale		+= other.scale;
		bDirtyMatrix = true;
	}

	/**
	 * @brief Subtract transform from transform
	 * @param other		Another transform
	 */
	FORCEINLINE void Subtract( const CTransform& other )
	{
		translation	-= other.translation;
		rotation	= math_t::InverseQuaternion( other.rotation ) * rotation;
		scale		-= other.scale;
		bDirtyMatrix = true;
	}

	/**
	 * Rotate vector
	 * 
	 * @param vector 	Vector
	 * @return Return rotated vector
	 */
	FORCEINLINE vec3_t RotateVector( const vec3_t& vector ) const
	{
		return rotation * vector;
	}

	/**
	 * @brief Compare transform without scale
	 *
	 * @param otherTransform	Other transform
	 * @return Return true if equal with otherTransform, else returning false
	 */
	FORCEINLINE bool MatchesNoScale( const CTransform& otherTransform ) const
	{
		return translation == otherTransform.translation && rotation == otherTransform.rotation;
	}

	/**
	 * @brief Compare transform
	 *
	 * @param otherTransform	Other transform
	 * @return Return true if equal with otherTransform, else returning false
	 */
	FORCEINLINE bool Matches( const CTransform& otherTransform ) const
	{
		return MatchesNoScale( otherTransform ) && scale == otherTransform.scale;
	}

	/**
	 * Set location
	 * @param newLocation	A new location
	 */
	FORCEINLINE void SetLocation( const vec3_t& newLocation )
	{
		translation		= newLocation;
		bDirtyMatrix	= true;
	}

	/**
	 * @brief Set rotation
	 * @param newRotation	A new rotation
	 */
	FORCEINLINE void SetRotation( const quat_t& newRotation )
	{
		rotation		= newRotation;
		bDirtyMatrix	= true;
	}

	/**
	 * @brief Set scale
	 * @param newScale	A new scale
	 */
	FORCEINLINE void SetScale( const vec3_t& newScale )
	{
		scale			= newScale;
		bDirtyMatrix	= true;
	}

	/**
	 * @brief Set identity
	 */
	FORCEINLINE void SetIdentity()
	{
		translation		= math_t::vectorZero;
		rotation		= math_t::quaternionZero;
		scale			= math_t::vectorOne;
		bDirtyMatrix	= true;
	}

	/**
	 * @brief Get unit axis
	 * 
	 * @param axis	Axis
	 * @return Return unit axis
	 */
	FORCEINLINE vec3_t GetUnitAxis( axis_t axis ) const
	{
		switch ( axis )
		{
		case AXIS_X:	return RotateVector( vec3_t( 1.f, 0.f, 0.f ) );	break;
		case AXIS_Y:	return RotateVector( vec3_t( 0.f, 1.f, 0.f ) );	break;
		case AXIS_Z:	return RotateVector( vec3_t( 0.f, 0.f, 1.f ) );	break;
		default:		Assert( false ); break;
		}

		return math_t::vectorZero;
	}

	/**
	 * @brief Get location
	 * @return Return location of transform
	 */
	FORCEINLINE vec3_t GetLocation() const
	{
		return translation;
	}

	/**
	 * @brief Get rotation
	 * @return Return rotation of transform
	 */
	FORCEINLINE quat_t GetRotation() const
	{
		return rotation;
	}

	/**
	 * @brief Get scale
	 * @return Return scale of transform
	 */
	FORCEINLINE vec3_t GetScale() const
	{
		return scale;
	}

	/**
	 * @brief Convert transform to matrix
	 * @return Return matrix with location, scale and rotation
	 */
	FORCEINLINE const matrix_t& ToMatrix() const
	{
		if ( bDirtyMatrix )
		{
			ToMatrix( matrix );
			bDirtyMatrix	= false;
		}
		return matrix;
	}

	/**
	 * @brief Convert transform to matrix
	 * @param destMatrix	Output matrix with location, scale and rotation
	 */
	FORCEINLINE void ToMatrix( matrix_t& destMatrix ) const
	{
		if ( bDirtyMatrix )
		{
			matrix			= math_t::TranslateMatrix( translation ) * math_t::QuaternionToMatrix( rotation ) * math_t::ScaleMatrix( scale );
			bDirtyMatrix	= false;
		}
		destMatrix			= matrix;
	}

	/**
	 * @brief Override operator +
	 */
	FORCEINLINE CTransform operator+( const CTransform& other ) const
	{
		return CTransform( other.rotation * rotation, translation + other.translation, scale + other.scale );
	}

	/**
	 * @brief Override operator -
	 */
	FORCEINLINE CTransform operator-( const CTransform& other ) const
	{
		return CTransform( math_t::InverseQuaternion( other.rotation ) * rotation, translation - other.translation, scale - other.scale );
	}

protected:
	mutable bool		bDirtyMatrix;	/**< Is dirty matrix */
	vec3_t				translation;	/**< Translation of this transformation */
	quat_t				rotation;		/**< Rotation of this transformation */
	vec3_t				scale;			/**< 3D scale */
	mutable matrix_t	matrix;			/**< Transformation in matrix */
};

#endif // TRANSFORM_H
