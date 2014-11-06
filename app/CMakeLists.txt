#CMakeLists for building applications
add_executable( TestCone TestCone.cpp )
target_link_libraries( TestCone intelligent ${IntelligentDesign_LIBRARIES} )

add_executable( TestAssembly TestAssembly.cpp )
target_link_libraries( TestAssembly intelligent ${IntelligentDesign_LIBRARIES} )