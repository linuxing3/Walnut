--- @diagnostic disable:undefined-global
add_rules('mode.debug', 'mode.release')

includes('../vendor/imgui')
includes('../vendor/GLFW')

if is_os('windows') then
    target('Walnut')
    set_languages('c++20')
    set_kind('static')
    add_files('Source/**.cpp', 'Platform/GUI/**.cpp')
    add_includedirs(path.join(os.getenv('VULKAN_SDK'), 'Include'))
    add_includedirs('Source', 'Platform/GUI')
    add_includedirs('../vendor/glm')
    add_includedirs('../vendor/stb_image')
    add_includedirs('../vendor/imgui')
    add_includedirs('../vendor/GLFW/include')
    add_includedirs('../vendor/spdlog/include')
    -- windows links
    add_deps('Imgui', 'GLFW')
    add_linkdirs(path.join(os.getenv('VULKAN_SDK'), 'Lib'))
    add_links('vulkan-1')
else
    target('Walnut')
    set_languages('c++20')
    set_kind('static')
    add_files('Source/**.cpp', 'Platform/GUI/**.cpp')
    add_includedirs(path.join(os.getenv('VULKAN_SDK'), 'include'))
    add_includedirs('Source', 'Platform/GUI')
    add_includedirs('../vendor/glm')
    add_includedirs('../vendor/stb_image')
    add_includedirs('../vendor/imgui')
    add_includedirs('../vendor/GLFW/include')
    add_includedirs('../vendor/spdlog/include')
    -- windows links
    add_deps('Imgui', 'GLFW')
    add_linkdirs(path.join(os.getenv('VULKAN_SDK'), 'lib'))
    add_links('vulkan')
end
