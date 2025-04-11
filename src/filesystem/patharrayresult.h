/**
 * @file
 * @addtogroup filesystem filesystem
 */

#ifndef PATHARRAYRESULT_H
#define PATHARRAYRESULT_H

#include <vector>
#include <string>

#include "filesystem/ifilesystem.h"

/**
 * @ingroup filesystem
 * @brief Path array result
 * 
 * This is simple array who contains path elements
 */
class CPathArrayResult : public TRefCounted<IPathArrayResult>
{
public:
    /**
     * @brief Constructor
     * @param pathArray     Array with paths
     */
    CPathArrayResult( const std::vector<std::string>& pathArray );

    /**
     * @brief Get path by index
     *
     * @param index     Item index
     * @return Return path at index
     */
    virtual const achar* GetPath( uint32 index ) const override;

    /**
     * @brief Get path count in array
     * @return Return path count in array
     */
    virtual uint32 GetNum() const override;

    /**
     * @brief Is empty array
     * @return Return TRUE if array is empty, otherwise returns FALSE
     */
    virtual bool IsEmpty() const override;

private:
	std::vector<std::string>	pathArray;	/**< Array with paths */
};

#endif // !PATHARRAYRESULT_H