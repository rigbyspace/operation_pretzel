# gui/CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(trts_lab_gui LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

# All GUI source files
set(GUI_SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/EngineConfigPanel.cpp
    src/ExecutionPanel.cpp
    src/RhythmVisualizerWidget.cpp
    src/StackViewerWidget.cpp
    src/AnalyzerPanel.cpp
    src/FingerprintPanel.cpp
    src/PhaseMapExplorer.cpp
    src/TheoristExplorerPanel.cpp
    src/OutputTableWidget.cpp
    src/TRTSCoreProcess.cpp
    src/TRTSConfig.cpp
    ../../simulate.c
)

# (Headers listed for IDE convenience)
set(GUI_HEADERS
    include/MainWindow.hpp
    include/EngineConfigPanel.hpp
    include/ExecutionPanel.hpp
    include/RhythmVisualizerWidget.hpp
    include/StackViewerWidget.hpp
    include/AnalyzerPanel.hpp
    include/FingerprintPanel.hpp
    include/PhaseMapExplorer.hpp
    include/TheoristExplorerPanel.hpp
    include/OutputTableWidget.hpp
    include/TRTSConfig.hpp
    include/TRTSCoreProcess.hpp
)

add_executable(trts_lab_gui
    ${GUI_SOURCES}
    ${GUI_HEADERS}
)

# Include GUI headers and the core include directory
target_include_directories(trts_lab_gui
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/..   # for trts_core headers and simulate.h
)

# Link against the full core library (all C sources) and Qt
target_link_libraries(trts_lab_gui
    PRIVATE
        trts_core       # <— brings in simulate_stream plus all rational_/state_/engine_/psi_/koppa_ functions
        Qt5::Core
        Qt5::Widgets
)

# Enable Qt automoc/uic
set_target_properties(trts_lab_gui PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
)

