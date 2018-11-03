project(Incarnate)
file(GLOB INC_SRC Incarnate/*.cpp)
file(GLOB STB_SRC Incarnate/3rdparty/*.cpp)
add_library(Incarnate SHARED ${INC_SRC} ${STB_SRC})

find_package(OpenCL REQUIRED)

target_link_libraries(Incarnate OpenCL::OpenCL embree3)
