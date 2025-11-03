-- Get all cpp files in examples directory
local example_files = os.files("*.cpp")

-- Create executable target for each example
for _, filepath in ipairs(example_files) do
    local filename = path.basename(filepath)
    local targetname = path.filename(filepath)
    
    target(targetname)
        set_kind("binary")
        add_files(filepath)
        add_deps("nr")
        set_policy("build.c++.modules", true)
        set_targetdir("$(projectdir)/bin/examples")
        after_build(function (target)
            -- Ensure bin/examples directory exists
            os.mkdir("$(projectdir)/bin/examples")
        end)
    target_end()
end
