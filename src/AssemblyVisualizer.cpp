#include "intelligent/AssemblyVisualizer.h"
#include "intelligent/BlockVariable.h"

namespace intelligent {

	AssemblyVisualizer::AssemblyVisualizer( RendererManager& _renderer ) :
		renderer( _renderer ),
		requestCounter( 0 ) {}

	void AssemblyVisualizer::Visualize( const DiscreteAssembly& assembly ) {

		std::vector<RenderRequestVariant> requests;
		
		ClearRequest crequest;
		RenderRequestVariant creq = crequest;
		requests.push_back( creq );
		
		requestCounter = 0;
		
		DiscreteBox3::Operator visOp =
			boost::bind( &AssemblyVisualizer::VisualizePoint, this, assembly, boost::ref(requests), _1 );
		DiscreteBox3 latticeBounds = assembly.GetLattice().GetBoundingBox();

		latticeBounds.Iterate( visOp );

		renderer.QueueRenderRequests( requests );
	}

	void AssemblyVisualizer::VisualizePoint( const DiscreteAssembly& assembly,
											 std::vector<RenderRequestVariant>& requests,
											 const DiscretePoint3& point ) {

		CubeRenderRequest creq;
		creq.id = requestCounter++;
		creq.color = Color( 0, 0, 1 );
		creq.useLighting = true;
		creq.showEdges = true;
		creq.edgeColor = Color( 0, 0, 0 );

		creq.center[0] = point.x;
		creq.center[1] = point.y;
		creq.center[2] = point.z;
		creq.lengths[0] = 1;
		creq.lengths[1] = 1;
		creq.lengths[2] = 1;

		unsigned int id = assembly.GetLattice().GetNodeID( point );
		GibbsVariable::Ptr var = assembly.GetField().GetVariable( id );
		BlockVariable::Ptr block = std::dynamic_pointer_cast<BlockVariable>( var );
		if( !block ) {
			throw std::runtime_error( "Variable was not a BlockVariable!" );
		}

		BlockType state = block->GetState();
		switch( state ) {
			case BLOCK_EMPTY:
				creq.color = Color( 0, 0, 0 );
				creq.representation = RENDER_GEOM_WIREFRAME;
				break;
			case BLOCK_HALF:
				creq.color = Color( 1, 0, 0 );
				creq.opacity = 0.5;
				break;
			case BLOCK_FULL:
				creq.opacity = 1.0;
				break;
		}
		
		RenderRequestVariant vreq = creq;
		requests.push_back( vreq );
		
	}
	
};