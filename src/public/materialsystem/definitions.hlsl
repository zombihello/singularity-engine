#ifndef DEFINITIONS_HLSL
#define DEFINITIONS_HLSL 0

//-----------------------------------------------------------------------------
// Here defines undefined `#defines` to 0 
//-----------------------------------------------------------------------------
#ifndef VERTEX_SHADER
    #define VERTEX_SHADER 0
#endif // !VERTEX_SHADER

#ifndef DOMAIN_SHADER
    #define DOMAIN_SHADER 0
#endif // !DOMAIN_SHADER

#ifndef HULL_SHADER
    #define HULL_SHADER 0
#endif // !HULL_SHADER

#ifndef GEOMETRY_SHADER
    #define GEOMETRY_SHADER 0
#endif // !GEOMETRY_SHADER

#ifndef PIXEL_SHADER
    #define PIXEL_SHADER 0
#endif // !PIXEL_SHADER

#ifndef COMPUTE_SHADER
    #define COMPUTE_SHADER 0
#endif // !COMPUTE_SHADER

#ifndef USE_VERTEXFACTORY
    #define USE_VERTEXFACTORY 0
#endif // !USE_VERTEXFACTORY

#ifndef PLATFORM_VK
    #define PLATFORM_VK 0
#endif // !PLATFORM_VK

#endif // !DEFINITIONS_HLSL