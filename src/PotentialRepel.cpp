#include "intelligent/PotentialRepel.h"
#include "intelligent/BlockVariable.h"

#include <memory>

namespace intelligent {

	PotentialRepel::PotentialRepel( const GibbsField& _field, unsigned int _id, 
									const std::vector<unsigned int>& _variableIDs, 
								    const Lattice& _lattice, const ContinuousPoint3 _objPos) : GibbsPotential( _field, _id,_variableIDs ) 
	{										  
		// get node id
		unsigned int node_id = _variableIDs[0];
												  
		// send node id to lattice to get node height in lattice
		//nodeHeight = _lattice.GetNodePosition( node_id ).z - _lattice.GetBoundingBox().minZ + 1;
		//latticeHeight = _lattice.GetBoundingBox().maxZ - _lattice.GetBoundingBox().minZ + 1;
		double disX = _lattice.GetNodePosition( node_id ).x - _objPos.x;
		double disY = _lattice.GetNodePosition( node_id ).y - _objPos.y;
		double disZ = _lattice.GetNodePosition( node_id ).z - _objPos.z;	
		
		distance = std::sqrt( disX*disX + disY*disY + disZ*disZ );	
	}  
  
	PotentialRepel::PotentialRepel( const GibbsField& _field, const PotentialRepel& other ) : 
		GibbsPotential( _field, other ),
		distance( other.distance )  {}

	GibbsPotential::Ptr PotentialRepel::Clone( const GibbsField& _field ){
		std::shared_ptr<GibbsPotential> pointer( new PotentialRepel( _field, *this ) );
		return pointer;
	}
			
	double PotentialRepel::CalculatePotential(){
		
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
		
		double threshold = 3;
		
		double prob = 1;
		
		if ( me != 0 ) {
			if (distance < threshold ) {
				//prob = distance / threshold;
				prob = std::exp( -3*distance );
			}
		}
			
		return prob;
	}
				
}


