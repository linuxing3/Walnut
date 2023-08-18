-- build deps
add_rules('mode.debug')
add_rules('mode.release')

if is_mode('debug') then
    set_symbols('debug')
    set_optimize('none')
end

if is_mode('release') then
    set_symbols('hidden')
    set_optimize('fastest')
    set_strip('all')
end

includes('../Walnut')

if is_os('windows') then
    target('Genesis')
    set_kind('binary')
    set_languages('c++20')
    add_defines('WL_PLATFORM_WINDOWS', 'WL_DIST')
    set_targetdir('.')
    add_files('src/**.cpp')
    add_cxxflags('/EHsc', '/Zc:preprocessor', '/Zc:__cplusplus')
    add_includedirs('../Walnut/Source', '../Walnut/Platform/GUI')
    add_includedirs('src', '../vendor/imgui', '../vendor/GLFW/include', '../vendor/glm', '../vendor/stb_image')
    add_includedirs('../vendor/vulkan/include')
    -- use deps
    add_deps('Walnut')
    after_build(function(target)
        os.cp('../Assets/images/*.png', target:targetdir())
        os.cp('../Assets/images/*.jpg', target:targetdir())
    end)
else
    target('Genesis')
    set_kind('binary')
    set_languages('c++20')
    set_targetdir('.')
    add_files('src/**.cpp')
    add_includedirs('../Walnut/Source', '../Walnut/Platform/GUI')
    add_includedirs('src', '../vendor/imgui', '../vendor/GLFW/include', '../vendor/glm', '../vendor/stb_image')
    add_includedirs(path.join(os.getenv('VULKAN_SDK'), 'include'))
    -- use deps
    add_deps('Walnut')
    after_build(function(target)
        os.cp('../Assets/images/*.png', target:targetdir())
        os.cp('../Assets/images/*.jpg', target:targetdir())
    end)
end
