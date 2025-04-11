{
	// Shader list name
	"name":								"stdshaders",
	
	// Output path for generated C++ classes
	"cppgen_output": 					"generated/",
	
	// Include directories
	"include": [
		"../",
		"../../public/materialsystem/"
	],
	
	// Output shader cache directory
	"output":							"//CORE/",
	
	"shaders": [
		// Shader for example
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
	
		// Unlit Generic
		{
			"source": 		"unlitgeneric_vs.hlsl",
			"main_func": 	"MainVS",
			"type":			"vertex"
		},
		{
			"source": 		"unlitgeneric_ps.hlsl",
			"main_func": 	"MainPS",
			"type":			"pixel"
		}
	]
}