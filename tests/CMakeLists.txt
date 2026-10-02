cmake_minimum_required(VERSION 3.21)

project(PCConsultantTests
    VERSION 0.1.0
    DESCRIPTION "Tests for PCConsultant Core"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 6.11 REQUIRED COMPONENTS Test Core)

# Test DecisionTree
add_executable(test_decision_tree
    ${CMAKE_CURRENT_SOURCE_DIR}/test_decision_tree.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/DecisionTree.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/DecisionNode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/NodeId.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/NodeKind.h
)

target_include_directories(test_decision_tree PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

target_link_libraries(test_decision_tree PRIVATE Qt6::Test)

add_test(NAME test_decision_tree COMMAND test_decision_tree)

# Test TreeDataLoader
add_executable(test_tree_loader
    ${CMAKE_CURRENT_SOURCE_DIR}/test_tree_loader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/TreeDataLoader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/DecisionTree.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/DecisionNode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/NodeId.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/NodeKind.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../resources/resources.qrc
)

target_include_directories(test_tree_loader PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

target_link_libraries(test_tree_loader PRIVATE Qt6::Test Qt6::Core)

add_test(NAME test_tree_loader COMMAND test_tree_loader)

# Test TreeValidator
add_executable(test_tree_validator
    ${CMAKE_CURRENT_SOURCE_DIR}/test_tree_validator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/TreeValidator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/DecisionTree.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/DecisionNode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/NodeId.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/NodeKind.h
)

target_include_directories(test_tree_validator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

target_link_libraries(test_tree_validator PRIVATE Qt6::Test)

add_test(NAME test_tree_validator COMMAND test_tree_validator)

# Test ConsultationSession
add_executable(test_consultation_session
    ${CMAKE_CURRENT_SOURCE_DIR}/test_consultation_session.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/ConsultationSession.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/DecisionTree.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/DecisionNode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/NodeId.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/NodeKind.h
)

target_include_directories(test_consultation_session PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

target_link_libraries(test_consultation_session PRIVATE Qt6::Test)

add_test(NAME test_consultation_session COMMAND test_consultation_session)

# Integration test
add_executable(test_integration
    ${CMAKE_CURRENT_SOURCE_DIR}/test_integration.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/TreeDataLoader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/TreeValidator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/ConsultationSession.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/DecisionTree.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/DecisionNode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/NodeId.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/NodeKind.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../resources/resources.qrc
)

target_include_directories(test_integration PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

target_link_libraries(test_integration PRIVATE Qt6::Test Qt6::Core)

add_test(NAME test_integration COMMAND test_integration)