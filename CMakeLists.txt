cmake_minimum_required(VERSION 3.1...3.5)

project("JW_ab_ana")

option (UNIX_USE_CCACHE "Treat warnings as error" ON)

set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")
add_compile_options(-rdynamic -fPIC)
# CERN-ROOT suggest CXX17
add_compile_options(-Wno-cpp)
#set(CMAKE_PREFIX_PATH "/home/wangying/software/boost_1_84_0-build/lib/cmake;${CMAKE_PREFIX_PATH}")

find_package(Boost REQUIRED COMPONENTS system timer)
find_package(ROOT REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/inc
    ${ROOT_INCLUDE_DIRS}
    ${ROOT_INCLUDE_DIR}
    )
find_program(ROOTCINT rootcint PATH_SUFFIXES bin REQUIRED)
set(root_cint_out "root_cint_out.cpp")
set(data_struct "data_strcut_cint.h")
execute_process(COMMAND "${ROOTCINT}" -I ${PROJECT_SOURCE_DIR}/inc -f ${PROJECT_SOURCE_DIR}/src/${root_cint_out} -c ${data_struct} LinkDef.h)
execute_process(COMMAND "${ROOTCINT}" -I ${PROJECT_SOURCE_DIR}/inc -f ${root_cint_out} -c ${data_struct} LinkDef.h)

#file(COPY ${root_cint_out} DESTINATION ${PROJECT_SOURCE_DIR})

file(GLOB sources 
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/*.cxx
    )
file(GLOB headers
    ${PROJECT_SOURCE_DIR}/inc/*.hpp
    ${PROJECT_SOURCE_DIR}/inc/*.h
    ${PROJECT_SOURCE_DIR}/inc/*.hpp
    )

set(exec_name "main")
add_executable(${exec_name} main.cpp ${sources} ${headers})
target_link_libraries(${exec_name} ${ROOT_LIBRARIES})
if (NOT EXISTS "argv.ini")
  file(COPY "${PROJECT_SOURCE_DIR}/calc/argv.ini" DESTINATION "./")
endif()
#target_link_libraries(${exec_name} ${ROOT_LIBRARIES} Boost::timer)
