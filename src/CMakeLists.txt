set(SOURCE_FILES "png2ico.cpp")

add_executable(png2ico ${SOURCE_FILES})

target_include_directories(png2ico
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

set_target_properties(png2ico
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    )
