project(incViewer)

include_directories(.)
find_package(SFML 2.5 COMPONENTS graphics REQUIRED)

FILE(GLOB IMGUI_SRC imgui/*.cpp)

add_executable(incViewer incViewer.cpp Shader.cpp Surface.cpp ${IMGUI_SRC})
include_directories(${EIGEN3_INCLUDE_DIR} imgui)

target_link_libraries(incViewer sfml-graphics lua Incarnate GL GLEW)
