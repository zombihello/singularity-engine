#include "pch_core.h"
#include "core/profile.h"

#if ENABLE_PROFILING
#include <optick.h>

// Table for convert profileScopeGroup_t to Optick::Category
static const Optick::Category::Type		s_ProfileGroups[PROFILE_SCOPE_NUM_GROUPS] =
{
	OPTICK_MAKE_CATEGORY(	Optick::Filter::None,		Optick::Color::DarkGray		),		// PROFILE_SCOPE_GROUP_NONE
	OPTICK_MAKE_CATEGORY(	Optick::Filter::AI,			Optick::Color::Purple		),		// PROFILE_SCOPE_GROUP_AI
	OPTICK_MAKE_CATEGORY(	Optick::Filter::Animation,	Optick::Color::LightSkyBlue	),		// PROFILE_SCOPE_GROUP_ANIMATION
	OPTICK_MAKE_CATEGORY(	Optick::Filter::Audio,		Optick::Color::HotPink		),		// PROFILE_SCOPE_GROUP_AUDIO
	OPTICK_MAKE_CATEGORY(	Optick::Filter::Debug,		Optick::Color::Black		),		// PROFILE_SCOPE_GROUP_DEBUG
	OPTICK_MAKE_CATEGORY(	Optick::Filter::Camera,		Optick::Color::Black		),		// PROFILE_SCOPE_GROUP_CAMERA
	OPTICK_MAKE_CATEGORY(	Optick::Filter::Cloth,		Optick::Color::DarkGreen	),		// PROFILE_SCOPE_GROUP_CLOTH
	OPTICK_MAKE_CATEGORY(	Optick::Filter::GameLogic,	Optick::Color::RoyalBlue	),		// PROFILE_SCOPE_GROUP_GAMELOGIC
	OPTICK_MAKE_CATEGORY(	Optick::Filter::Input,		Optick::Color::Ivory		),		// PROFILE_SCOPE_GROUP_INPUT
	OPTICK_MAKE_CATEGORY(	Optick::Filter::Navigation, Optick::Color::Magenta		),		// PROFILE_SCOPE_GROUP_NAVIGATION
	OPTICK_MAKE_CATEGORY(	Optick::Filter::Network,	Optick::Color::Olive		),		// PROFILE_SCOPE_GROUP_NETWORK
	OPTICK_MAKE_CATEGORY(	Optick::Filter::Physics,	Optick::Color::LawnGreen	),		// PROFILE_SCOPE_GROUP_PHYSICS
	OPTICK_MAKE_CATEGORY(	Optick::Filter::Rendering,	Optick::Color::BurlyWood	),		// PROFILE_SCOPE_GROUP_RENDERING
	OPTICK_MAKE_CATEGORY(	Optick::Filter::Scene,		Optick::Color::RoyalBlue	),		// PROFILE_SCOPE_GROUP_SCENE
	OPTICK_MAKE_CATEGORY(	Optick::Filter::Script,		Optick::Color::Plum			),		// PROFILE_SCOPE_GROUP_SCRIPT
	OPTICK_MAKE_CATEGORY(	Optick::Filter::Streaming,	Optick::Color::Gold			),		// PROFILE_SCOPE_GROUP_STREAMING
	OPTICK_MAKE_CATEGORY(	Optick::Filter::UI,			Optick::Color::PaleTurquoise),		// PROFILE_SCOPE_GROUP_UI
	OPTICK_MAKE_CATEGORY(	Optick::Filter::VFX,		Optick::Color::SaddleBrown	),		// PROFILE_SCOPE_GROUP_VFX
	OPTICK_MAKE_CATEGORY(	Optick::Filter::Visibility, Optick::Color::Snow			),		// PROFILE_SCOPE_GROUP_VISIBILITY
	OPTICK_MAKE_CATEGORY(	Optick::Filter::Wait,		Optick::Color::Tomato		),		// PROFILE_SCOPE_GROUP_WAIT
	OPTICK_MAKE_CATEGORY(	Optick::Filter::IO,			Optick::Color::Khaki		),		// PROFILE_SCOPE_GROUP_IO
};


