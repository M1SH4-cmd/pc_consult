find_package(Qt6 6.11 REQUIRED COMPONENTS Test)

# Collect only Core sources
set(CORE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/core/DecisionTree.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TreeDataLoader.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TreeValidator.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ConsultationSession.cpp
)

# Create test executable
qt_add_executable(PCConsultantCoreTests
    test_decision_tree.cpp
    #test_tree_loader.cpp
    test_tree_validator.cpp
    ${CORE_SOURCES}
)

target_include_directories(PCConsultantCoreTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/core
)

# Link Qt Core + Test + resource file
target_link_libraries(PCConsultantCoreTests PRIVATE Qt6::Core Qt6::Test)
qt_add_resources(PCConsultantCoreTests "resources" PREFIX "/" BASE "${CMAKE_SOURCE_DIR}" FILES "${CMAKE_SOURCE_DIR}/resources/data/consultant_tree.json")

# Register with CTest
add_test(NAME PCConsultantCoreTests COMMAND PCConsultantCoreTests)
set_tests_properties(PCConsultantCoreTests PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
