add_library(Audio
    Envelope.cpp)

target_link_libraries(Audio
    portaudio
    rt
    m
    asound)
