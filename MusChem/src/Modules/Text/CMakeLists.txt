add_library(Text
    Text.cpp
    Font.cpp
    TextBoard.cpp)

target_link_libraries(Text
    GLogger
    GParser
    freetype)
