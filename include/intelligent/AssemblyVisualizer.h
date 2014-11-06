#ifndef _ASSEMBLY_VISUALIZER_H_
#define _ASSEMBLY_VISUALIZER_H_

#include "intelligent/RendererManager.h"
#include "intelligent/DiscreteAssembly.h"

namespace intelligent {

	// TODO Replace with actual DiscreteAssembly class
	class DiscreteAssembly;
		
	class AssemblyVisualizer {
	public:

		AssemblyVisualizer( RendererManager& _renderer );

		void Visualize( const DiscreteAssembly& assembly );
		
	private:

		unsigned int requestCounter;
		RendererManager& renderer;

		void VisualizePoint( const DiscreteAssembly& assembly,
							 const DiscretePoint3& point );
		
	};
	
}

#endif