/**
 * @ingroup core
 * @brief Optick profiler
 */
class COptickProfile : public IProfile
{
public:
	/**
	 * @brief Constructor
	 */
	COptickProfile()
		: bIsRecord( false )
	{}

	/**
	 * @brief Initialize the profiler
	 */
	virtual void Init() override
	{
		Optick::SetStateChangedCallback( COptickProfile::StateChangedCallback );
	}

	/**
	 * @brief Shutdown the profiler
	 */
	virtual void Shutdown() override
	{
		Optick::Shutdown();
	}

	/**
	 * @brief Create a profile description
	 * @param pFunctionName		Function name
	 * @param pFileName			File name
	 * @param fileLine			File line
	 * @param pScopeName		Scope name
	 * @param scopeGroup		Scope group
	 * @return Return created a new profile description
	 */
	virtual profileDescription_t CreateDescription( const achar* pFunctionName, const achar* pFileName, uint32 fileLine, const achar* pScopeName = NULL, profileScopeGroup_t scopeGroup = PROFILE_SCOPE_GROUP_NONE ) override
	{
		uint8								flags = pScopeName ? Optick::EventDescription::IS_CUSTOM_NAME : 0;
		const Optick::Category::Type& scopeCategory = s_ProfileGroups[scopeGroup];
		return Optick::EventDescription::Create( pFunctionName, pFileName, fileLine, Optick::Category::GetColor( scopeCategory ), Optick::Category::GetMask( scopeCategory ) );
	}

	/**
	 * @brief Create a profile description
	 * @param pFunctionName		Function name
	 * @param pFileName			File name
	 * @param fileLine			File line
	 * @param scopeGroup		Scope group
	 * @return Return created a new profile description
	 */
	virtual profileDescription_t CreateDescription( const achar* pFunctionName, const achar* pFileName, uint32 fileLine, profileScopeGroup_t scopeGroup ) override
	{
		return CreateDescription( pFunctionName, pFileName, fileLine, NULL, scopeGroup );
	}

	/**
	 * @brief Start profile a thread scope
	 * @param pThreadName	Thread name
	 */
	virtual void StartThreadScope( const achar* pThreadName ) override
	{
		Optick::RegisterThread( pThreadName );
	}

	/**
	 * @brief Stop profile the thread scope
	 */
	virtual void StopThreadScope() override
	{
		Optick::UnRegisterThread( false );
	}

	/**
	 * @brief Begin profile a scope
	 * @param pDescription		Profile description
	 * @return Return the scope data
	 */
	virtual profileScopeData_t BeginScope( profileDescription_t pDescription ) override
	{
		Optick::EventDescription*		pEventDescription = ( Optick::EventDescription* )pDescription;
		return Optick::Event::Start( *pEventDescription );
	}

	/**
	 * @brief End profile the scope
	 * @param pScopeData	Scope data
	 */
	virtual void EndScope( profileScopeData_t pScopeData ) override
	{
		Optick::EventData*		pEventData = ( Optick::EventData* )pScopeData;
		Optick::Event::Stop( *pEventData );
	}

	/**
	 * @brief Begin a next frame
	 * @return Return a frame number
	 */
	virtual uint32 NextFrame() override
	{
		Optick::EndFrame();
		Optick::Update();
		return Optick::BeginFrame();
	}

	/**
	 * @brief Attach a custom data-tag to the description
	 * @param pDescription		Profile description
	 * @param value				Value
	 */
	virtual void AttachTag( profileDescription_t pDescription, float value ) override
	{
		const Optick::EventDescription*		pEventDescription = ( Optick::EventDescription* )pDescription;
		Optick::Tag::Attach( *pEventDescription, value );
	}

	/**
	 * @brief Attach a custom data-tag to the description
	 * @param pDescription		Profile description
	 * @param value				Value
	 */
	virtual void AttachTag( profileDescription_t pDescription, int32 value ) override
	{
		const Optick::EventDescription*		pEventDescription = ( Optick::EventDescription* )pDescription;
		Optick::Tag::Attach( *pEventDescription, value );
	}

