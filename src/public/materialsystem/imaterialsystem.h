/**
 * @file
 * @addtogroup materialsystem materialsystem
 */

#ifndef IMATERIALSYSTEM_H
#define IMATERIALSYSTEM_H

#include "stdlib/refcount.h"
#include "appframework/iappsystem.h"

// Forward declarations
class IMaterial;

/**
 * @ingroup materialsystem
 * @brief Material system interface version
 */
#define MATERIALSYSTEM_INTERFACE_VERSION "SMaterialSystem001"

/**
 * @ingroup materialsystem
 * @brief Material system interface
 */
class IMaterialSystem : public IAppSystem
{
public:
};

#endif // !IMATERIALSYSTEM_H