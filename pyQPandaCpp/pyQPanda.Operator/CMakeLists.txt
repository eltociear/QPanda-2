cmake_minimum_required(VERSION 3.1)
project(pyQPandaOperator)

set(LIB_PYQPANDA_OPERATOR pyQPandaOperator)
file(GLOB_RECURSE PYQPANDA_OPERATOR_SRC *.cpp)
add_library(${LIB_PYQPANDA_OPERATOR} SHARED ${PYQPANDA_OPERATOR_SRC})

if (UNIX)
    if (APPLE)
        set_target_properties(${LIB_PYQPANDA_OPERATOR} PROPERTIES SUFFIX ".so")
    endif(APPLE)
    set_target_properties(${LIB_PYQPANDA_OPERATOR} PROPERTIES PREFIX "")
elseif(WIN32)
    if (NOT MSVC)
        set_target_properties(${LIB_PYQPANDA_OPERATOR} PROPERTIES PREFIX "")
    endif()
    set_target_properties(${LIB_PYQPANDA_OPERATOR} PROPERTIES SUFFIX ".pyd")
endif(UNIX)

target_link_libraries(${LIB_PYQPANDA_OPERATOR} ${LIB_QPANDA} ${LIB_QALG} ${LIB_VAR} ${PYTHON_LIBRARIES})

set(PYQPANDA_OUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../pyQPanda/pyqpanda/Operator")
if (UNIX)
    set(PYQPANDA_OUT_LIB "${LIB_PYQPANDA_OPERATOR}.so")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${LIBRARY_OUTPUT_PATH})
elseif (WIN32)
    set(PYQPANDA_OUT_LIB "${LIB_PYQPANDA_OPERATOR}.pyd")
endif(UNIX)

add_custom_command(OUTPUT ${PYQPANDA_OUT_LIB}
   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/${PYQPANDA_OUT_LIB} ${PYQPANDA_OUT_PATH}/${PYQPANDA_OUT_LIB}
 )
add_custom_target(CopyQPandaTaskOperator ALL DEPENDS ${PYQPANDA_OUT_LIB} ${LIB_PYQPANDA_OPERATOR})

# generate stub file for IDE intelliSense 
if(PYQPANDA_STUBGEN)
    add_custom_command(TARGET CopyQPandaTaskOperator
        POST_BUILD
        COMMAND ${PYTHON_EXECUTABLE} ${PYQPANDA_POST_BUILD_TOOL_DIR}/stubgen/stubgen.py --search-path ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE} -m ${LIB_PYQPANDA_OPERATOR} -o ${PYQPANDA_OUT_PATH}
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
        VERBATIM
        USES_TERMINAL
    )
endif(PYQPANDA_STUBGEN)
