#include "intelligent/PotentialMass.h"
#include "intelligent/BlockVariable.h"
#include <memory>

namespace intelligent {

	PotentialMass::PotentialMass( const GibbsField& _field, unsigned int _id,
									const std::vector<unsigned int>& _variableIDs ) : 
									GibbsPotential( _field, _id,_variableIDs ) {
	}  
  
	PotentialMass::PotentialMass( const GibbsField& _field, const GibbsPotential& other ) : 
									GibbsPotential( _field, other ) {
	}

	GibbsPotential::Ptr PotentialMass::Clone( const GibbsField& _field ){
		std::shared_ptr<GibbsPotential> pointer( new PotentialMass( _field, *this ) );
		return pointer;
	}
			
	double PotentialMass::CalculatePotential(){
		
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
		if ( me == 1 ) {prob = 0.5;} 
		else if ( me == .5 ) {prob = 0.75;}
		else {prob = 1;} 
			
		return prob;
	}
				
}


