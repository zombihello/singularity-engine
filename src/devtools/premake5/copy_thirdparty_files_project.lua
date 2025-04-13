project "copy_thirdparty_files"
    kind        "Makefile"
    location( intermediateDir )

    buildcommands {
        "{CHDIR} %{root}/devtools/",
        "python copy_thirdparty_files.py %{cfg.platform} %{cfg.buildcfg} -g %{game} -t"
    }