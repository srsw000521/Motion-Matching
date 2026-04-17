cmake_minimum_required(VERSION 3.20)
project(MotionMatchingQt LANGUAGES CXX)

# qt_standard_project_setup() requires Qt 6.3+.
# It enables AUTOMOC, AUTORCC, AUTOUIC and sets C++17.
find_package(Qt6 6.3 REQUIRED COMPONENTS Widgets OpenGLWidgets)
qt_standard_project_setup()

# ---------------------------------------------------------------------------
# Core algorithm sources — all pch.h-free and MFC-free after Phase 1/2 work.
#
# Excluded intentionally:
#   GeoPrimitives.cpp  — IMPLEMENT_SERIAL / CObject (MFC, deferred)
#   matrix.cpp         — #include "pch.h" at line 54 (third-party, MFC via pch)
#   vector3d.cpp       — same
#   pch.cpp            — MSVC PCH creation, not needed in CMake build
#   MFC app layer      — MotionMatching.cpp, MainFrm.cpp, MotionMatchingDoc.cpp,
#                        MotionMatchingView.cpp
# ---------------------------------------------------------------------------
set(CORE_SOURCES
    CMotionMatching.cpp
    Motion.cpp
    MotionFeatured.cpp
    MyJoint.cpp
    MySkeleton.cpp
    MyPosture.cpp
    Trajectory.cpp
    ImPCA.cpp
    LinearAlgebra.cpp
    mat3_function.cpp
    quat.cpp
    MotionState.cpp
)

set(APP_SOURCES
    main.cpp
    MainWindow.h
    MainWindow.cpp
    MotionGLWidget.h
    MotionGLWidget.cpp
)

qt_add_executable(MotionMatchingQt
    ${CORE_SOURCES}
    ${APP_SOURCES}
)

target_compile_definitions(MotionMatchingQt PRIVATE
    # Suppress MSVC warnings for fopen/strtok/etc in core files
    _CRT_SECURE_NO_WARNINGS
    # Required when linking against the static freeglut lib (no DLL present).
    # freeglut.lib is at the project root; MotionGLWidget will include GL headers
    # in Qt Commit 3 — the define must be in place before then.
    FREEGLUT_STATIC
)

target_include_directories(MotionMatchingQt PRIVATE
    # Project root — resolves all existing #include "Header.h" paths
    ${CMAKE_SOURCE_DIR}
    # freeglut / GLUT headers (opengl/include/GL/*.h)
    ${CMAKE_SOURCE_DIR}/opengl/include
)

target_link_libraries(MotionMatchingQt PRIVATE
    Qt6::Widgets
    Qt6::OpenGLWidgets
    # Windows system OpenGL libraries
    opengl32
    glu32
    # Static freeglut — only one .lib found at project root.
    # Assumed x64; replace with the correct path if building x86.
    # Not strictly needed until Qt Commit 3 (MotionGLWidget), but
    # resolving the path here keeps the CMakeLists stable across commits.
    ${CMAKE_SOURCE_DIR}/freeglut.lib
)

# Run the executable from the project root so relative BVH file paths
# ("LocomotionFlat01_000.bvh") resolve correctly at runtime.
set_target_properties(MotionMatchingQt PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
