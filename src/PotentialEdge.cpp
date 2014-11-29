#include "intelligent/PotentialEdge.h"
#include "intelligent/BlockVariable.h"
#include <memory>
#include <iostream>

namespace intelligent {

	PotentialEdge::PotentialEdge( const GibbsField& _field, unsigned int _id,
									const std::vector<unsigned int>& _variableIDs,
									const Lattice& _lattice ) : 
									GibbsPotential( _field, _id,_variableIDs ) {

		// get node id
		unsigned int node_id = _variableIDs[0];
												  
		// send node id to lattice to get node x,y in lattice
		DiscretePoint3 pos = _lattice.GetNodePosition( node_id );
		int nodeX = pos.x;
		int nodeY = pos.y;
		int nodeZ = pos.z;

		DiscreteBox3 bounds = _lattice.GetBoundingBox();
		
		bool xEdge = false;
		if (nodeX == bounds.maxX || nodeX == bounds.minX) {
			xEdge = true;			
		}
		else {xEdge = false;}
		
		bool yEdge = false;
		if (nodeY == bounds.maxY || nodeY == bounds.minY) {
			yEdge = true;			
		}
		else {yEdge = false;}

		bool zEdge = false;
		if( nodeZ == bounds.maxZ ) {
			zEdge = true;
		}
		
		edge = xEdge || yEdge || zEdge;
		
	}  
  
	PotentialEdge::PotentialEdge( const GibbsField& _field, const PotentialEdge& other ) :
		GibbsPotential( _field, other ),
		edge( other.edge ) {}

	GibbsPotential::Ptr PotentialEdge::Clone( const GibbsField& _field ){
		std::shared_ptr<GibbsPotential> pointer =
			std::make_shared<PotentialEdge>( _field, *this );
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
		double prob = 1;
		if (edge && me != 0) {
			prob = 0;
			std::cout << "Edge!" << std::endl;
		}
		
		return prob;
	}
				
}


