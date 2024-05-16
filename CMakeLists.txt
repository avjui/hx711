if("${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" VERSION_GREATER_EQUAL "5.0")
idf_component_register(
    REQUIRES "driver"
    SRCS "hx711.cpp"
    INCLUDE_DIRS "."
)
else()
idf_component_register(
    SRCS "hx711.cpp"
    INCLUDE_DIRS "."
)
endif()

