idf_component_register(SRCS 
    #dfi/Dfi.cxx
    #dfi/HttpClient.cxx

    Application.cxx
    #StatusLed.cxx
    #Wireless.cxx

    INCLUDE_DIRS ".")

#add_subdirectory(components/pugixml)
#add_subdirectory(components/Time)
#add_subdirectory(components/util)

target_link_libraries(${COMPONENT_LIB} PUBLIC 
                #pugixml
                #Time
                #util
                )
