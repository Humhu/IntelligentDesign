#CMake file for src folder

set( IntelligentDesign_SOURCES
	 GibbsField.cpp
	 MCMCSampler.cpp
	 RandomDistributions.cpp )


add_library( intelligent SHARED ${IntelligentDesign_SOURCES} )

set_target_properties( intelligent PROPERTIES
    OUTPUT_NAME intelligent )

target_link_libraries( intelligent ${IntelligentDesign_LIBRARIES} )
