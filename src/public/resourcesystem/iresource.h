/**
 * @file
 * @addtogroup resourcesystem resourcesystem
 */

#ifndef IRESOURCE_H
#define IRESOURCE_H

#include "stdlib/refcount.h"

// Forward declarations
class IMaterial;
class ITexture;

/**
 * @ingroup resourcesystem
 * @brief Resource types
 */
enum resourceType_t
{
	RESOURCE_TYPE_MATERIAL,		/**< Material */
	RESOURCE_TYPE_TEXTURE,		/**< Texture */
	RESOURCE_TYPE_MODEL,		/**< Model */
	RESOURCE_NUM_TYPES			/**< Resource types number */
};

/**
 * @ingroup resourcesystem
 * @brief Resource interface
 */
class IResource : public IRefCounted
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~IResource() {}

	/**
	 * @brief Get resource type
	 * @return Return resource type
	 */
	virtual resourceType_t GetType() const = 0;

	/**
	 * @brief Get resource data
	 * @return Return resource data. If the data isn't valid return the default resource or NULL if it isn't exist in the system
	 */
	virtual IRefCounted* GetData() const = 0;
};

/**
 * @ingroup resourcesystem
 * @brief Resource pointer
 */
template<class TResourceClass>
class TResourcePtr
{
public:
	/**
	 * @brief Constructor
	 */
	TResourcePtr()
	{}

	/**
	 * @brief Constructor
	 * @param pPtr		Resource pointer
	 */
	TResourcePtr( IResource* pPtr )
		: pPtr( pPtr )
	{
		Assert( !pPtr || IsValidType<TResourceClass>( pPtr->GetType() ) );
	}

	/**
	 * @brief Constructor
	 * @param copy		Resource pointer
	 */
	TResourcePtr( const TResourcePtr& copy )
		: pPtr( copy.pPtr )
	{
		Assert( !pPtr || IsValidType<TResourceClass>( pPtr->GetType() ) );
	}

	/**
	 * @brief Overloaded operator =
	 * @param pPtr		Resource pointer
	 * @return Return reference to self
	 */
	FORCEINLINE TResourcePtr& operator=( IResource* pPtr )
	{
		TResourcePtr::pPtr = pPtr;
		Assert( !pPtr || IsValidType<TResourceClass>( pPtr->GetType() ) );
		return *this;
	}

	/**
	 * @brief Overloaded operator =
	 * @param copy	Copy
	 * @return Return reference to self
	 */
	FORCEINLINE TResourcePtr& operator=( const TResourcePtr& copy )
	{
		pPtr = copy.pPtr;
		Assert( !pPtr || IsValidType<TResourceClass>( pPtr->GetType() ) );
		return *this;
	}

	/**
	 * @brief Overloaded operator ==
	 * @param right		Right operand
	 * @return Return TRUE if pointers are equal, FALSE otherwise
	 */
	FORCEINLINE bool operator==( const TResourcePtr& right ) const
	{
		return pPtr == right.pPtr;
	}

	/**
	 * @brief Overloaded operator ==
	 * @param pRight	Right operand
	 * @return Return TRUE if pointers are equal, FALSE otherwise
	 */
	FORCEINLINE bool operator==( IResource* pRight ) const
	{
		return pPtr == pRight;
	}

	/**
	 * @brief Overloaded operator !=
	 * @param right		Right operand
	 * @return Return TRUE if pointers aren't equal, FALSE otherwise
	 */
	FORCEINLINE bool operator!=( const TResourcePtr& right ) const
	{
		return pPtr != right.pPtr;
	}

	/**
	 * @brief Overloaded operator !=
	 * @param pRight	Right operand
	 * @return Return TRUE if pointers aren't equal, FALSE otherwise
	 */
	FORCEINLINE bool operator!=( IResource* pRight ) const
	{
		return pPtr != pRight;
	}

	/**
	 * @brief Overloaded cast to bool
	 * @return Return TRUE if the pointer isn't NULL, otherwise FALSE 
	 */
	FORCEINLINE operator bool() const
	{
		return !!pPtr;
	}

	/**
	 * @brief Overloaded cast to ptrint
	 * @return Return address
	 */
	FORCEINLINE operator ptrint() const
	{
		return ( ptrint )pPtr;
	}

	/**
	 * @brief Overloaded cast to uptrint
	 * @return Return address
	 */
	FORCEINLINE operator uptrint() const
	{
		return ( uptrint )pPtr;
	}

	/**
	 * @brief Overloaded operator ->
	 * @return Return pointer to the resource data
	 */
	FORCEINLINE TResourceClass* operator->() const
	{
		Assert( pPtr );
		return ( TResourceClass* )pPtr->GetData();
	}

	/**
	 * @brief Overloaded operator *
	 * @return Return reference to the resource data
	 */
	FORCEINLINE TResourceClass* operator*()
	{
		Assert( pPtr );
		return ( TResourceClass* )pPtr->GetData();
	}

	/**
	 * @brief Get a raw pointer to the resource
	 * @return Return a raw pointer to the resource
	 */
	FORCEINLINE IResource* GetPtr() const
	{
		return pPtr;
	}

private:
	/**
	 * @brief Is valid resource type for TResourceClass
	 * @param type		Resource type
	 * @return Return TRUE if resource type is valid for TResourceClass, otherwise FALSE
	 */
	template<class TClass>
	FORCEINLINE bool IsValidType( resourceType_t type ) const
	{
		static_assert( false, "Invalid TResourceClass, the class isn't corresponds to any resource type" );
		return false;
	}

	template<>
	FORCEINLINE bool IsValidType<IMaterial>( resourceType_t type ) const
	{
		return type == RESOURCE_TYPE_MATERIAL;
	}

	template<>
	FORCEINLINE bool IsValidType<ITexture>( resourceType_t type ) const
	{
		return type == RESOURCE_TYPE_TEXTURE;
	}

	TRefPtr<IResource>		pPtr;	/**< Resource pointer */
};

#endif // !IRESOURCE_H