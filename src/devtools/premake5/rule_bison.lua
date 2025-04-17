-- Output directory for Bison
bisonOutputDir       = intermediateDir .. "/generated/%{prj.name}/"

rule "bison"
    display         "Bison scripts compiler"
    fileExtension   ".bison"
    location( intermediateDir )

	propertyDefinition {
		name    = "AdditionalArguents",
		kind    = "string",
		value   = "",
	}

	buildcommands {
        'SET PATH=' .. root .. '/devtools/bison/bin/;%PATH%',
        'bison.exe [AdditionalArguents] --defines=' .. bisonOutputDir .. '"%{file.basename}_bison.cpp.h" -o' .. bisonOutputDir .. '"%{file.basename}_bison.cpp" "%{file.abspath}"'
    }

    buildoutputs {
        bisonOutputDir .. '%{file.basename}_bison.cpp.h', 
        bisonOutputDir .. '%{file.basename}_bison.cpp'
    }