#include "pch_tier0.h"
#include "tier0/profile.h"

#if ENABLE_PROFILING
	//-----------------------------------------------------------------------------
	// Optick profiler
	//-----------------------------------------------------------------------------
	#include <optick.h>

// Table for convert profileScopeGroup_t to Optick::Category
static const Optick::Category::Type s_ProfileGroups[PROFILE_SCOPE_NUM_GROUPS] = {
	OPTICK_MAKE_CATEGORY( Optick::Filter::None, Optick::Color::DarkGray ),			 // PROFILE_SCOPE_GROUP_NONE
	OPTICK_MAKE_CATEGORY( Optick::Filter::AI, Optick::Color::Purple ),				 // PROFILE_SCOPE_GROUP_AI
	OPTICK_MAKE_CATEGORY( Optick::Filter::Animation, Optick::Color::LightSkyBlue ),	 // PROFILE_SCOPE_GROUP_ANIMATION
	OPTICK_MAKE_CATEGORY( Optick::Filter::Audio, Optick::Color::HotPink ),			 // PROFILE_SCOPE_GROUP_AUDIO
	OPTICK_MAKE_CATEGORY( Optick::Filter::Debug, Optick::Color::Black ),			 // PROFILE_SCOPE_GROUP_DEBUG
	OPTICK_MAKE_CATEGORY( Optick::Filter::Camera, Optick::Color::Black ),			 // PROFILE_SCOPE_GROUP_CAMERA
	OPTICK_MAKE_CATEGORY( Optick::Filter::Cloth, Optick::Color::DarkGreen ),		 // PROFILE_SCOPE_GROUP_CLOTH
	OPTICK_MAKE_CATEGORY( Optick::Filter::GameLogic, Optick::Color::RoyalBlue ),	 // PROFILE_SCOPE_GROUP_GAMELOGIC
	OPTICK_MAKE_CATEGORY( Optick::Filter::Input, Optick::Color::Ivory ),			 // PROFILE_SCOPE_GROUP_INPUT
	OPTICK_MAKE_CATEGORY( Optick::Filter::Navigation, Optick::Color::Magenta ),		 // PROFILE_SCOPE_GROUP_NAVIGATION
	OPTICK_MAKE_CATEGORY( Optick::Filter::Network, Optick::Color::Olive ),			 // PROFILE_SCOPE_GROUP_NETWORK
	OPTICK_MAKE_CATEGORY( Optick::Filter::Physics, Optick::Color::LawnGreen ),		 // PROFILE_SCOPE_GROUP_PHYSICS
	OPTICK_MAKE_CATEGORY( Optick::Filter::Rendering, Optick::Color::BurlyWood ),	 // PROFILE_SCOPE_GROUP_RENDERING
	OPTICK_MAKE_CATEGORY( Optick::Filter::Scene, Optick::Color::RoyalBlue ),		 // PROFILE_SCOPE_GROUP_SCENE
	OPTICK_MAKE_CATEGORY( Optick::Filter::Script, Optick::Color::Plum ),			 // PROFILE_SCOPE_GROUP_SCRIPT
	OPTICK_MAKE_CATEGORY( Optick::Filter::Streaming, Optick::Color::Gold ),			 // PROFILE_SCOPE_GROUP_STREAMING
	OPTICK_MAKE_CATEGORY( Optick::Filter::UI, Optick::Color::PaleTurquoise ),		 // PROFILE_SCOPE_GROUP_UI
	OPTICK_MAKE_CATEGORY( Optick::Filter::VFX, Optick::Color::SaddleBrown ),		 // PROFILE_SCOPE_GROUP_VFX
	OPTICK_MAKE_CATEGORY( Optick::Filter::Visibility, Optick::Color::Snow ),		 // PROFILE_SCOPE_GROUP_VISIBILITY
	OPTICK_MAKE_CATEGORY( Optick::Filter::Wait, Optick::Color::Tomato ),			 // PROFILE_SCOPE_GROUP_WAIT
	OPTICK_MAKE_CATEGORY( Optick::Filter::IO, Optick::Color::Khaki ),				 // PROFILE_SCOPE_GROUP_IO
};

class COptickProfile : public IProfile
{
public:
	COptickProfile()
		: bIsRecord( false )
	{
	}

	// IProfile interface
	virtual void Init() override;
	virtual void Shutdown() override;

	virtual profileDescription_t CreateDescription( const char* pFunctionName, const char* pFileName, uint32 fileLine, const char* pScopeName = NULL, profileScopeGroup_t scopeGroup = PROFILE_SCOPE_GROUP_NONE ) override;
	virtual profileDescription_t CreateDescription( const char* pFunctionName, const char* pFileName, uint32 fileLine, profileScopeGroup_t scopeGroup ) override;

