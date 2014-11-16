#include "intelligent/PotentialHeight.h"
#include "intelligent/BlockVariable.h"
#include <memory>

namespace intelligent {

	PotentialHeight::PotentialHeight( const GibbsField& _field, unsigned int _id,
									  const std::vector<unsigned int>& _variableIDs, const Lattice& _lattice ) : GibbsPotential( _field, _id,_variableIDs ) {
										  
		// get node id
		unsigned int node_id = _variableIDs[0];
												  
		// send node id to lattice to get node height in lattice
		nodeHeight = _lattice.GetNodePosition( node_id ).z;
		latticeHeight = _lattice.GetBoundingBox().maxZ - _lattice.GetBoundingBox().minZ + 1;
		
	}  
  
	PotentialHeight::PotentialHeight( const GibbsField& _field, const GibbsPotential& other ) : 
									  GibbsPotential( _field, other ) {
	}

	GibbsPotential::Ptr PotentialHeight::Clone( const GibbsField& _field ){
		std::shared_ptr<GibbsPotential> pointer( new PotentialHeight( _field, *this ) );
		return pointer;
	}
			
	double PotentialHeight::CalculatePotential(){
		
		std::vector < GibbsVariable :: Ptr > clique = GetClique() ;

		std::vector < BlockVariable :: Ptr > markov_blanket( clique.size() );
		std::vector < double > blanket_val( clique.size() );
		// for(unsigned int i=0; i<clique.size(); i++) { 
			markov_blanket[0] = std::dynamic_pointer_cast <BlockVariable>( clique[0] );
			if ( markov_blanket[0]->GetState() == BLOCK_FULL ) { blanket_val[0] = 1; }
			else if ( markov_blanket[0]->GetState() == BLOCK_HALF ) { blanket_val[0] = .5; }
			else { blanket_val[0] = 0; }
		//}
		
		// GetClique returned pointers in the order: self, top, bottom, sides.
		// here, we only look at the value for ourself
		double me = blanket_val[0];
		
		// we want to reward low weights.
		double prob = 0;
		if ( me == 0 ) {prob = 0.01;}
		else {prob = nodeHeight/latticeHeight;} 
			
		return prob;
	}
				
}


