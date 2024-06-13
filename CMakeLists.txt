if("${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" VERSION_GREATER_EQUAL "5.0")
idf_component_register(
    REQUIRES "driver esp_timer"
    SRCS "hx711.cpp"
    INCLUDE_DIRS "include"
)
else()
idf_component_register(
    SRCS "hx711.cpp"
    REQUIRES "esp_timer"
    INCLUDE_DIRS "include"
)
endif()

