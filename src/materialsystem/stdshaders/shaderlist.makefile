// *.shader example:
// source 		<shader_source_path>.hlsl
// main_func 	<function_entry>
// type			<shader_type>		// shader types: vertex, hull, domain, geometry, pixel, compute
// flags
// {
//		<FLAG_NAME>
//		{
//			min 	<min_value>
//			max 	<max_value>
//			default	<default_value [optional]>
//		}
//		...
// }
//
// defines
// {
//		<DEFINE_NAME> <VALUE>
//		...
// }
//
// vertexfactory <factory_name>		// optional, repeatable; names must exist in the vertex factory list below
//									// NOTE: you can set 'all' to use all known vertex factories

// Include directories
include "./"
include "../../public/materialsystem/"

// Vertex factory list
vertexfactory "../../public/modelsystem/vertexfactory"

// Shaders list (path to *.shader files, relative to the file)
shader "unlitgeneric_vs"
shader "unlitgeneric_ps"