	virtual void StartThreadScope( const char* pThreadName ) override;
	virtual void StopThreadScope() override;

	virtual profileScopeData_t BeginScope( profileDescription_t pDescription ) override;
	virtual void			   EndScope( profileScopeData_t pScopeData ) override;

	virtual uint32 NextFrame() override;

	virtual void AttachTag( profileDescription_t pDescription, float value ) override;
	virtual void AttachTag( profileDescription_t pDescription, int32 value ) override;
	virtual void AttachTag( profileDescription_t pDescription, uint32 value ) override;
	virtual void AttachTag( profileDescription_t pDescription, uint64 value ) override;
	virtual void AttachTag( profileDescription_t pDescription, const vec3_t& value ) override;
	virtual void AttachTag( profileDescription_t pDescription, const char* pValue ) override;
	virtual void AttachTag( profileDescription_t pDescription, const char* pValue, uint16 length ) override;
	virtual void AttachTag( profileDescription_t pDescription, float x, float y, float z ) override;

	virtual profileDescription_t GetFrameDescription() override;
	virtual bool				 IsRecord() const override;

private:
	static bool StateChangedCallback( Optick::State::Type optickState );

	bool bIsRecord;
};

/*
 ==================
 COptickProfile::Init
 ==================
 */
void COptickProfile::Init()
{
	Optick::SetStateChangedCallback( COptickProfile::StateChangedCallback );
}

/*
 ==================
 COptickProfile::Shutdown
 ==================
 */
void COptickProfile::Shutdown()
{
	Optick::Shutdown();
}

/*
 ==================
 COptickProfile::CreateDescription
 ==================
 */
profileDescription_t COptickProfile::CreateDescription( const char* pFunctionName, const char* pFileName, uint32 fileLine, const char* pScopeName /*= NULL*/, profileScopeGroup_t scopeGroup /*= PROFILE_SCOPE_GROUP_NONE*/ )
{
	uint8						  flags			= pScopeName ? Optick::EventDescription::IS_CUSTOM_NAME : 0;
	const Optick::Category::Type& scopeCategory = s_ProfileGroups[scopeGroup];
	return Optick::EventDescription::Create( pFunctionName, pFileName, fileLine, Optick::Category::GetColor( scopeCategory ), Optick::Category::GetMask( scopeCategory ) );
}

/*
 ==================
 COptickProfile::CreateDescription
 ==================
 */
profileDescription_t COptickProfile::CreateDescription( const char* pFunctionName, const char* pFileName, uint32 fileLine, profileScopeGroup_t scopeGroup )
{
	return CreateDescription( pFunctionName, pFileName, fileLine, NULL, scopeGroup );
}

/*
 ==================
 COptickProfile::StartThreadScope
 ==================
 */
void COptickProfile::StartThreadScope( const char* pThreadName )
{
	Optick::RegisterThread( pThreadName );
}

/*
 ==================
 COptickProfile::StopThreadScope
 ==================
 */
void COptickProfile::StopThreadScope()
{
	Optick::UnRegisterThread( false );
}

/*
 ==================
 COptickProfile::BeginScope
 ==================
 */
profileScopeData_t COptickProfile::BeginScope( profileDescription_t pDescription )
{
	Optick::EventDescription* pEventDescription = (Optick::EventDescription*)pDescription;
	return Optick::Event::Start( *pEventDescription );
}

/*
 ==================
 COptickProfile::EndScope
 ==================
 */
void COptickProfile::EndScope( profileScopeData_t pScopeData )
{
	Optick::EventData* pEventData = (Optick::EventData*)pScopeData;
	Optick::Event::Stop( *pEventData );
}

/*
 ==================
 COptickProfile::NextFrame
 ==================
 */
uint32 COptickProfile::NextFrame()
{
	Optick::EndFrame();
	Optick::Update();
	return Optick::BeginFrame();
}

/*
 ==================
 COptickProfile::AttachTag
 ==================
 */
void COptickProfile::AttachTag( profileDescription_t pDescription, float value )
{
	const Optick::EventDescription* pEventDescription = (Optick::EventDescription*)pDescription;
	Optick::Tag::Attach( *pEventDescription, value );
}

/*
 ==================
 COptickProfile::AttachTag
 ==================
 */
void COptickProfile::AttachTag( profileDescription_t pDescription, int32 value )
{
	const Optick::EventDescription* pEventDescription = (Optick::EventDescription*)pDescription;
	Optick::Tag::Attach( *pEventDescription, value );
}

/*
 ==================
 COptickProfile::AttachTag
 ==================
 */
