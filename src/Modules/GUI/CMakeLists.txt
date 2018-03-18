add_library(GUI
    Graph.cpp
    Slider.cpp
    DiscSlider.cpp
    Switch.cpp)

target_link_libraries(GUI
    GLogger
    SOIL)
