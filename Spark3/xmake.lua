--- @diagnostic disable:undefined-global
--- @diagnostic disable:undefined-field

includes('../Walnut')

if is_os('windows') then
    target('spark3')
    set_kind('binary')
    set_languages('c++20')
    add_defines('WL_PLATFORM_WINDOWS', 'WL_DIST')
    add_files('src/**.cpp')
    set_targetdir('.')
    add_cxxflags('/EHsc', '/Zc:preprocessor', '/Zc:__cplusplus')
    add_includedirs('../Walnut/Source', '../Walnut/Platform/GUI')
    add_includedirs('src', '../vendor/imgui', '../vendor/GLFW/include', '../vendor/glm', '../vendor/stb_image')
    add_includedirs(path.join(os.getenv('VULKAN_SDK'), 'Include'))
    -- use deps
    add_deps('Walnut')
    after_build(function(target)
        local imgui_ini_path = path.join(target:targetdir(), 'imgui.ini')
        if os.exists(imgui_ini_path) then
            os.cp(imgui_ini_path, imgui_ini_path .. '.' .. target:basename())
        end
        os.cp('$(scriptdir)/imgui.ini', target:targetdir())
    end)
else
    target('spark3')
    set_kind('binary')
    set_languages('c++20')
    add_files('src/**.cpp')
    set_targetdir('.')
    add_rpathdirs('./')
    add_includedirs('include')
    add_includedirs('../Walnut/Source', '../Walnut/Platform/GUI')
    add_includedirs(
        'src',
        '../vendor/imgui',
        '../vendor/GLFW/include',
        '../vendor/glm',
        '../vendor/stb_image',
        '../vendor/spdlog/include'
    )
    add_includedirs(path.join(os.getenv('VULKAN_SDK'), 'include'))

    add_includedirs('../Walnut-Modules/Walnut-Networking/vendor/GameNetworkingSockets/include/')
    add_includedirs('../Walnut-Modules/Walnut-Networking/Source')
    add_includedirs('../WalnutChat/App-Common/Source')
    -- use deps
    add_packages('protoc', 'protobuf-cpp')
    add_deps('Walnut')
    add_linkdirs('lib')
    add_links('SparkChain')
    after_build(function(target)
        os.cp('$(scriptdir)/lib/libSparkChain.so', path.join(target:targetdir(), 'libSparkChain.so'))
    end)
end
