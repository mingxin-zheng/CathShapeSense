# 1D Catheter Shape Fitting
A CMake/C++ project for embedding a VTK 3D view in a Qt window.

## Prerequisites
- Qt 5.x downloaded.
- VTK 9.x source downloaded & compiled. See [Build-VTK.md](doc/Build-VTK.md) for a guide to compile VTK.
- Ceres-solver built without glog/gflags/suitesparse on Windows

## Usage
1. Clone or download this repository.
2. Open CMakeLists.txt in Qt Creator.
3. Configure project.
	1. Set build directory.
	2. Specify QT_DIR.
	3. Specify VTK_DIR.
	4. Specify CERES_DIR
4. Build project.

## License
