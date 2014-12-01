#CMake file for src folder

set( IntelligentDesign_SOURCES
	 AssemblyConstructor.cpp
	 AssemblySampler.cpp
	 AssemblyVisualizer.cpp
	 BlockVariable.cpp
	 DiscreteAssembly.cpp
	 DiscretePoint.cpp
	 GibbsField.cpp
	 Lattice.cpp
	 MCMCSampler.cpp
	 PotentialCOM.cpp
	 PotentialEdge.cpp
	 PotentialFixed.cpp
	 PotentialHeight.cpp
	 PotentialMass.cpp
	 PotentialRepel.cpp
	 PotentialSupport.cpp
	 RandomDistributions.cpp
	 RendererManager.cpp
	 TreeSearch.cpp )

add_library( intelligent SHARED ${IntelligentDesign_SOURCES} )

set_target_properties( intelligent PROPERTIES
    OUTPUT_NAME intelligent )

target_link_libraries( intelligent ${IntelligentDesign_LIBRARIES} )
