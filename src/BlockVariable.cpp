#include "intelligent/BlockVariable.h"
#include "intelligent/RandomDistributions.h"

#include <sstream>
#include <iostream>

namespace intelligent {

	BlockVariable::BlockVariable( const GibbsField& _field, unsigned int _id ) : GibbsVariable( _field, _id ) {
		state = BLOCK_EMPTY;
	}  
  
	BlockVariable::BlockVariable( const GibbsField& _field, const BlockVariable& other ) :
		GibbsVariable( _field, other ) {
			
		state = other.state;
	}

	GibbsVariable::Ptr BlockVariable::Clone( const GibbsField& _field ){
		std::shared_ptr<GibbsVariable> pointer( new BlockVariable( _field, *this ) );
		return pointer;
	}
			
	void BlockVariable::Sample( double rng ){

		std::vector<double> potentials(3);
		state = BLOCK_EMPTY;
		potentials[0] = CalculatePotential();
		state = BLOCK_HALF;
		potentials[1] = CalculatePotential();
		state = BLOCK_FULL;
		potentials[2] = CalculatePotential();

		unsigned int ind = SampleNumberLine( potentials, rng );
		if( ind == 0 ) {
			state = BLOCK_EMPTY;
		}
		else if( ind == 1 ) {
			state = BLOCK_HALF;
		}
		else if( ind == 2 ) {
			state = BLOCK_FULL;
		}
		else {
			std::stringstream ss;
			ss << "Received invalid sample index of " << ind << std::endl;
			throw std::runtime_error( ss.str() );
		}
	}

	void BlockVariable::SetState( BlockType _state ) {
		state = _state;
	}
	
	BlockType BlockVariable::GetState() const {
		return state;
	}
			
}