void COptickProfile::AttachTag( profileDescription_t pDescription, uint32 value )
{
	const Optick::EventDescription* pEventDescription = (Optick::EventDescription*)pDescription;
	Optick::Tag::Attach( *pEventDescription, value );
}

/*
 ==================
 COptickProfile::AttachTag
 ==================
 */
void COptickProfile::AttachTag( profileDescription_t pDescription, uint64 value )
{
	const Optick::EventDescription* pEventDescription = (Optick::EventDescription*)pDescription;
	Optick::Tag::Attach( *pEventDescription, value );
}

/*
 ==================
 COptickProfile::AttachTag
 ==================
 */
void COptickProfile::AttachTag( profileDescription_t pDescription, const vec3_t& value )
{
	const Optick::EventDescription* pEventDescription = (Optick::EventDescription*)pDescription;
	float							rawValue[3]		  = { value.x, value.y, value.z };
	Optick::Tag::Attach( *pEventDescription, rawValue );
}

/*
 ==================
 COptickProfile::AttachTag
 ==================
 */
void COptickProfile::AttachTag( profileDescription_t pDescription, const char* pValue )
{
	const Optick::EventDescription* pEventDescription = (Optick::EventDescription*)pDescription;
	Optick::Tag::Attach( *pEventDescription, pValue );
}

/*
 ==================
 COptickProfile::AttachTag
 ==================
 */
void COptickProfile::AttachTag( profileDescription_t pDescription, const char* pValue, uint16 length )
{
	const Optick::EventDescription* pEventDescription = (Optick::EventDescription*)pDescription;
	Optick::Tag::Attach( *pEventDescription, pValue, length );
}

/*
 ==================
 COptickProfile::AttachTag
 ==================
 */
void COptickProfile::AttachTag( profileDescription_t pDescription, float x, float y, float z )
{
	vec3_t value( x, y, z );
	AttachTag( pDescription, value );
}

/*
 ==================
 COptickProfile::GetFrameDescription
 ==================
 */
profileDescription_t COptickProfile::GetFrameDescription()
{
	return (profileDescription_t)Optick::GetFrameDescription();
}

/*
 ==================
 COptickProfile::IsRecord
 ==================
 */
bool COptickProfile::IsRecord() const
{
	return bIsRecord;
}

/*
 ==================
 COptickProfile::StateChangedCallback
 ==================
 */
bool COptickProfile::StateChangedCallback( Optick::State::Type optickState )
{
	COptickProfile* pOptickProfile = (COptickProfile*)Profile();
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
#else
class CNullProfile : public IProfile
{
public:
	// IProfile interface
	virtual void				 Init() override {}
	virtual void				 Shutdown() override {}
	virtual profileDescription_t CreateDescription( const char* pFunctionName, const char* pFileName, uint32 fileLine, const char* pScopeName = NULL, profileScopeGroup_t scopeGroup = PROFILE_SCOPE_GROUP_NONE ) override
	{
		return NULL;
	}
	virtual profileDescription_t CreateDescription( const char* pFunctionName, const char* pFileName, uint32 fileLine, profileScopeGroup_t scopeGroup ) override
	{
		return NULL;
	}
	virtual void				 StartThreadScope( const char* pThreadName ) override {}
	virtual void				 StopThreadScope() override {}
	virtual profileScopeData_t	 BeginScope( profileDescription_t pDescription ) override { return NULL; }
	virtual void				 EndScope( profileScopeData_t pScopeData ) override {}
	virtual uint32				 NextFrame() override { return 0; }
	virtual void				 AttachTag( profileDescription_t pDescription, float value ) override {}
	virtual void				 AttachTag( profileDescription_t pDescription, int32 value ) override {}
	virtual void				 AttachTag( profileDescription_t pDescription, uint32 value ) override {}
	virtual void				 AttachTag( profileDescription_t pDescription, uint64 value ) override {}
	virtual void				 AttachTag( profileDescription_t pDescription, const vec3_t& value ) override {}
	virtual void				 AttachTag( profileDescription_t pDescription, const char* pValue ) override {}
	virtual void				 AttachTag( profileDescription_t pDescription, const char* pValue, uint16 length ) override {}
	virtual void				 AttachTag( profileDescription_t pDescription, float x, float y, float z ) override {}
	virtual profileDescription_t GetFrameDescription() override { return NULL; }
	virtual bool				 IsRecord() const override { return false; }
};
#endif	// ENABLE_PROFILING

/*
==================
Profile
==================
*/
IProfile* Profile()
{
#if ENABLE_PROFILING
	static COptickProfile s_OptickProfile;
	return &s_OptickProfile;
#else
	static CNullProfile s_NullProfile;
	return &s_NullProfile;
#endif	// ENABLE_PROFILING
}
