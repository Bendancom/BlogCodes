add_rules("mode.debug","mode.release")

target("multi_index")
    set_languages("c++23")
    add_files("multi_index.cppm","test.cpp")
    set_kind("binary")