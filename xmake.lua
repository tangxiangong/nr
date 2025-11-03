set_project("nr")

set_languages("cxx23")
add_rules("mode.debug", "mode.release")
if is_plat("macosx") then
    add_cxflags("-isysroot $(xcrun --show-sdk-path)", {force = true})
end

target("nr")
    set_kind("shared")
    add_files("src/*.cppm")
    set_policy("build.c++.modules", true)
    set_targetdir("lib")
    after_build(function (target)
        os.mkdir("lib")
    end)
target_end()

includes("examples")
