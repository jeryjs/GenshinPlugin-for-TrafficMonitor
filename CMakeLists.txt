cmake_minimum_required(VERSION 3.15)
project(GenshinPlugin LANGUAGES CXX RC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 64-bit w64devkit compiler (or default system compiler)
# Override with: cmake -DCMAKE_CXX_COMPILER=...
if(NOT CMAKE_CXX_COMPILER)
    find_program(W64DEVKIT_GXX x86_64-w64-mingw32-g++
        PATHS "C:/Users/jery/AppData/Local/Temp/w64devkit/w64devkit/bin"
              "/tmp/w64devkit/w64devkit/bin"
        NO_DEFAULT_PATH)
    if(W64DEVKIT_GXX)
        set(CMAKE_CXX_COMPILER ${W64DEVKIT_GXX})
        # Also set windres
        find_program(W64DEVKIT_WINDRES windres
            PATHS "C:/Users/jery/AppData/Local/Temp/w64devkit/w64devkit/bin"
                  "/tmp/w64devkit/w64devkit/bin"
            NO_DEFAULT_PATH)
        if(W64DEVKIT_WINDRES)
            set(CMAKE_RC_COMPILER ${W64DEVKIT_WINDRES})
        endif()
    endif()
endif()

add_library(GenshinPlugin SHARED
    PluginGenshin.cpp
    PluginGenshin.h
    GenshinItems.cpp
    GenshinItems.h
    DataManager.cpp
    DataManager.h
    HttpClient.cpp
    HttpClient.h
    OptionsDlg.cpp
    OptionsDlg.h
    GenshinPlugin.rc
    resource.h
    pch.h
)

target_include_directories(GenshinPlugin PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(GenshinPlugin PROPERTIES
    RC_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(GenshinPlugin PRIVATE wininet comctl32)

set_target_properties(GenshinPlugin PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    PREFIX ""
)
