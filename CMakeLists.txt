cmake_minimum_required(VERSION 3.10)
project(mario-kart-gl)

if (MSVC)
    add_compile_options(/W3)
else()
    add_compile_options(-Wall -Wextra)
endif()

set(SOURCES
    main.c
    include/glad/glad.c
)

if(WIN32)
    set(LIBS opengl32 gdi32)
    add_compile_options(/W3)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif(APPLE)
    set(LIBS "-framework OpenGL -framework Cocoa")
else()
    set(LIBS X11 GL Xrandr)
endif()

add_executable(mario-kart-gl ${SOURCES})

target_link_libraries(mario-kart-gl ${LIBS})

set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "*.dSYM")
