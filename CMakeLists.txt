# CMake version string
cmake_minimum_required(VERSION 3.0)

# Project
set(PROJECT lidar)
project(${PROJECT})

# CMake modules
include(cmake/RecurseSubdirs.cmake)

# Minimum Qt version
set(QT_REQUIRED_VERSION 5.9.0)

# Set default output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/result)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/result)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Add compiler flags
set(CMAKE_CXX_STANDARD 11)
add_compile_options(-Wall -fPIC)

# Enable globaly some Qt modules
find_package(Qt5 COMPONENTS
    Core
    Gui
    Charts
REQUIRED)

# Global libraries
set(LIBRARIES
# Qt5
    Qt5::Core
    Qt5::Gui
    Qt5::Charts
)

# Sources and headrers
file(GLOB_RECURSE HEADERS "sources/*.h")
file(GLOB_RECURSE SOURCES "sources/*.cpp")
#file(GLOB_RECURSE QML "sources/*.qml")

# Includes
HEADER_DIRECTORIES(INCLUDES "sources")
include_directories(${INCLUDES})

# Resources
file(GLOB_RECURSE QRC_FILES "*.qrc")

# Qt5 add resources
qt5_add_resources(QRC_SOURCES ${QRC_FILES})

# Qt5 wrap node's headers
qt5_wrap_cpp(MOC_SOURCES ${HEADERS})

# Executable target
add_executable(${PROJECT} ${SOURCES} ${HEADERS} ${QRC_SOURCES} ${MOC_SOURCES})

# Link Libraries
target_link_libraries (${PROJECT} ${LIBRARIES})
