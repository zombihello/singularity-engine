// *.shader example:
// {
//		"source": 		"<shader_source_path>.hlsl",
//		"main_func": 	"<function_entry>",
//		"type":			"<shader_type>"		// shader types: vertex, hull, domain, geometry, pixel, compute
//		"flags": [
//			{ "name": "<FLAG_NAME>", "min": <min_value>, "max": <max_value>, "default": <default_value [optional]> },
//			...
//		],
//		"defines": [
//			{ "name": "<DEFINE_NAME>", "value": <VALUE> },
//			...
//		]
// }

{
	// Include directories
	"include": [
		"../",
		"../../public/materialsystem/"
	],

	// Shaders list (path to *.shader files, relative to the file)
	"shaders": [
		// Unlit Generic
		"unlitgeneric_vs",
		"unlitgeneric_ps"
	]
}