	/**
	 * @brief Attach a custom data-tag to the description
	 * @param pDescription		Profile description
	 * @param value				Value
	 */
	virtual void AttachTag( profileDescription_t pDescription, uint32 value ) override
	{
		const Optick::EventDescription*		pEventDescription = ( Optick::EventDescription* )pDescription;
		Optick::Tag::Attach( *pEventDescription, value );
	}

	/**
	 * @brief Attach a custom data-tag to the description
	 * @param pDescription		Profile description
	 * @param value				Value
	 */
	virtual void AttachTag( profileDescription_t pDescription, uint64 value ) override
	{
		const Optick::EventDescription*		pEventDescription = ( Optick::EventDescription* )pDescription;
		Optick::Tag::Attach( *pEventDescription, value );
	}

	/**
	 * @brief Attach a custom data-tag to the description
	 * @param pDescription		Profile description
	 * @param value				Value
	 */
	virtual void AttachTag( profileDescription_t pDescription, const vec3_t& value ) override
	{
		const Optick::EventDescription*		pEventDescription	= ( Optick::EventDescription* )pDescription;
		float								rawValue[3]			= { value.x, value.y, value.z };
		Optick::Tag::Attach( *pEventDescription, rawValue );
	}

	/**
	 * @brief Attach a custom data-tag to the description
	 * @param pDescription		Profile description
	 * @param value				Value
	 */
	virtual void AttachTag( profileDescription_t pDescription, const achar* pValue ) override
	{
		const Optick::EventDescription*		pEventDescription = ( Optick::EventDescription* )pDescription;
		Optick::Tag::Attach( *pEventDescription, pValue );
	}

	/**
	 * @brief Attach a custom data-tag to the description
	 * @param pDescription		Profile description
	 * @param value				Value
	 * @param length			Length
	 */
	virtual void AttachTag( profileDescription_t pDescription, const achar* pValue, uint16 length ) override
	{
		const Optick::EventDescription*		pEventDescription = ( Optick::EventDescription* )pDescription;
		Optick::Tag::Attach( *pEventDescription, pValue, length );
	}

	/**
	 * @brief Attach a custom data-tag to the description
	 * @param pDescription		Profile description
	 * @param x					X
	 * @param y					Y
	 * @param z					Z
	 */
	virtual void AttachTag( profileDescription_t pDescription, float x, float y, float z ) override
	{
		vec3_t		value( x, y, z );
		AttachTag( pDescription, value );
	}

	/**
	 * @brief Get the frame description
	 * @return Return the frame description
	 */
	virtual profileDescription_t GetFrameDescription() override
	{
		return ( profileDescription_t )Optick::GetFrameDescription();
	}

	/**
	 * @brief Does the profiler record the capture
	 * @return Return TRUE if now the profiler record a capture, otherwise FALSE
	 */
	virtual bool IsRecord() const override
	{
		return bIsRecord;
	}

private:
	/**
	 * @brief Optick state changed callback
	 * @param optickState	New Optick state
	 */
	static bool StateChangedCallback( Optick::State::Type optickState )
	{
		COptickProfile*		pOptickProfile = ( COptickProfile* )Profile();
		switch ( optickState )
		{
		case Optick::State::START_CAPTURE:
			pOptickProfile->bIsRecord = true;
			break;

		case Optick::State::DUMP_CAPTURE:


		case Optick::State::CANCEL_CAPTURE:
			pOptickProfile->bIsRecord = false;
			break;
		}
		return true;
	}

	bool	bIsRecord;		/**< Does the profiler record the capture */
};
#else
/**
 * @ingroup core
 * @brief Null profiler
 */
class CNullProfile : public IProfile
{
public:
	/**
	 * @brief Initialize the profiler
	 */
	virtual void Init() override 
	{}

	/**
	 * @brief Shutdown the profiler
	 */
	virtual void Shutdown() override 
	{}

	/**
	 * @brief Create a profile description
	 * @param pFunctionName		Function name
	 * @param pFileName			File name
	 * @param fileLine			File line
	 * @param pScopeName		Scope name
	 * @param scopeGroup		Scope group
	 * @return Return created a new profile description
	 */
	virtual profileDescription_t CreateDescription( const achar* pFunctionName, const achar* pFileName, uint32 fileLine, const achar* pScopeName = NULL, profileScopeGroup_t scopeGroup = PROFILE_SCOPE_GROUP_NONE ) override
	{
		return NULL;
	}

