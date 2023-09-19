add_rules("mode.debug", "mode.release")

includes("../../vendor/imgui")
includes("../../vendor/GLFW")

if is_os('windows') then
target("Walnut-Networking")
    set_languages("c++20")
    set_targetdir("build")
    set_kind("static")
    add_defines("WL_PLATFORM_WINDOWS")
    add_files(
       "Source/**.cpp",
       "Platform/Windows/**.cpp"
       )
    
   add_includedirs("Platform/Windows")
   add_includedirs
   ( 
      "Source",

      "vendor/GameNetworkingSockets/include",

      --------------------------------------------------------
      -- Walnut includes
      -- Assumes we are in Walnut-Modules/Walnut-Networking
      "../../Walnut/Source",

      "../../vendor/imgui",
      "../../vendor/glfw/include",
      "../../vendor/glm",
      "../../vendor/spdlog/include"
      --------------------------------------------------------
   ) 
   -- windows links
   add_links { "Ws2_32.lib" }
   
else
target("Walnut-Networking")
    set_languages("c++20")
    set_targetdir("build")
    set_kind("static")
    add_files(
       "Source/**.cpp",
       "Platform/Linux/Walnut/Networking/**.cpp"
       )
    
   add_includedirs("Platform/Linux")
   add_includedirs
   ( 
      "Source",

      "vendor/GameNetworkingSockets/include",

      --------------------------------------------------------
      -- Walnut includes
      -- Assumes we are in Walnut-Modules/Walnut-Networking
      "../../Walnut/Source",

      "../../vendor/imgui",
      "../../vendor/GLFW/include",
      "../../vendor/glm",
      "../../vendor/spdlog/include"
      --------------------------------------------------------
   ) 
   
end