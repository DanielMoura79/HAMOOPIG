cmake_minimum_required(VERSION 3.10)

project(HAMOOPIG)

set(SGDK_VERSION 1.90)

# Current uid/gid
if (UNIX OR APPLE)
    execute_process (COMMAND "id -u" OUTPUT_VARIABLE UID)
    execute_process (COMMAND "id -g" OUTPUT_VARIABLE GID)
else()
    set(UID 1000)
    set(GID 1000)
endif ()

set(SGDK_COMPILE_COMMAND
    docker run -it --rm --user ${UID}:${GID} -v ${CMAKE_CURRENT_SOURCE_DIR}:/src sgdk:${SGDK_VERSION}
)

set(PROJECT_COMPILE_COMMAND
    docker run -it --rm --user ${UID}:${GID} -v ${CMAKE_CURRENT_SOURCE_DIR}:/src sgdk:${SGDK_VERSION}
)

add_custom_target(HAMOOPIG ALL ${SGDK_COMPILE_COMMAND}
        COMMAND ${PROJECT_COMPILE_COMMAND}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})