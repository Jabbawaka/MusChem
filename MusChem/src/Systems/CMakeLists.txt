add_library(Systems
    Systems.cpp)

target_link_libraries(Systems
    Graphics
    Input
    Text
    Monitor
    GameFlow)
