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

// Include directories
include "./"
include "../../public/materialsystem/"

// Shaders list (path to *.shader files, relative to the file)
shader "unlitgeneric_vs"
shader "unlitgeneric_ps"