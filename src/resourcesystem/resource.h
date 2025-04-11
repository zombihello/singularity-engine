/**
 * @file
 * @addtogroup resourcesystem resourcesystem
 */

#ifndef RESOURCE_H
#define RESOURCE_H

#include "resourcesystem/iresource.h"

/**
 * @ingroup resourcesystem
 * @brief Resource
 */
class CResource : public TRefCounted<IResource>
{
public:
	/**
	 * @brief Constructor
	 * @param pPath			Resource path
	 * @param pData			Resource data
	 * @param type			Resource type
	 * @param bProcedural	Is procedural resource
	 */
	CResource( const achar* pPath, IRefCounted* pData, resourceType_t type, bool bProcedural = false );

	/**
	 * @brief Set resource data
	 * @param pData		Resource data
	 * @param type		Resource type
	 */
	FORCEINLINE void SetData( IRefCounted* pData, resourceType_t type )
	{
		CResource::type		= type;
		CResource::pData	= pData;
	}

	/**
	 * @brief Get resource type
	 * @return Return resource type. If the resource is procedural return empty string
	 */
	virtual resourceType_t GetType() const override;

	/**
	 * @brief Get resource data
	 * @return Return resource data. If the data isn't valid return the default material (or NULL if the default resource isn't exist)
	 */
	virtual IRefCounted* GetData() const override;

	/**
	 * @brief Is procedural the resource (load from a file)
	 * @return Return TRUE if the procedural resource, otherwise FALSE
	 */
	FORCEINLINE bool IsProcedural() const
	{
		return bProcedural;
	}

	/**
	 * @brief Get resource path
	 * @return Return resource path
	 */
	FORCEINLINE const achar* GetPath() const
	{
		return path.c_str();
	}

private:
	bool					bProcedural;	/**< Is procedural the resource */
	resourceType_t			type;			/**< Resource type */
	std::string				path;			/**< Resource path */
	TRefPtr<IRefCounted>	pData;			/**< Resource data */
};

#endif // !RESOURCE_H