idf_component_register(SRCS 
    dfi/Dfi.cxx
    dfi/HttpClient.cxx

    led/LedControl.cxx
    led/RenderTask.cxx

    light_sensor/AD7417.cxx
    light_sensor/LightSensor.cxx

    rest/RestApiHandlers.cxx
    rest/RestServer.cxx

    wifi/Wireless.cxx

    Application.cxx
    sync.cxx
    #StatusLed.cxx

    REQUIRES
    esp_driver_gpio
    esp_driver_i2c
    esp_driver_ledc
    esp_http_client
    esp_app_format
    esp_http_server
    esp_wifi
    json
    nvs_flash
    spiffs

    INCLUDE_DIRS ".")

# Load web data into SPIFFS
set(WEB_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../web-ui/esp-dfi-ignah-ui")
    if(EXISTS ${WEB_SRC_DIR}/dist)
       execute_process(COMMAND du -sh ${WEB_SRC_DIR}/dist OUTPUT_VARIABLE command_return)
       MESSAGE( STATUS "size of dist folder: " ${command_return})
       # uncomment if web data should be updated/written
       # spiffs_create_partition_image(www ${WEB_SRC_DIR}/dist/esp-dfi-ignah-ui FLASH_IN_PROJECT)
    else()
        # message(FATAL_ERROR "${WEB_SRC_DIR}/dist/esp-dfi-ignah-ui doesn't exit. Please run 'ng build' in ${WEB_SRC_DIR}")
    endif()

# Unfortunately the pugixml library performs install and export.
# Override install() and export() to do nothing instead.
function(install)
endfunction()

function(export)
endfunction()
    
set(BUILD_SHARED_LIBS OFF)
set(BUILD_TESTING OFF)

# pugixml
add_compile_definitions(
   PUGIXML_NO_XPATH
   PUGIXML_NO_EXCEPTIONS)

add_subdirectory(components/pugixml)
add_subdirectory(components/Time)
add_subdirectory(components/display-renderer)
add_subdirectory(components/util)

target_link_libraries(${COMPONENT_LIB} PUBLIC 
                pugixml
                Time
                display-renderer
                util
                )
