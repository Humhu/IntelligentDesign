#include "intelligent/AssemblyConstructor.h"
#include "intelligent/AssemblyVisualizer.h"
#include "intelligent/RendererManager.h"
#include "intelligent/DiscreteAssembly.h"
#include "intelligent/MCMCSampler.h"
#include "intelligent/AssemblySampler.h"
#include "intelligent/TreeSearch.h"

// Potentials
#include "intelligent/PotentialSupport.h"
#include "intelligent/PotentialHeight.h"
#include "intelligent/PotentialMass.h"
#include "intelligent/PotentialEdge.h"
#include "intelligent/PotentialCOM.h"
#include "intelligent/PotentialFixed.h"
#include "intelligent/PotentialRepel.h"

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

GibbsPotential::Ptr CreateRepelPotential( const GibbsField& field, const Lattice& lattice,
										  unsigned int id, const std::vector<unsigned int>& variableIDs, 
										  const ContinuousPoint3 objPos ) {
	return std::make_shared<PotentialRepel>( field, id, variableIDs, lattice, objPos );
}

GibbsPotential::Ptr CreateEdgePotential( const GibbsField& field, const Lattice& lattice,
										 unsigned int id, const std::vector<unsigned int>& variableIDs ) {
	return std::make_shared<PotentialEdge>( field, id, variableIDs, lattice );
}

GibbsPotential::Ptr CreateCOMPotential( const GibbsField& field, const Lattice& lattice,
										unsigned int id, const std::vector<unsigned int>& variableIDs,
										ContinuousPoint3 com ) {
	PotentialCOM::Ptr pot =
		std::make_shared<PotentialCOM>( field, id, variableIDs, lattice );
	pot->SetDesiredCOM( com );
	return pot;
}

GibbsPotential::Ptr CreateMassPotential( const GibbsField& field, const Lattice& lattice,
										 unsigned int id, const std::vector<unsigned int>& variableIDs,
										 double massCoeff, double maxMass ) {
	return std::make_shared<PotentialMass>( field, id, variableIDs, massCoeff, maxMass );
}

void AddCOMPoint( std::vector<DiscretePoint3>& pts, const DiscretePoint3& p ) {
	pts.push_back(p);
}

void AddInteriorPoint( std::vector<unsigned int>& inds, const Lattice& lattice,
					   const DiscretePoint3& p ) {
	DiscreteBox3 bounds = lattice.GetBoundingBox();
	if( p.x == bounds.minX || p.x == bounds.maxX
		|| p.y == bounds.minY || p.y == bounds.maxY
		|| p.z == bounds.maxZ ) {
		return;
	}
	std::cout << p << std::endl;
	inds.push_back( lattice.GetNodeID( p ) );
}

