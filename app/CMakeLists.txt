#CMakeLists for building applications
add_executable( TestCone TestCone.cpp )
target_link_libraries( TestCone intelligent ${IntelligentDesign_LIBRARIES} )