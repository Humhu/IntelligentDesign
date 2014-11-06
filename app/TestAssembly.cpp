#include "intelligent/AssemblyConstructor.h"
#include "intelligent/AssemblyVisualizer.h"
#include "intelligent/RendererManager.h"
#include "intelligent/DiscreteAssembly.h"

using namespace intelligent;

GibbsVariable::Ptr CreateBlock( const GibbsField& field, unsigned int id ) {
	return std::make_shared<BlockVariable>( field, id );
}

int main() {

	AssemblyConstructor::VariableConstructor vconst =
		boost::bind( &CreateBlock, _1, _2 );
	AssemblyConstructor aconst( vconst );

	int xDim = 10;
	int yDim = 10;
	int zDim = 10;
	
	DiscreteAssembly assembly;
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
	GibbsField& field = assembly.GetField();
	Lattice& lattice = assembly.GetLattice();
	
	DiscreteBox3::Operator addOp =
		boost::bind( &AssemblyConstructor::AddVoxel, &aconst,
					 boost::ref(assembly), _1 );
	box.Iterate( addOp );

	assembly.GetBlock( 0 )->SetState( BLOCK_FULL );
	assembly.GetBlock( 1 )->SetState( BLOCK_HALF );
	assembly.GetBlock( 2 )->SetState( BLOCK_FULL );

	RendererManager rman( "Output", 600, 600 );
	AssemblyVisualizer aviz( rman );
	aviz.Visualize( assembly );
	
	pause();

}
