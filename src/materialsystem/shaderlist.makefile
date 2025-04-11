{
	// Shader list name
	"name":								"default",
	
	// Output path for generated C++ classes
	"cppgen_output": 					"generated/",
	
	// Include directories
	"include": [
		"./",
		"../public/materialsystem/"
	],
	
	// Output shader cache directory
	"output":							"//CORE/",
	
	"shaders": [
		// Wireframe
		{
			"source": 		"wireframe_vs.hlsl",
			"main_func": 	"MainVS",
			"type":			"vertex"
		},
		{
			"source": 		"wireframe_ps.hlsl",
			"main_func": 	"MainPS",
			"type":			"pixel"
		}
	]
}