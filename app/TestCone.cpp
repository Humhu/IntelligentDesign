#include "intelligent/RendererManager.h"

using namespace intelligent;

int main() {

	RendererManager rman( "Output" );

	std::cout << "Initialization complete." << std::endl;

	PlaneRenderRequest request;
	request.id = 1;
	request.center[0] = 0;
	request.center[1] = 0;
	request.center[2] = -0.05;

	request.lengths[0] = 0.5;
	request.lengths[1] = 0.5;

	request.normal[0] = 0;
	request.normal[1] = 0;
	request.normal[2] = 1;

	RenderRequestVariant pvar = request;
	rman.QueueRenderRequest( pvar );
	
	for( unsigned int i = 0; i < 100; i++ ) {

		CubeRenderRequest crequest;
		crequest.id = 0;
		crequest.center[0] = 0;
		crequest.center[1] = 0;
		crequest.center[2] = 0;
		crequest.lengths[0] = 0.1;
		crequest.lengths[1] = 0.1;
		crequest.lengths[2] = 0.1;

		crequest.color = Color( 1.0, 1.0, 0.0 );
		
		crequest.opacity = 0.01*i;

		RenderRequestVariant variant = crequest;

		rman.QueueRenderRequest( variant );

		usleep( 1E5 );
	}

}
