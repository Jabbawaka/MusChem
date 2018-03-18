add_library(Audio
    Envelope.cpp
    Keyboard.cpp)

target_link_libraries(Audio
    portaudio
    rt
    m
    asound)
