include(FetchContent)

function(add_target_if_missing TARGET GIT_REPOSITORY GIT_TAG)
    if(TARGET ${TARGET})
        return()
    endif()

    message("${TARGET} not found. Fetch the contents")
    FetchContent_Declare(
        ${TARGET}
        GIT_REPOSITORY ${GIT_REPOSITORY}
        GIT_TAG ${GIT_TAG}
        GIT_SHALLOW 1
    )
    FetchContent_MakeAvailable(${TARGET})
endfunction(add_target_if_missing)

add_target_if_missing(nodec https://github.com/nodec-project/nodec.git main)
add_target_if_missing(nodec_scene https://github.com/nodec-project/nodec_scene.git main)

function(add_basic_test TARGET TEST_SOURCES)
    add_executable(${TARGET} ${TEST_SOURCES})
    target_link_libraries(${TARGET} nodec_world)
    target_include_directories(${TARGET} PRIVATE ${nodec_SOURCE_DIR}/tests/common)

    add_test(NAME ${TARGET} COMMAND ${TARGET})
endfunction(add_basic_test)

add_basic_test("nodec_world__system_registry" system_registry.cpp)
