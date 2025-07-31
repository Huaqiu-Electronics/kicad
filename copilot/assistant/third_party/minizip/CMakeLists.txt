find_package(ZLIB REQUIRED)
add_library(minizip STATIC
    zip.c
    ioapi.c
)

target_include_directories(minizip PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(minizip PUBLIC ZLIB::ZLIB)
