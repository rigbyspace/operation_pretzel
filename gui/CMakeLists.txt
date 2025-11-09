cmake_minimum_required(VERSION 3.16)
project(trts_lab_gui LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/TRTSConfig.cpp
    src/TRTSCoreProcess.cpp
    src/EngineConfigPanel.cpp
    src/ExecutionPanel.cpp
    src/RhythmVisualizerWidget.cpp
    src/StackViewerWidget.cpp
    src/AnalyzerPanel.cpp
    src/FingerprintPanel.cpp
    src/PhaseMapExplorer.cpp
    src/TheoristExplorerPanel.cpp
    src/OutputTableWidget.cpp
)

set(HEADERS
    include/MainWindow.hpp
    include/TRTSConfig.hpp
    include/TRTSCoreProcess.hpp
    include/EngineConfigPanel.hpp
    include/ExecutionPanel.hpp
    include/RhythmVisualizerWidget.hpp
    include/StackViewerWidget.hpp
    include/AnalyzerPanel.hpp
    include/FingerprintPanel.hpp
    include/PhaseMapExplorer.hpp
    include/TheoristExplorerPanel.hpp
    include/OutputTableWidget.hpp
)

add_executable(trts_lab_gui ${SOURCES} ${HEADERS})

target_include_directories(trts_lab_gui PRIVATE include)

target_link_libraries(trts_lab_gui PRIVATE Qt5::Core Qt5::Widgets)

install(TARGETS trts_lab_gui DESTINATION bin)
