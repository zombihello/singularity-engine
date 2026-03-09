#include "utils/interfaces/interfaces.h"
#include "tier0/iprofiler.h"
#include "tier1/keyvalues.h"
#include "filesystem/ifilesystem.h"
#include "utils/smdldoc/smdl_source_doc.h"

// Table for convert text to axisUp_t
static const char* s_pAxisUpNames[] = {
	"+x",  // AXIS_UP_PLUS_X
	"-x",  // AXIS_UP_MINUS_X
	"+y",  // AXIS_UP_PLUS_Y
	"-y",  // AXIS_UP_MINUS_Y
	"+z",  // AXIS_UP_PLUS_Z
	"-z"   // AXIS_UP_MINUS_Z
};
static_assert( ARRAYSIZE( s_pAxisUpNames ) == AXIS_UP_NUM, "Array size 's_pAxisUpNames' must be equal to AXIS_UP_NUM" );

/*
==================
ConvTextToAxisUp
==================
*/
static bool ConvTextToAxisUp( const char* pText, axisUp_t& axisUp )
{
	for ( uint32 index = 0; index < ARRAYSIZE( s_pAxisUpNames ); ++index )
	{
		if ( !S_Stricmp( pText, s_pAxisUpNames[index] ) )
		{
			axisUp = (axisUp_t)index;
			return true;
		}
	}

	axisUp = AXIS_UP_DEFAULT;
	return false;
}

/*
==================
ConvAxisUpToText
==================
*/
static const char* ConvAxisUpToText( axisUp_t axisUp )
{
	return s_pAxisUpNames[axisUp];
}

/*
==================
CSMDLSourceModelDoc::CSMDLSourceModelDoc
==================
*/
CSMDLSourceModelDoc::CSMDLSourceModelDoc()
	: bCombineModels( false )
	, axisUp( AXIS_UP_DEFAULT )
	, materialsDir( "materials/" )
{
}

/*
==================
CSMDLSourceModelDoc::LoadFromFile
==================
*/
bool CSMDLSourceModelDoc::LoadFromFile( const char* pPath )
{
	// Load key values file
	PROFILER_SCOPE_FUNC();
	CKeyValues keyValues( "smdl" );
	if ( !keyValues.LoadFromFile( pPath ) )
	{
		return false;
	}
	Clear();

	// Get combine models
	bCombineModels = keyValues.GetBool( "combine_models" );

	// Get axis up
	const char* pStringValue = keyValues.GetString( "axis_up", ConvAxisUpToText( AXIS_UP_DEFAULT ) );
	if ( !ConvTextToAxisUp( pStringValue, axisUp ) )
	{
		Error( "SMDLDoc: Invalid SMDL, unknown axis up '%s' (file: '%s')", pStringValue, pPath );
		return false;
	}

	// Get source path
	bool bGotDefaultValue = false;
	srcPath				  = keyValues.GetString( "source", "", NULL, &bGotDefaultValue );
	if ( bGotDefaultValue )
	{
		Error( "SMDLDoc: Invalid SMDL, not found required field 'source' (file: '%s')", pPath );
		return false;
	}
	if ( srcPath.empty() )
	{
		Error( "SMDLDoc: Invalid SMDL, a source file can't be empty (file: '%s')", pPath );
		return false;
	}

	// Get materials directory
	materialsDir = keyValues.GetString( "materials_dir", "materials/" );

	// Get renamed materials
	CKeyValues* pRenamedMaterials = keyValues.FindKey( "rename_materials" );
	if ( pRenamedMaterials )
	{
		for ( CKeyValuesSubKeysIterator it( pRenamedMaterials ); it; ++it )
		{
			const char* pName = it->GetName();
			pStringValue	  = it->GetString( NULL );
			if ( !pName || !pName[0] )
			{
				Error( "SMDLDoc: Invalid SMDL, an original material name can't be empty (file: '%s')", pPath );
				return false;
			}
			if ( !pStringValue || !pStringValue[0] )
			{
				Error( "SMDLDoc: Invalid SMDL, a new material name can't be empty (file: '%s')", pPath );
				return false;
			}

			renamedMaterialsDict[pName] = pStringValue;
		}
	}

	// We are done
	return true;
}

/*
==================
CSMDLSourceModelDoc::SaveFile
==================
*/
bool CSMDLSourceModelDoc::SaveFile( const char* pPath )
{
	// Create key values
	PROFILER_SCOPE_FUNC();
	CKeyValues keyValues( "smdl" );
	keyValues.SetBool( "combine_models", bCombineModels );
	keyValues.SetString( "axis_up", ConvAxisUpToText( axisUp ) );
	keyValues.SetString( "source", srcPath.c_str() );
	keyValues.SetString( "materials_dir", materialsDir.c_str() );
	if ( !renamedMaterialsDict.empty() )
	{
		CKeyValues* pRenamedMaterials = new CKeyValues( "rename_materials", &keyValues );
		for ( auto it = renamedMaterialsDict.begin(), itEnd = renamedMaterialsDict.end(); it != itEnd; ++it )
		{
			pRenamedMaterials->SetString( it->first.c_str(), it->second.c_str() );
		}
	}

	// Save the key values to a file
	return keyValues.SaveToFile( pPath );
}
