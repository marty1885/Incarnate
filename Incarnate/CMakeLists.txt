project(Incarnate)
file(GLOB INC_SRC Incarnate/*.cpp)
add_library(Incarnate SHARED ${INC_SRC})

find_package(OpenCL REQUIRED)

target_link_libraries(Incarnate OpenCL::OpenCL embree3)
