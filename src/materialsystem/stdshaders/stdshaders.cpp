#include "pch_stdshaders.h"
#include "shaderlib/shaderlib.h"

CShaderLib		g_ShaderLib( "stdshaders" );
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CShaderLib, IShaderLib, SHADERLIB_INTERFACE_VERSION, g_ShaderLib );