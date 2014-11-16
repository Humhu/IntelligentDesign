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
			else if ( markov_blanket[i]->GetState() == BLOCK_HALF ) { blanket_val[i] = .5; }
			else { blanket_val[i] = 0; }
		}
		
		// GetClique returned pointers in the order: self, top, bottom, sides.
		// so, here is some poor coding, which doesn't do any error checks. sorry :(
		// we just go through assuming there are 6 neighbors and that they are in the
		// believed order.
		double points = 0;
			
		// if I have a block above me, I get 1*state-of-above-block points
		points = points + blanket_val[1];
		
		// if I have a block underneath me, I get 100*state-of-below-block points
		points = points + 3*blanket_val[2];
		
		// for every block on my side, I get sum(20*state-of-side-block) points for each side block
		for(int i=3; i<6; i++){
			points = points + 1.25*blanket_val[i];
		}
		
		// ok, those are all the points based on my neighbors. the max points are if every neighbor is 
		// a full block. then, there will be 1+3+1.25*4 = 9 points. therefore, if I score between 0 and 3 
		// I should be empty, if I score between 3 and 6 I should be half-full, and if I score between
		// 6 and 9 I should be full. SO, if I AM full and I have a high score, I should be 1. Etc.
		double me = blanket_val[0];
		double prob = 0;
		if ( me == 1 ) {
			if (points >= 6) 			   {prob = 1;}
			if (points < 6 && points >= 3) {prob = .5;}
			if (points < 3 && points >= 0) {prob = 0;}    }
		else if ( me == .5 ) {
			if (points >= 6) 			   {prob = .5;}
			if (points < 6 && points >= 3) {prob = 1;}
			if (points < 3 && points >= 0) {prob = .5;}   }
		else {
			if (points >= 6) 			   {prob = 0;}
			if (points < 6 && points >= 3) {prob = .5;}
			if (points < 3 && points >= 0) {prob = 1;}    }	
			
		// then, the potential returned is exp(sum-of-all-points)
		return prob;
	}
				
}


