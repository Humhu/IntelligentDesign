#include "intelligent/PotentialSupport.h"
#include "intelligent/BlockVariable.h"

#include <memory>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace intelligent {

	PotentialSupport::PotentialSupport( const GibbsField& _field, unsigned int _id,
									const std::vector<unsigned int>& _variableIDs ) : 
									GibbsPotential( _field, _id,_variableIDs ) {
										
	}  
  
	PotentialSupport::PotentialSupport( const GibbsField& _field,
										const PotentialSupport& other ) :
		GibbsPotential( _field, other ) {}

	GibbsPotential::Ptr PotentialSupport::Clone( const GibbsField& _field ){
		std::shared_ptr<GibbsPotential> pointer =
			std::make_shared<PotentialSupport>( _field, *this );
		return pointer;
	}
			
	double PotentialSupport::CalculatePotential(){
		
		std::vector <GibbsVariable::Ptr> clique = GetClique() ;

		std::vector <BlockVariable::Ptr> markov_blanket( clique.size() );
		std::vector <double> blanket_val( clique.size() );
		
		for( unsigned int i = 0; i < clique.size(); i++ ) {
			markov_blanket[i] = std::dynamic_pointer_cast <BlockVariable>( clique[i] );

			BlockType type = markov_blanket[i]->GetState();
			switch( type ) {
				case BLOCK_FULL:
					blanket_val[i] = 1.0;
					break;
				case BLOCK_HALF:
					blanket_val[i] = 0.5;
					break;
				case BLOCK_EMPTY:
					blanket_val[i] = 0.0;
					break;
				default:
					throw std::runtime_error("Invalid block type!");
			}

		}
		
		// GetClique returned pointers in the order: self, top, bottom, sides.
		// so, here is some poor coding, which doesn't do any error checks. sorry :(
		// we just go through assuming there are 6 neighbors and that they are in the
		// believed order.
		double points = 0;
		bool hasFullNeighbor = false;
		
		bool leftRightSupport = blanket_val[3] == 1.0 && blanket_val[5] == 1.0;
		bool frontBackSupport = blanket_val[4] == 1.0 && blanket_val[6] == 1.0;
		bool supportedUnderneath = blanket_val[2] == 1.0;
		
		for( int i = 1; i < 7; i++ ) {
			hasFullNeighbor = hasFullNeighbor || blanket_val[i] == 1.0;
		}
		
		// if I have a block above me, I get 1*state-of-above-block points
		points = points + 3*blanket_val[1];
		
		// if I have a block underneath me, I get 3*state-of-below-block points
		points = points + 3*blanket_val[2];
		
		// for every block on my side, I get sum(20*state-of-side-block) points for each side block
		for(int i=3; i<=6; i++){
			points = points + 1*blanket_val[i];
		}
		
		// ok, those are all the points based on my neighbors. the max points are if every neighbor is 
		// a full block. then, there will be 1+3+1.25*4 = 9 points. therefore, if I score between 0 and 3 
		// I should be empty, if I score between 3 and 6 I should be half-full, and if I score between
		// 6 and 9 I should be full. SO, if I AM full and I have a high score, I should be 1. Etc.
		double me = blanket_val[0];
		double prob = 0;

		if( me == 0 ) {
// 			prob = std::exp( -3*points );
			prob = 0.5;
		}
		else if( me == 0.5 ) {
			double econst = -1.0/me;
			prob = 1 - std::exp( econst*points );
		}
		else if( me == 1.0 ) {

// 			if( !hasFullNeighbor ) {
// 				prob = 0;
// 			}
			if( blanket_val[2] == 0 ) {
				prob = 0;
			}
			else {
				double econst = -1.0/me;
				prob = 1 - std::exp( econst*points );
			}
		}

// 		std::cout << "p: " << prob << " for type " << me << " and points " << points << std::endl;
		return prob;
	}
				
}