	/**
	 * @brief Create a profile description
	 * @param pFunctionName		Function name
	 * @param pFileName			File name
	 * @param fileLine			File line
	 * @param scopeGroup		Scope group
	 * @return Return created a new profile description
	 */
	virtual profileDescription_t CreateDescription( const achar* pFunctionName, const achar* pFileName, uint32 fileLine, profileScopeGroup_t scopeGroup ) override
	{
		return NULL;
	}

	/**
	 * @brief Start profile a thread scope
	 * @param pThreadName	Thread name
	 */
	virtual void StartThreadScope( const achar* pThreadName ) override
	{}

	/**
	 * @brief Stop profile the thread scope
	 */
	virtual void StopThreadScope() override
	{}

	/**
	 * @brief Begin profile a scope
	 * @param pDescription		Profile description
	 * @return Return the scope data
	 */
	virtual profileScopeData_t BeginScope( profileDescription_t pDescription ) override
	{
		return NULL;
	}

	/**
	 * @brief End profile the scope
	 * @param pScopeData	Scope data
	 */
	virtual void EndScope( profileScopeData_t pScopeData ) override
	{}

	/**
	 * @brief Begin a next frame
	 * @return Return a frame number
	 */
	virtual uint32 NextFrame() override
	{
		return 0;
	}

	/**
	 * @brief Attach a custom data-tag to the description
	 * @param pDescription		Profile description
	 * @param value				Value
	 */
	virtual void AttachTag( profileDescription_t pDescription, float value ) override
	{}

	/**
	 * @brief Attach a custom data-tag to the description
	 * @param pDescription		Profile description
	 * @param value				Value
	 */
	virtual void AttachTag( profileDescription_t pDescription, int32 value ) override
	{}

	/**
	 * @brief Attach a custom data-tag to the description
	 * @param pDescription		Profile description
	 * @param value				Value
	 */
	virtual void AttachTag( profileDescription_t pDescription, uint32 value ) override
	{}

	/**
	 * @brief Attach a custom data-tag to the description
	 * @param pDescription		Profile description
	 * @param value				Value
	 */
	virtual void AttachTag( profileDescription_t pDescription, uint64 value ) override
	{}

	/**
	 * @brief Attach a custom data-tag to the description
	 * @param pDescription		Profile description
	 * @param value				Value
	 */
	virtual void AttachTag( profileDescription_t pDescription, const vec3_t& value ) override
	{}

	/**
	 * @brief Attach a custom data-tag to the description
	 * @param pDescription		Profile description
	 * @param value				Value
	 */
	virtual void AttachTag( profileDescription_t pDescription, const achar* pValue ) override
	{}

	/**
	 * @brief Attach a custom data-tag to the description
	 * @param pDescription		Profile description
	 * @param value				Value
	 * @param length			Length
	 */
	virtual void AttachTag( profileDescription_t pDescription, const achar* pValue, uint16 length ) override
	{}

	/**
	 * @brief Attach a custom data-tag to the description
	 * @param pDescription		Profile description
	 * @param x					X
	 * @param y					Y
	 * @param z					Z
	 */
	virtual void AttachTag( profileDescription_t pDescription, float x, float y, float z ) override
	{}

	/**
	 * @brief Get the frame description
	 * @return Return the frame description
	 */
	virtual profileDescription_t GetFrameDescription() override
	{
		return NULL;
	}

	/**
	 * @brief Does the profiler record the capture
	 * @return Return TRUE if now the profiler record a capture, otherwise FALSE
	 */
	virtual bool IsRecord() const override
	{
		return false;
	}
};
#endif // ENABLE_PROFILING


/*
==================
Profile
==================
*/
IProfile* Profile()
{
#if ENABLE_PROFILING
	static COptickProfile	s_OptickProfile;
	return &s_OptickProfile;
#else
	static CNullProfile		s_NullProfile;
	return &s_NullProfile;
#endif // ENABLE_PROFILING
}