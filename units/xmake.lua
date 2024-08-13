add_rules("mode.debug","mode.release")

target("units")
    set_languages("c++23")
    add_files("units.cppm","test.cpp")
    set_kind("binary")