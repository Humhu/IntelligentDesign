#include "intelligent/PotentialSupport.h"
#include "intelligent/BlockVariable.h"
#include <memory>

namespace intelligent {

	PotentialSupport::PotentialSupport( const GibbsField& _field, unsigned int _id,
									const std::vector<unsigned int>& _variableIDs ) : 
									GibbsPotential( _field, _id,_variableIDs ) {
	}  
  
	PotentialSupport::PotentialSupport( const GibbsField& _field, const GibbsPotential& other ) : 
									GibbsPotential( _field, other ) {
	}

	GibbsPotential::Ptr PotentialSupport::Clone( const GibbsField& _field ){
		std::shared_ptr<GibbsPotential> pointer( new PotentialSupport( _field, *this ) );
		return pointer;
	}
			
	double PotentialSupport::CalculatePotential(){
		
		std::vector < GibbsVariable :: Ptr > clique = GetClique() ;

		std::vector < BlockVariable :: Ptr > markov_blanket( clique.size() );
		std::vector < double > blanket_val( clique.size() );
		for(unsigned int i=0; i<clique.size(); i++) { //(int n=10; n>0; n--)
			markov_blanket[i] = std::dynamic_pointer_cast <BlockVariable>( clique[i] );
			if ( markov_blanket[i]->GetState() == BLOCK_FULL ) { blanket_val[i] = 1; }
			else if ( markov_blanket[i]->GetState() == BLOCK_THIN ) { blanket_val[i] = .5; }
			else { blanket_val[i] = 0; }
		}
		
		// GetClique returned pointers in the order: top, bottom, sides.
		// so, here is some poor coding, which doesn't do any error checks. sorry :(
		// we just go through assuming there are 6 neighbors and that they are in the
		// believed order.
		double points = 0;
		
		// if I have a block above me, I get 1*state-of-above-block points
		points = points + blanket_val[0];
		
		// if I have a block underneath me, I get 100*state-of-below-block points
		points = points + 100*blanket_val[1];
		
		// for every block on my side, I get sum(20*state-of-side-block) points for each side block
		for(int i=2; i<5; i++){
			points = points + 20*blanket_val[i];
		}
		
		// then, the potential returned is exp(sum-of-all-points)
		return points;
	}
				
}


