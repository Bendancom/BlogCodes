add_rules("mode.debug","mode.release")

set_languages("cxxlatest")

target("multi_index")
    add_files("multi_index.cppm","test.cpp")
    set_kind("binary")
