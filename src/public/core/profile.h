/**
 * @file
 * @addtogroup core core
 */

#ifndef PROFILE_H
#define PROFILE_H

#include "core/core.h"
#include "stdlib/math/math.h"

/**
 * @ingroup core
 * @brief Profiler description
 */
typedef void* profileDescription_t;

/**
 * @ingroup core
 * @brief Profiler scope data
 */
typedef void* profileScopeData_t;

/**
 * @ingroup core
 * @brief Profiler scope group
 */
enum profileScopeGroup_t
{
	PROFILE_SCOPE_GROUP_NONE,			/**< None */
	PROFILE_SCOPE_GROUP_AI,				/**< AI */
	PROFILE_SCOPE_GROUP_ANIMATION,		/**< Animation */
	PROFILE_SCOPE_GROUP_AUDIO,			/**< Audio */
	PROFILE_SCOPE_GROUP_DEBUG,			/**< Debug */
	PROFILE_SCOPE_GROUP_CAMERA,			/**< Camera */
	PROFILE_SCOPE_GROUP_CLOTH,			/**< Cloth */
	PROFILE_SCOPE_GROUP_GAMELOGIC,		/**< Game Logic */
	PROFILE_SCOPE_GROUP_INPUT,			/**< Input */
	PROFILE_SCOPE_GROUP_NAVIGATION,		/**< Navigation */
	PROFILE_SCOPE_GROUP_NETWORK,		/**< Network */
	PROFILE_SCOPE_GROUP_PHYSICS,		/**< Physics */
	PROFILE_SCOPE_GROUP_RENDERING,		/**< Rendering */
	PROFILE_SCOPE_GROUP_SCENE,			/**< Scene */
	PROFILE_SCOPE_GROUP_SCRIPT,			/**< Script */
	PROFILE_SCOPE_GROUP_STREAMING,		/**< Streaming */
	PROFILE_SCOPE_GROUP_UI,				/**< UI */
	PROFILE_SCOPE_GROUP_VFX,			/**< VFX */
	PROFILE_SCOPE_GROUP_VISIBILITY,		/**< Visibility */
	PROFILE_SCOPE_GROUP_WAIT,			/**< Wait */
	PROFILE_SCOPE_GROUP_IO,				/**< IO */
	PROFILE_SCOPE_NUM_GROUPS			/**< Profile groups number */
};

