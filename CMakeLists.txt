cmake_minimum_required(VERSION 3.1...3.5)

project("JW_ab_ana")

option (UNIX_USE_CCACHE "Treat warnings as error" ON)

set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")
add_compile_options(-rdynamic -fPIC)
# CERN-ROOT suggest CXX17
add_compile_options(-Wno-cpp)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup()

#set_target_properties(menus PROPERTIES
#    WIN32_EXECUTABLE TRUE
#    MACOSX_BUNDLE TRUE
#)

#set(CMAKE_PREFIX_PATH "/home/wangying/software/boost_1_84_0-build/lib/cmake;${CMAKE_PREFIX_PATH}")

#find_package(Boost REQUIRED COMPONENTS system timer)
#find_package(ncurese)

#if(NCURSES_FOUND)
#  message(STATUS ${NCURSES_INCLUDE_DIRS})
#else()
#  message(FATAL_ERROR "ncurese library not found.")
#endif()

find_package(Boost COMPONENTS program_options REQUIRED)
#if (BOOST_FOUND)
#  message(STATUS "BOOST_FOUND")
#else()
#  message(STATUS "!BOOST_FOUND")
#endif()
find_package(ROOT REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/inc
    ${ROOT_INCLUDE_DIRS}
    ${ROOT_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/gui/inc
    ${PROJECT_SOURCE_DIR}/gui/3rdparty
    )
find_program(ROOTCINT rootcint PATH_SUFFIXES bin REQUIRED)
set(root_cint_out "root_cint_out.cpp")
set(data_struct "data_strcut_cint.h")
execute_process(COMMAND "${ROOTCINT}" -I ${PROJECT_SOURCE_DIR}/inc -f ${PROJECT_SOURCE_DIR}/src/${root_cint_out} -c ${data_struct} LinkDef.h)
execute_process(COMMAND "${ROOTCINT}" -I ${PROJECT_SOURCE_DIR}/inc -f ${root_cint_out} -c ${data_struct} LinkDef.h)

find_program(UIC uic PATH_SUFFIXES bin REQUIRED)

message(STATUS "uic")
file(GLOB ui_file
  ${PROJECT_SOURCE_DIR}/gui/ui/*.ui
  )
foreach(i ${ui_file})
  STRING(REGEX REPLACE ".+/(.+)\\..*" "\\1" ui_file_name ${i})
  set(ui_file_name "ui_${ui_file_name}.h")
  message(STATUS "${UIC} ${i} -o ${ui_file_name}")
  execute_process(COMMAND "${UIC}" ${i} -o ${ui_file_name})
  file(COPY 
    "${CMAKE_BINARY_DIR}/${ui_file_name}"
    DESTINATION 
    "${PROJECT_SOURCE_DIR}/gui/inc"
    )
  file(COPY
    "${i}"
    DESTINATION 
    "${PROJECT_SOURCE_DIR}/gui/inc"
    )
endforeach()
#file(GLOB_RECURSE files RELATIVE ${PROJECT_SOURCE_DIR}
#    "src/*.cpp"
#    )
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

file(GLOB gui
  ${PROJECT_SOURCE_DIR}/gui/src/*.cpp
  ${PROJECT_SOURCE_DIR}/gui/inc/*.h
  )

file (GLOB 3rdparty
  ${PROJECT_SOURCE_DIR}/gui/3rdparty/*.cpp
  )

set(exec_name "JSH")
#add_executable(${exec_name} main.cpp ${sources} ${headers})
add_executable(
  ${exec_name} 
  main.cpp 
  ${sources} 
  ${headers}
  ${gui}
  ${3rdparty}
)
target_link_libraries(
  ${exec_name} 
  ${ROOT_LIBRARIES} 
  ${Boost_LIBRARIES}
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
  )

if (NOT EXISTS "calc")
  file(COPY "${PROJECT_SOURCE_DIR}/calc" DESTINATION "./")
endif()
if (NOT EXISTS "argv.ini")
  file(COPY "./calc/argv.ini" DESTINATION "./")
endif()

add_subdirectory(script)
#target_link_libraries(${exec_name} ${ROOT_LIBRARIES} Boost::timer)
