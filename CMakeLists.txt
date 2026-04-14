cmake_minimum_required(VERSION 3.15)

if(CMAKE_GENERATOR MATCHES "MinGW")
    # Pick a real x64 MinGW toolchain before project() enables the languages.
    # The old temp w64devkit path goes stale, so prefer the installed Qt MinGW first.
    if(NOT DEFINED CMAKE_CXX_COMPILER OR CMAKE_CXX_COMPILER STREQUAL "")
        find_program(GENSHINPLUGIN_GXX
            NAMES x86_64-w64-mingw32-g++.exe g++.exe
            HINTS
                "Y:/Dev/Qt/Tools/mingw1310_64/bin"
                "Y:/Dev/MinGW/bin"
                "C:/Users/jery/AppData/Local/Temp/w64devkit/w64devkit/bin")
        if(GENSHINPLUGIN_GXX)
            set(CMAKE_CXX_COMPILER "${GENSHINPLUGIN_GXX}" CACHE FILEPATH "C++ compiler" FORCE)

            find_program(GENSHINPLUGIN_WINDRES
                NAMES windres.exe
                HINTS
                    "Y:/Dev/Qt/Tools/mingw1310_64/bin"
                    "Y:/Dev/MinGW/bin"
                    "C:/Users/jery/AppData/Local/Temp/w64devkit/w64devkit/bin")
            if(GENSHINPLUGIN_WINDRES)
                set(CMAKE_RC_COMPILER "${GENSHINPLUGIN_WINDRES}" CACHE FILEPATH "Resource compiler" FORCE)
            endif()
        endif()
    endif()
endif()

project(GenshinPlugin LANGUAGES CXX RC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(GenshinPlugin MODULE
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
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/release
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/release
    PREFIX ""
)

if(MSVC)
    set_target_properties(GenshinPlugin PROPERTIES
        PDB_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_options(GenshinPlugin PRIVATE /INCREMENTAL:NO)
endif()