int main() {

	// Create the assembly and constructor
	DiscreteAssembly::Ptr assembly = std::make_shared<DiscreteAssembly>();
	AssemblyConstructor::VariableConstructor vconst =
		boost::bind( &CreateBlock, _1, _2 );
	AssemblyConstructor aconst( vconst );

	// Add the support potential slot
	// Order for support slot is top, bottom, 4 sides (in no particular order)
	std::vector<DiscretePoint3> supportPoints(7);
	supportPoints[0] = DiscretePoint3( 0, 0, 0 );
	supportPoints[1] = DiscretePoint3( 0, 0, 1 );
	supportPoints[2] = DiscretePoint3( 0, 0, -1 );
	supportPoints[3] = DiscretePoint3( 1, 0, 0 );
	supportPoints[4] = DiscretePoint3( -1, 0, 0 );
	supportPoints[5] = DiscretePoint3( 0, 1, 0 );
	supportPoints[6] = DiscretePoint3( 0, -1, 0 );
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

// 	aconst.AddSlot( heightSlot );

	// Add the border unary slot
	// Unary slot only needs self
	std::vector<DiscretePoint3> edgePoints;
	edgePoints.emplace_back( 0, 0, 0 );
	AssemblySlot::PotentialConstructor edgeConstructor =
		boost::bind( &CreateEdgePotential, _1, _2, _3, _4 );
	AssemblySlot::Ptr edgeSlot =
		std::make_shared<AssemblySlot>( edgePoints, edgeConstructor );

// 	aconst.AddSlot( edgeSlot );

	// Add a repel slot
	// Unary slot only needs self
	ContinuousPoint3 objPos( 2, 2, 4 );
	std::vector<DiscretePoint3> repelPoints;
	repelPoints.emplace_back( 0, 0, 0 );
	AssemblySlot::PotentialConstructor repelConstructor =
		boost::bind( &CreateRepelPotential, _1, _2, _3, _4, objPos);
	AssemblySlot::Ptr repelSlot =
		std::make_shared<AssemblySlot>( repelPoints, repelConstructor );

// 	aconst.AddSlot( repelSlot );

	
	// Lattice range
	int xDim = 30;
	int yDim = 2;
	int zDim = 30;
	
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
	
	// Get global IDs for global potentials
	std::vector<DiscretePoint3> allPoints;
	DiscreteBox3::Operator pushOp =
		boost::bind( &AddCOMPoint, boost::ref(allPoints), _1 );
	box.Iterate( pushOp );
	
	DiscreteBox3::Operator addOp =
		boost::bind( &AssemblyConstructor::AddVoxel, &aconst,
					 boost::ref(*assembly), _1 );
	box.Iterate( addOp );
	
	// Once voxels are added, add potentials
	aconst.BuildPotentials( *assembly );

	// Add a fix potential for the seed block manually
	unsigned int seedID = assembly->GetLattice().GetNodeID( DiscretePoint3( 1, 1, 0 ) );
	GibbsVariable::Ptr seedVar = assembly->GetField().GetVariable( seedID );
	std::vector<unsigned int> seedClique;
	seedClique.push_back( seedID );
	PotentialFixed::Ptr fixPot =
		std::make_shared<PotentialFixed>( assembly->GetField(),
										  assembly->GetField().NumPotentials(),
										  seedClique, BLOCK_FULL );
	
	seedVar->AddPotential( fixPot->id );
	assembly->GetField().AddPotential( fixPot );
	std::dynamic_pointer_cast<BlockVariable>( seedVar )->SetState( BLOCK_FULL );
	
	std::cout << "Created " << assembly->GetField().NumPotentials() << " potentials." << std::endl;

	// Visualize the assembly
	RendererManager rman( "Output", 600, 600 );
	AssemblyVisualizer aviz( rman );
	
	// Pull indices for non-edge blocks
	std::vector<unsigned int> interiorIDs;
	DiscreteBox3::Operator intOp =
		boost::bind( &AddInteriorPoint, boost::ref(interiorIDs),
					 boost::ref(assembly->GetLattice()), _1 );
	box.Iterate( intOp );

	std::cout << interiorIDs.size() << std::endl;
	
	MCMCSampler mcmcSampler;
	mcmcSampler.SetIndexSet( interiorIDs );
	AssemblySampler aSampler( mcmcSampler );
	aSampler.SetBase( assembly );
	
	unsigned int sampleDepth = 10;
	unsigned int sampleCounter = 0;

	aviz.Visualize( *assembly );

	TreeSearch tsearch( aSampler );
	
// 	while(true) {
// 		std::vector<DiscreteAssembly::Ptr> samples = aSampler.Sample( 1, sampleDepth );
// 		assembly = samples[0];
// 		aSampler.SetBase( assembly );
// 
// 		sampleCounter++;
// 		std::cout << "Generated " << sampleCounter << " samples" << std::endl;
// 
// 		if( sampleCounter % 100 == 0 ) {
// 			aviz.Visualize( *assembly );
// 			rman.RequestScreenshot();
// 		}
// 		
// 		usleep( 1E5 );
// 	}

	tsearch.SetNumSuccessors( 5 );
	tsearch.SetSampleDepth( 50 );
	tsearch.Add( assembly );
	tsearch.SetMaxQueueSize( 1000 );
	
	while( true ) {

		DiscreteAssembly::Ptr best = tsearch.Next();

		sampleCounter++;
		std::cout << "# successors: " << tsearch.Size() << std::endl;
		std::cout << "Generated " << sampleCounter << " successors" << std::endl;
		if( sampleCounter % 10 == 0 ) {
			aviz.Visualize( *best );
		}

		usleep( 1E5 );
		
	}
	
	pause();

}
