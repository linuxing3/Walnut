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
    target('Raytracing')
    set_kind('binary')
    set_languages('c++20')
    set_runtimes('MT')
    set_targetdir('.')
    add_defines('WL_PLATFORM_WINDOWS', 'WL_DIST')
    add_files('main.cpp', 'App/**.cpp')
    add_cxxflags('/EHsc', '/Zc:preprocessor', '/Zc:__cplusplus')
    add_includedirs('../Walnut/Source', '../Walnut/Platform/GUI')
    add_includedirs(
        '.',
        '../vendor/imgui',
        '../vendor/GLFW/include',
        '../vendor/glm',
        '../vendor/stb_image',
        '../vendor/spdlog/include'
    )
    add_includedirs(path.join(os.getenv('VULKAN_SDK'), 'Include'))
    add_includedirs(path.join(os.getenv('VCPKG_ROOT'), 'installed/x64-windows/include'))
    -- use deps
    add_deps('Walnut')
    add_links('fmt')
    add_linkdirs(path.join(os.getenv('VCPKG_ROOT'), 'installed/x64-windows/lib'))
else
    target('Raytracing')
    set_kind('binary')
    set_languages('c++20')
    set_runtimes('MT')
    set_targetdir('.')
    add_files('main.cpp', 'App/**.cpp')
    add_includedirs('../Walnut/Source', '../Walnut/Platform/GUI')
    add_includedirs(
        '.',
        '../vendor/imgui',
        '../vendor/GLFW/include',
        '../vendor/glm',
        '../vendor/stb_image',
        '../vendor/magic_enum/include',
        '../vendor/spdlog/include'
    )
    add_includedirs(path.join(os.getenv('VULKAN_SDK'), 'include'))
    -- use deps
    add_deps('Walnut')
    add_links('fmt')
end
