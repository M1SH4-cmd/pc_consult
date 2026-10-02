cmake_minimum_required(VERSION 3.21)

project(PCConsultant
    VERSION 0.1.0
    DESCRIPTION "Desktop consultant for PC components"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/utf-8)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.11 REQUIRED COMPONENTS Widgets Test)

qt_add_executable(PCConsultant
    src/main.cpp
    src/ui/MainWindow.h
    src/ui/MainWindow.cpp
    src/ui/HistoryView.h
    src/ui/HistoryView.cpp
    src/core/Answer.h
    src/core/ConsultationSession.h
    src/core/ConsultationSession.cpp
    src/core/DecisionNode.h
    src/core/DecisionTree.h
    src/core/DecisionTree.cpp
    src/core/HistoryEntry.h
    src/core/NodeId.h
    src/core/NodeKind.h
    src/core/TreeDataLoader.h
    src/core/TreeDataLoader.cpp
    src/core/TreeValidator.h
    src/core/TreeValidator.cpp
    src/settings/AppSettings.h
    src/settings/AppSettings.cpp
    src/settings/SettingsDialog.h
    src/settings/SettingsDialog.cpp
    src/settings/ThemeManager.h
    src/settings/ThemeManager.cpp
    resources/resources.qrc
)

target_include_directories(PCConsultant PRIVATE src)

target_link_libraries(PCConsultant PRIVATE Qt6::Widgets)

enable_testing()

add_subdirectory(tests)
