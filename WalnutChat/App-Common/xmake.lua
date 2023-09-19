includes('../../Walnut')

if is_os('windows') then
    target('App-Common')
    set_kind('static')
    set_languages('c++20')
    add_defines('WL_PLATFORM_WINDOWS', 'WL_DIST')
    add_files('Source/**.cpp')
    set_targetdir('.')
    add_cxxflags('/EHsc', '/Zc:preprocessor', '/Zc:__cplusplus')
    add_includedirs('../Walnut/Source', '../Walnut/Platform/GUI')
    add_includedirs('Source', '../vendor/imgui', '../vendor/GLFW/include', '../vendor/glm', '../vendor/stb_image')
    add_includedirs(path.join(os.getenv('VULKAN_SDK'), 'Include'))
    -- use deps
    add_deps('Walnut')
else
    target('App-Common')
    set_kind('static')
    set_languages('c++20')
    add_files('Source/**.cpp')
    set_targetdir('.')
    add_includedirs('../../Walnut/Source', '../../Walnut/Platform/GUI')
    add_includedirs('Source', '../../vendor/imgui', '../../vendor/GLFW/include', '../../vendor/glm', '../../vendor/stb_image', '../../vendor/spdlog/include')
    add_includedirs(path.join(os.getenv('VULKAN_SDK'), 'include'))
    add_includedirs('/home/vagrant/workspace/cxx/AIKit_Spark/SDK/include')
    -- use deps
    add_deps('Walnut')
end