#if ENABLE_PROFILING
	/**
	 * @ingroup core
	 * @brief Initialize the profiler
	 */
	#define PROFILE_INIT()				Profile()->Init();

	/**
	 * @ingroup core
	 * @brief Shutdown the profiler
	 */
	#define PROFILE_SHUTDOWN()			Profile()->Shutdown();

	/**
	 * @ingroup core
	 * @brief Does the profiler record the capture
	 */
	#define PROFILE_IS_RECORD()			Profile()->IsRecord()

	/**
	 * @ingroup core
	 * @brief Attaches a custom data-tag to a scope
	 * Supported types: int32, uint32, uint64, vec3, string (cut to 32 characters)
	 * 
	 * @param Name		Tag name
	 * @param ...		Tag value
	 */
	#define PROFILE_TAG( Name, ... ) \
		static profileDescription_t			CONCAT( __s_pAutogenTagDescription_, __LINE__ ) = NULL; \
		if ( !CONCAT( __s_pAutogenTagDescription_, __LINE__ ) ) \
		{ \
			CONCAT( __s_pAutogenTagDescription_, __LINE__ ) = Profile()->CreateDescription( Name, __FILE__, __LINE__ ); \
		} \
		Profile()->AttachTag( CONCAT( __s_pAutogenTagDescription_, __LINE__ ), __VA_ARGS__ );

	/**
	 * @ingroup core
	 * @brief Scoped profiling event which automatically grabs current function name
	 * @param ...	Profile description arguments
	 */
	#define PROFILE_SCOPE( ... ) \
		static profileDescription_t			CONCAT( __s_pAutogenScopeDescription_, __LINE__ ) = NULL; \
		if ( !CONCAT( __s_pAutogenScopeDescription_, __LINE__ ) ) \
		{ \
			CONCAT( __s_pAutogenScopeDescription_, __LINE__ ) = Profile()->CreateDescription( FUNCSIG, __FILE__, __LINE__, ##__VA_ARGS__ ); \
		} \
		CProfileScope	CONCAT( __s_AutogenScope, __LINE__ )( CONCAT( __s_pAutogenScopeDescription_, __LINE__ ) ); \

	/**
	 * @ingroup core
	 * @brief Macro for registration a thread
	 * @param TreadName		Thread name
	 */
	#define PROFILE_THREAD( ThreadName ) \
		CProfileThreadScope				__threadScope( ThreadName );

	/**
	 * @ingroup core
	 * @brief Profiling event for the main loop update
	 * You need to call this function in the beginning of the each new frame
	 * 
	 * @param ThreadName	Thread name
	 */
	#define PROFILE_FRAME( ThreadName ) \
		static CProfileThreadScope		__mainThreadScope( ThreadName ); \
		uint32							__frameNumber = Profile()->NextFrame(); \
		CProfileScope					CONCAT( __s_AutogenFrameScope, __LINE__ )( Profile()->GetFrameDescription() ); \
		PROFILE_TAG( "Frame", __frameNumber );
#else
	#define PROFILE_INIT()
	#define PROFILE_SHUTDOWN()
	#define PROFILE_TAG( ... )
	#define PROFILE_SCOPE( ... )
	#define PROFILE_THREAD( ... )
	#define PROFILE_FRAME( ... )
	#define PROFILE_IS_RECORD()				false
#endif // ENABLE_PROFILING

/**
 * @ingroup core
 * @brief Profiler interface
 */
class IProfile
{
public:
	/**
	 * @brief Initialize the profiler
	 */
	virtual void Init() = 0;

	/**
	 * @brief Shutdown the profiler
	 */
	virtual void Shutdown() = 0;

	/**
	 * @brief Create a profile description
	 * @param pFunctionName		Function name
	 * @param pFileName			File name
	 * @param fileLine			File line
	 * @param pScopeName		Scope name
	 * @param scopeGroup		Scope group
	 * @return Return created a new profile description
	 */
	virtual profileDescription_t CreateDescription( const achar* pFunctionName, const achar* pFileName, uint32 fileLine, const achar* pScopeName = NULL, profileScopeGroup_t scopeGroup = PROFILE_SCOPE_GROUP_NONE ) = 0;

	/**
	 * @brief Create a profile description
	 * @param pFunctionName		Function name
	 * @param pFileName			File name
	 * @param fileLine			File line
	 * @param scopeGroup		Scope group
	 * @return Return created a new profile description
	 */
	virtual profileDescription_t CreateDescription( const achar* pFunctionName, const achar* pFileName, uint32 fileLine, profileScopeGroup_t scopeGroup ) = 0;

	/**
	 * @brief Start profile a thread scope
	 * @param pThreadName	Thread name
	 */
	virtual void StartThreadScope( const achar* pThreadName ) = 0;

	/**
	 * @brief Stop profile the thread scope
	 */
	virtual void StopThreadScope() = 0;

	/**
	 * @brief Begin profile a scope
	 * @param pDescription		Profile description
	 * @return Return the scope data
	 */
	virtual profileScopeData_t BeginScope( profileDescription_t pDescription ) = 0;

	/**
	 * @brief End profile the scope
	 * @param pScopeData	Scope data
	 */
	virtual void EndScope( profileScopeData_t pScopeData ) = 0;

	/**
	 * @brief Begin a next frame
	 * @return Return a frame number
	 */
	virtual uint32 NextFrame() = 0;

	/**
	 * @brief Attach a custom data-tag to the description
	 * @param pDescription		Profile description
	 * @param value				Value
	 */
	virtual void AttachTag( profileDescription_t pDescription, float value ) = 0;

	/**
	 * @brief Attach a custom data-tag to the description
	 * @param pDescription		Profile description
	 * @param value				Value
	 */
	virtual void AttachTag( profileDescription_t pDescription, int32 value ) = 0;

	/**
	 * @brief Attach a custom data-tag to the description
	 * @param pDescription		Profile description
	 * @param value				Value
	 */
	virtual void AttachTag( profileDescription_t pDescription, uint32 value ) = 0;

	/**
	 * @brief Attach a custom data-tag to the description
	 * @param pDescription		Profile description
	 * @param value				Value
	 */
	virtual void AttachTag( profileDescription_t pDescription, uint64 value ) = 0;

	/**
	 * @brief Attach a custom data-tag to the description
	 * @param pDescription		Profile description
	 * @param value				Value
	 */
	virtual void AttachTag( profileDescription_t pDescription, const vec3_t& value ) = 0;

	/**
	 * @brief Attach a custom data-tag to the description
	 * @param pDescription		Profile description
	 * @param value				Value
	 */
	virtual void AttachTag( profileDescription_t pDescription, const achar* pValue ) = 0;

	/**
	 * @brief Attach a custom data-tag to the description
	 * @param pDescription		Profile description
	 * @param value				Value
	 * @param length			Length
	 */
	virtual void AttachTag( profileDescription_t pDescription, const achar* pValue, uint16 length ) = 0;

	/**
	 * @brief Attach a custom data-tag to the description
	 * @param pDescription		Profile description
	 * @param x					X
	 * @param y					Y
	 * @param z					Z
	 */
	virtual void AttachTag( profileDescription_t pDescription, float x, float y, float z ) = 0;

	/**
	 * @brief Get the frame description
	 * @return Return the frame description
	 */
	virtual profileDescription_t GetFrameDescription() = 0;

	/**
	 * @brief Does the profiler record the capture
	 * @return Return TRUE if now the profiler record a capture, otherwise FALSE
	 */
	virtual bool IsRecord() const = 0;
};

/**
 * @ingroup core
 * @brief Get profiler
 * @return Return profiler
 */
CORE_INTERFACE IProfile* Profile();

/**
 * @ingroup core
 * @brief Profiler a thread scope
 */
class CProfileThreadScope
{
public:
	/**
	 * @brief Constructor
	 * @param pThreadName	Thread name
	 */
	CProfileThreadScope( const achar* pThreadName )
	{
		Profile()->StartThreadScope( pThreadName );
	}

	/**
	 * @brief Destructor
	 */
	~CProfileThreadScope()
	{
		Profile()->StopThreadScope();
	}
};

/**
 * @ingroup core
 * @brief Profiler a scope
 */
class CProfileScope
{
public:
	/**
	 * @brief Constructor
	 * @param pDescription		Profile description
	 */
	CProfileScope( profileDescription_t pDescription )
		: pScopeData( Profile()->BeginScope( pDescription ) )
	{}

	/**
	 * @brief Destructor
	 */
	~CProfileScope()
	{
		if ( pScopeData )
		{
			Profile()->EndScope( pScopeData );
		}
	}

private:
	profileScopeData_t		pScopeData;		/**< Scope data */
};

#endif // !PROFILE_H