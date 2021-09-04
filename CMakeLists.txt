cmake_minimum_required(VERSION 3.8)

project(QtVtkViewer)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

# Find package Qt
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)

# Use Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find package VTK
find_package(VTK
    9.0
    REQUIRED
    COMPONENTS
        GUISupportQt
        IOLegacy
        CommonColor
        CommonComputationalGeometry
        CommonCore
        CommonDataModel
        FiltersCore
        FiltersSources
        InteractionStyle
        RenderingContextOpenGL2
        RenderingCore
        RenderingFreeType
        RenderingGL2PSOpenGL2
        RenderingOpenGL2
  )

add_subdirectory(app)

