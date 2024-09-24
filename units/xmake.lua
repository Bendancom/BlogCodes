add_rules("mode.debug","mode.release")

target("units")
    set_languages("c++26")
    add_files("test.cpp")
    set_kind("binary")
