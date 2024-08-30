add_library(miditones INTERFACE)

target_include_directories(miditones
    INTERFACE
    .)

target_sources(miditones
    INTERFACE
    midiFreqTable.cpp
    miditones.cpp
    miditonesFile.cpp
    )