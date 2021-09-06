set(CMAKE_INCLUDE_CURRENT_DIR ON)

# List source files & resources
file (GLOB Sources *.cpp)
file (GLOB Headers *.h)
file (GLOB Resources *.qrc)
file (GLOB UIs *.ui)

include_directories( ${CERES_INCLUDE_DIRS} )

# Build executable
add_executable(QtVtkViewer WIN32 ${Sources} ${Headers} ${Resources} ${UIs})

target_link_libraries(QtVtkViewer PRIVATE 
        Qt5::Widgets Qt5::OpenGL VTK::GUISupportQt VTK::IOLegacy ${VTK_LIBRARIES} ${CERES_LIBRARIES})

vtk_module_autoinit(
    TARGETS QtVtkViewer
    MODULES VTK::GUISupportQt VTK::IOLegacy ${VTK_LIBRARIES}
    )