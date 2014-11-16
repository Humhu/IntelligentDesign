#include "intelligent/PotentialEdge.h"
#include "intelligent/BlockVariable.h"
#include <memory>

namespace intelligent {

	PotentialEdge::PotentialEdge( const GibbsField& _field, unsigned int _id,
									const std::vector<unsigned int>& _variableIDs,
									const Lattice& _lattice ) : 
									GibbsPotential( _field, _id,_variableIDs ) {

		// get node id
		unsigned int node_id = _variableIDs[0];
												  
		// send node id to lattice to get node x,y in lattice
		int nodeX = _lattice.GetNodePosition( node_id ).x;
		int nodeY = _lattice.GetNodePosition( node_id ).y;
		
		bool xEdge = false;
		if (nodeX == _lattice.GetBoundingBox().maxX && nodeX == _lattice.GetBoundingBox().minX) {
			xEdge = true;			
		}
		else {xEdge = false;}
		
		bool yEdge = false;
		if (nodeY == _lattice.GetBoundingBox().maxY && nodeY == _lattice.GetBoundingBox().minY) {
			yEdge = true;			
		}
		else {yEdge = false;}
										
		edge = xEdge || yEdge;
		
	}  
  
	PotentialEdge::PotentialEdge( const GibbsField& _field, const GibbsPotential& other ) : 
									GibbsPotential( _field, other ) {
	}

	GibbsPotential::Ptr PotentialEdge::Clone( const GibbsField& _field ){
		std::shared_ptr<GibbsPotential> pointer( new PotentialEdge( _field, *this ) );
		return pointer;
	}
			
	double PotentialEdge::CalculatePotential(){
			
		std::vector < GibbsVariable :: Ptr > clique = GetClique() ;

		std::vector < BlockVariable :: Ptr > markov_blanket( clique.size() );
		std::vector < double > blanket_val( clique.size() );
		for(unsigned int i=0; i<clique.size(); i++) { //(int n=10; n>0; n--)
			markov_blanket[i] = std::dynamic_pointer_cast <BlockVariable>( clique[i] );
			if ( markov_blanket[i]->GetState() == BLOCK_FULL ) { blanket_val[i] = 1; }
			else if ( markov_blanket[i]->GetState() == BLOCK_HALF ) { blanket_val[i] = .5; }
			else { blanket_val[i] = 0; }
		}
		
		double me = blanket_val[0];
		double prob = 0;
		if (edge) {
			if (me == 0) {prob = 1;}
			else {prob = 0;}
		}	
		else {prob = 1;}
			
		return prob;
	}
				
}


