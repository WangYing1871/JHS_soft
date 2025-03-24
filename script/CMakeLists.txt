set(exec_name0 "draw_wave")
set(exec_name1 "draw_hitmap")
set(exec_name2 "draw_time_dis")
include_directories(
    ${PROJECT_SOURCE_DIR}/script/inc
    )
file(GLOB sources
    ${PROJECT_SOURCE_DIR}/src/${root_cint_out}
    ${PROJECT_SOURCE_DIR}/script/src/draw_wave.cpp
    ${PROJECT_SOURCE_DIR}/script/src/util.cpp
    )
file(GLOB headers
    ${PROJECT_SOURCE_DIR}/script/inc/util.h
    )
file(GLOB sources0
    ${PROJECT_SOURCE_DIR}/src/${root_cint_out}
    ${PROJECT_SOURCE_DIR}/script/src/draw_hitmap.cpp
    ${PROJECT_SOURCE_DIR}/script/src/util.cpp
    )
file(GLOB sources1
    ${PROJECT_SOURCE_DIR}/src/${root_cint_out}
    ${PROJECT_SOURCE_DIR}/script/src/draw_time_dis.cpp
    ${PROJECT_SOURCE_DIR}/script/src/util.cpp
    )
add_executable(${exec_name0} ${sources} ${headers})
target_link_libraries(${exec_name0} ${ROOT_LIBRARIES})
add_executable(${exec_name1} ${sources0} ${headers})
target_link_libraries(${exec_name1} ${ROOT_LIBRARIES})
add_executable(${exec_name2} ${sources1} ${headers})
target_link_libraries(${exec_name2} ${ROOT_LIBRARIES})

string(REPLACE ".cpp" "_rdict.pcm" root_cint_out_rdict ${root_cint_out})
if (NOT EXISTS ${CMAKE_BINARY_DIR}/${root_cint_out_rdict})
cmake_warning(DEPRECATION "${root_cint_out_rdict} can't access! scripts may not exec successful")
else()
file(COPY 
    ${CMAKE_BINARY_DIR}/${root_cint_out_rdict}
    DESTINATION 
    "${CMAKE_CURRENT_BINARY_DIR}")
endif()
