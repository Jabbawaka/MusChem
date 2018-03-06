add_library(Graphics
    Graphics.cpp
    Window.cpp
    Shader.cpp
    SpriteSheet.cpp
    Graph.cpp
    Slider.cpp
    Camera.cpp)

if(APPLE)
    FIND_LIBRARY(OPENGL_LIB OpenGL)
    set(GRAPHICS_LIB ${OPENGL_LIB} glfw GLEW)
else()
    FIND_LIBRARY(OPENGL_LIB GL)
    set(GRAPHICS_LIB ${OPENGL_LIB} glfw GLEW)#X11 Xi Xrandr Xxf86vm Xinerama Xcursor pthread)
endif()

target_link_libraries(Graphics
    GLogger
    ${GRAPHICS_LIB}
    m
    dl
    SOIL)
