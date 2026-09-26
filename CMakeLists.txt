cmake_minimum_required(VERSION 3.24)

project(NintendoWare C CXX ASM)

function(nw_library TARGET_NAME MODULE_DIR)
    file(GLOB_RECURSE MODULE_SOURCES
        "${MODULE_DIR}/*.cpp"
        "${MODULE_DIR}/*.c"
        "${MODULE_DIR}/*.s"
        "${MODULE_DIR}/*.asm"
    )

    if(NOT MODULE_SOURCES)
        message(STATUS "Skipping ${TARGET_NAME} (no sources)")
        return()
    endif()

    add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

    set_target_properties(${TARGET_NAME} PROPERTIES
        OUTPUT_NAME "lib${TARGET_NAME}"
        SUFFIX ".fast.a"
    )

    target_compile_options(${TARGET_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:
        --multibyte-chars --signed_chars --cpu=MPCore
        --dollar --littleend --wchar --wchar16 --library_interface=rvct
        --apcs=//interwork --data-reorder --split_sections
        --debug_info=line_inlining_extensions --force_new_nothrow --fpmode=fast --no_exceptions --rtti --rtti_data --remarks --no_vfe --allow_fpreg_for_nonfpdata
        --diag_suppress=3017,940,997,177,541,1,1134,550,83,427,2525,117,284,174,111,1441,161,826,177,1549,513,1441,340,228,2530,1297,401,261,381,2516,2192,1596,193,113,1035,2523,2091,1663,1801,1621,2519,1676,63,286,74,271,2945,1301,1945,1847,2813,68,2523
        --cpp --arm -O3 -Otime
        -DNN_PROCESSOR_ARM11MPCORE -DNW_RELEASE -DNW_PLATFORM_CTR
        >
    )

    target_compile_options(${TARGET_NAME} PRIVATE $<$<COMPILE_LANGUAGE:C>:
        --multibyte-chars --signed_chars --cpu=MPCore
        --dollar --littleend --wchar --wchar16 --library_interface=rvct
        --apcs=//interwork --data-reorder --split_sections
        --debug_info=line_inlining_extensions --force_new_nothrow --fpmode=fast --no_exceptions --rtti --rtti_data --remarks --no_vfe --allow_fpreg_for_nonfpdata
        --diag_suppress=3017,940,997,177,541,1,1134,550,83,427,2525,117,284,174,111,1441,161,826,177,1549,513,1441,340,228,2530,1297,401,261,381,2516,2192,1596,193,113,1035,2523,2091,1663,1801,1621,2519,1676,63,286,74,271,2945,1301,1945,1847,2813,68,2523
        --c99 --arm -O3 -Otime
        -DNN_PROCESSOR_ARM11MPCORE -DNW_RELEASE -DNW_PLATFORM_CTR
        >
    )

    target_include_directories(${TARGET_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${MODULE_DIR}
        ${PROJECT_SOURCE_DIR}/../CTRSDK-4_2_5/include
    )

    target_compile_definitions(${TARGET_NAME} PUBLIC
        NON_MATCHING=
        NW_RELEASE
    )
endfunction()

nw_library(nw_anim    ${CMAKE_CURRENT_SOURCE_DIR}/sources/libraries/anim)
nw_library(nw_font    ${CMAKE_CURRENT_SOURCE_DIR}/sources/libraries/font)
nw_library(nw_gfx     ${CMAKE_CURRENT_SOURCE_DIR}/sources/libraries/gfx)
nw_library(nw_io      ${CMAKE_CURRENT_SOURCE_DIR}/sources/libraries/io)
nw_library(nw_lyt     ${CMAKE_CURRENT_SOURCE_DIR}/sources/libraries/lyt)
nw_library(nw_os      ${CMAKE_CURRENT_SOURCE_DIR}/sources/libraries/os)
nw_library(nw_snd     ${CMAKE_CURRENT_SOURCE_DIR}/sources/libraries/snd)
nw_library(nw_ut      ${CMAKE_CURRENT_SOURCE_DIR}/sources/libraries/ut)

add_library(NintendoWare INTERFACE)

target_link_libraries(NintendoWare INTERFACE
    nw_anim
    nw_font
    nw_gfx
    nw_io
    nw_lyt
    nw_os
    nw_snd
    nw_ut
)