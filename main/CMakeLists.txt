idf_component_register(SRCS 
    dfi/Dfi.cxx
    dfi/HttpClient.cxx

    led/LedControl.cxx
    led/RenderTask.cxx

    light_sensor/AD7417.cxx
    light_sensor/LightSensor.cxx

    wifi/Wireless.cxx

    Application.cxx
    sync.cxx
    #StatusLed.cxx

    INCLUDE_DIRS ".")

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
