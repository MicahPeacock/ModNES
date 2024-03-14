cmake_minimum_required(VERSION 3.27)
project(ModNES)

set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: Debug Release."
            FORCE)
endif(NOT CMAKE_BUILD_TYPE)

set(BUILD_STATIC FALSE CACHE STRING "Set this to link external libraries statically")

if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/;${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}")

# Add sources
file(GLOB SOURCES
        "${PROJECT_SOURCE_DIR}/src/mappers/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/main.cpp"
)

function(define_file_basename_for_sources targetname)
    get_target_property(source_files "${targetname}" SOURCES)
    foreach(sourcefile ${source_files})
        # Get source file's current list of compile definitions.
        get_property(defs SOURCE "${sourcefile}"
                PROPERTY COMPILE_DEFINITIONS)
        # Add the FILE_BASENAME=filename compile definition to the list.
        get_filename_component(basename "${sourcefile}" NAME)
        list(APPEND defs "__FILENAME__=\"${basename}\"")
        # Set the updated compile definitions on the source file.
        set_property(
                SOURCE "${sourcefile}"
                PROPERTY COMPILE_DEFINITIONS ${defs})
    endforeach()
endfunction()

# Specify include Directory
include_directories("${PROJECT_SOURCE_DIR}/include")

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2-shared)

add_executable(ModNES ${SOURCES}
        include/mappers/mapper_nrom.hpp)
target_link_libraries(ModNES PRIVATE SDL2::SDL2)

set_property(TARGET ModNES PROPERTY CXX_STANDARD 20)
set_property(TARGET ModNES PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(ModNES)
define_file_basename_for_sources(ModNES)
