idf_component_register(
    SRCS
        "main.c"
        "domain/peripherals/rgb/rgb.c"
    INCLUDE_DIRS
        "."
        "${CMAKE_SOURCE_DIR}/include"
)
