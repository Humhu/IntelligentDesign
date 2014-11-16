#include "intelligent/AssemblyConstructor.h"
#include "intelligent/AssemblyVisualizer.h"
#include "intelligent/RendererManager.h"
#include "intelligent/DiscreteAssembly.h"
#include "intelligent/MCMCSampler.h"

// Potentials
#include "intelligent/PotentialSupport.h"
#include "intelligent/PotentialHeight.h"
#include "intelligent/PotentialMass.h"
#include "intelligent/PotentialEdge.h"

using namespace intelligent;

GibbsVariable::Ptr CreateBlock( const GibbsField& field, unsigned int id ) {
	return std::make_shared<BlockVariable>( field, id );
}

GibbsPotential::Ptr CreateSupportPotential( const GibbsField& field, const Lattice& lattice,
											unsigned int id, const std::vector<unsigned int>& variableIDs ) {
	return std::make_shared<PotentialSupport>( field, id, variableIDs );
}

GibbsPotential::Ptr CreateHeightPotential( const GibbsField& field, const Lattice& lattice,
										   unsigned int id, const std::vector<unsigned int>& variableIDs ) {
	return std::make_shared<PotentialHeight>( field, id, variableIDs, lattice );
}

GibbsPotential::Ptr CreateMassPotential( const GibbsField& field, const Lattice& lattice,
										 unsigned int id, const std::vector<unsigned int>& variableIDs ) {
	return std::make_shared<PotentialMass>( field, id, variableIDs );
}

GibbsPotential::Ptr CreateEdgePotential( const GibbsField& field, const Lattice& lattice,
										 unsigned int id, const std::vector<unsigned int>& variableIDs ) {
	return std::make_shared<PotentialEdge>( field, id, variableIDs, lattice );
}

int main() {

	// Create the assembly and constructor
	DiscreteAssembly assembly;
	AssemblyConstructor::VariableConstructor vconst =
	boost::bind( &CreateBlock, _1, _2 );
	AssemblyConstructor aconst( vconst );

	// Add the support potential slot
	// Order for support slot is top, bottom, 4 sides (in no particular order)
	std::vector<DiscretePoint3> supportPoints;
	supportPoints.emplace_back( 0, 0, 0 );
	supportPoints.emplace_back( 0, 0, 1 );
	supportPoints.emplace_back( 0, 0, -1 );
	supportPoints.emplace_back( 1, 0, 0 );
	supportPoints.emplace_back(-1, 0, 0 );
	supportPoints.emplace_back( 0, 1, 0 );
	supportPoints.emplace_back( 0, -1, 0 );
	AssemblySlot::PotentialConstructor supportConstructor =
		boost::bind( &CreateSupportPotential, _1, _2, _3, _4 );
	AssemblySlot::Ptr supportSlot =
		std::make_shared<AssemblySlot>( supportPoints, supportConstructor );
	
 	aconst.AddSlot( supportSlot );

	// Add the height potential slot
	// Unary slot only needs self
	std::vector<DiscretePoint3> heightPoints;
	heightPoints.emplace_back( 0, 0, 0 );
	AssemblySlot::PotentialConstructor heightConstructor =
		boost::bind( &CreateHeightPotential, _1, _2, _3, _4 );
	AssemblySlot::Ptr heightSlot =
		std::make_shared<AssemblySlot>( heightPoints, heightConstructor );

	aconst.AddSlot( heightSlot );

	// Add the mass unary slot
	// Unary slot only needs self
	std::vector<DiscretePoint3> massPoints;
	massPoints.emplace_back( 0, 0, 0 );
	AssemblySlot::PotentialConstructor massConstructor =
		boost::bind( &CreateMassPotential, _1, _2, _3, _4 );
	AssemblySlot::Ptr massSlot =
		std::make_shared<AssemblySlot>( massPoints, massConstructor );

	aconst.AddSlot( massSlot );
		
	// Add the border unary slot
	// Unary slot only needs self
	std::vector<DiscretePoint3> edgePoints;
	edgePoints.emplace_back( 0, 0, 0 );
	AssemblySlot::PotentialConstructor edgeConstructor =
		boost::bind( &CreateEdgePotential, _1, _2, _3, _4 );
	AssemblySlot::Ptr edgeSlot =
		std::make_shared<AssemblySlot>( edgePoints, edgeConstructor );

	aconst.AddSlot( edgeSlot );
		
	// Iterate over the cubical range and add voxels
	int xDim = 10;
	int yDim = 10;
	int zDim = 10;
	
	std::vector<DiscretePoint3> corners;
	corners.emplace_back( 0, 0, 0 );
	corners.emplace_back( 0, 0, zDim );
	corners.emplace_back( 0, yDim, 0 );
	corners.emplace_back( 0, yDim, zDim );
	corners.emplace_back( xDim, 0, 0 );
	corners.emplace_back( xDim, 0, zDim );
	corners.emplace_back( xDim, yDim, 0 );
	corners.emplace_back( xDim, yDim, zDim );
	
	DiscreteBox3 box( corners );
	DiscreteBox3::Operator addOp =
		boost::bind( &AssemblyConstructor::AddVoxel, &aconst,
					 boost::ref(assembly), _1 );
	box.Iterate( addOp );

	std::cout << "Created " << assembly.GetField().NumPotentials() << " potentials." << std::endl;

	// Add some dummy blocks to visualize
	assembly.GetBlock( 0 )->SetState( BLOCK_FULL );
	assembly.GetBlock( 1 )->SetState( BLOCK_HALF );
	assembly.GetBlock( 2 )->SetState( BLOCK_FULL );

	// Visualize the assembly
	RendererManager rman( "Output", 600, 600 );
	AssemblyVisualizer aviz( rman );

	MCMCSampler sampler;
	unsigned int sampleSize = 10;
	for( unsigned int i = 0; i < 100; i++ ) {
		sampler.Sample( assembly.GetField(), sampleSize );
		aviz.Visualize( assembly );
		usleep( 1E6 );
	}
	
	pause();

}
