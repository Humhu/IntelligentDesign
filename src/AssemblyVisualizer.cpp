#include "intelligent/AssemblyVisualizer.h"
#include "intelligent/BlockVariable.h"

namespace intelligent {

	AssemblyVisualizer::AssemblyVisualizer( RendererManager& _renderer ) :
		renderer( _renderer ),
		requestCounter( 0 ),
		showOutlines( false ) {}

	void AssemblyVisualizer::SetOutlines( bool out ) {
		showOutlines = out;
	}
		
	void AssemblyVisualizer::Visualize( const DiscreteAssembly& assembly ) {

		std::vector<RenderRequestVariant> requests;
		
		ClearRequest crequest;
		RenderRequestVariant clreq = crequest;
		requests.push_back( clreq );
		
		requestCounter = 0;
		
		DiscreteBox3::Operator visOp =
			boost::bind( &AssemblyVisualizer::VisualizePoint, this, assembly, boost::ref(requests), _1 );
		DiscreteBox3 latticeBounds = assembly.GetLattice().GetBoundingBox();

		latticeBounds.Iterate( visOp );

		ArrowRenderRequest areq;
		areq.start[2] = latticeBounds.minZ - 0.5;
		areq.start[0] = latticeBounds.minX - 1;
		areq.start[1] = latticeBounds.minY - 1;

		areq.finish[2] = latticeBounds.maxZ + 0.5;
		areq.finish[0] = latticeBounds.minX - 1;
		areq.finish[1] = latticeBounds.minY - 1;

		double scale = latticeBounds.maxZ - latticeBounds.minZ;
		areq.tipLength = 0.1;
		areq.tipRadius = 0.05;
		areq.tipResolution = 10;
		areq.shaftRadius = 0.01;
		areq.id = requestCounter++;
		RenderRequestVariant areqv = areq;
		requests.push_back( areqv );

		if( !showOutlines ) {
			CubeRenderRequest creq;
			creq.id = requestCounter++;
			creq.color = Color( 0, 0, 0 );
			creq.useLighting = false;
			creq.showEdges = true;
			creq.edgeColor = Color( 0, 0, 0 );

			creq.center[0] = 0.5*(latticeBounds.maxX + latticeBounds.minX);
			creq.center[1] = 0.5*(latticeBounds.maxY + latticeBounds.minY);
			creq.center[2] = 0.5*(latticeBounds.maxZ + latticeBounds.minZ);
			creq.lengths[0] = latticeBounds.maxX - latticeBounds.minX;
			creq.lengths[1] = latticeBounds.maxY - latticeBounds.minY;
			creq.lengths[2] = latticeBounds.maxZ - latticeBounds.minZ;
			creq.representation = RENDER_GEOM_WIREFRAME;
			RenderRequestVariant creqv = creq;
			requests.push_back( creqv );
		}
		
		renderer.ClearRenderRequests();
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
				if( !showOutlines ) { return; }
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