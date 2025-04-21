-- Output directory for Flex
flexOutputDir       = intermediateDir .. "/generated/flex/%{prj.name}/"

rule "flex"
    display         "Flex scripts compiler"
    fileExtension   ".flex"
    location( intermediateDir )

	propertyDefinition {
		name    = "AdditionalArguents",
		kind    = "string",
		value   = "",
	}

	buildcommands {
        'SET PATH=' .. root .. '/devtools/flex/;%PATH%',
        'win_flex.exe [AdditionalArguents] -o"' .. flexOutputDir .. '%{file.basename}_flex.cpp" "%{file.abspath}"'
    }

    buildoutputs {
        flexOutputDir .. '%{file.basename}_flex.cpp'
    }