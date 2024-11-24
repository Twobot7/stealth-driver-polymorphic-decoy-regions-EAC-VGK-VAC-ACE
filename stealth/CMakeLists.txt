cmake_minimum_required(VERSION 3.10)
project(DecoySystem)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add source files
set(SOURCES
    src/main.c
    src/loader/loader.c
    src/deception/decoy_manager.c
)

# Create executable
add_executable(DecoySystem ${SOURCES})

# Add include directories
target_include_directories(DecoySystem PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# Link Windows libraries
target_link_libraries(DecoySystem PRIVATE
    kernel32
    user32
)

# Set Windows subsystem
if(MSVC)
    set_target_properties(DecoySystem PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:CONSOLE"
    )
endif() 