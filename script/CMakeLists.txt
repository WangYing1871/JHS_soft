function(add_exec sname ename)
  file(GLOB sources
      ${PROJECT_SOURCE_DIR}/src/${root_cint_out}
      ${PROJECT_SOURCE_DIR}/script/src/${sname}
      ${PROJECT_SOURCE_DIR}/script/src/util.cpp
      #${PROJECT_SOURCE_DIR}/src/util.cpp
      )
  add_executable(${ename} ${sources})
  target_link_libraries(${ename} ${ROOT_LIBRARIES})
endfunction()
function(get_file_name path ret)
  string(LENGTH "${path}" length)
  string(FIND "${path}" "/" del_index REVERSE)
  math(EXPR sub_result "${length}-${del_index}")
  math(EXPR start "${del_index}+1")
  string(SUBSTRING "${path}" "${start}" "${sub_result}" name)
  set(${ret} "${name}" PARENT_SCOPE)
endfunction()
file(GLOB headers
    ${PROJECT_SOURCE_DIR}/script/inc/util.h
    )
include_directories(
    ${PROJECT_SOURCE_DIR}/script/inc
    ${Boost_INCLUDE_DIRS}
    )
file(GLOB scripts
    ${PROJECT_SOURCE_DIR}/script/src/*.cpp
    ${PROJECT_SOURCE_DIR}/script/src/*.cxx
    ${PROJECT_SOURCE_DIR}/script/src/*.cc
    ${PROJECT_SOURCE_DIR}/script/src/*.C
    )
foreach(x ${scripts})
  if (NOT "${x}" STREQUAL "${PROJECT_SOURCE_DIR}/script/src/util.cpp")
    cmake_language(CALL get_file_name "${x}" source_name)
    string(REGEX REPLACE "(\.cpp)|(\.cxx)|(\.cc)|(\.C)" "" exec_name ${source_name})
    cmake_language(CALL add_exec "${source_name}" "${exec_name}")
  endif()
endforeach()
string(REPLACE ".cpp" "_rdict.pcm" root_cint_out_rdict ${root_cint_out})
if (NOT EXISTS ${CMAKE_BINARY_DIR}/${root_cint_out_rdict})
cmake_warning(DEPRECATION "${root_cint_out_rdict} can't access! scripts may not exec successful")
else()
file(COPY 
    ${CMAKE_BINARY_DIR}/${root_cint_out_rdict}
    DESTINATION 
    "${CMAKE_CURRENT_BINARY_DIR}")
endif()
