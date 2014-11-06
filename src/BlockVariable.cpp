#include "intelligent/BlockVariable.h"

namespace intelligent {

	BlockVariable::BlockVariable( const GibbsField& _field, unsigned int _id ) : GibbsVariable( _field, _id ) {
		state = BLOCK_EMPTY;
	}  
  
	BlockVariable::BlockVariable( const GibbsField& _field, const BlockVariable& other ) : GibbsVariable( _field, other ) {
		state = other.state;
	}

	GibbsVariable::Ptr BlockVariable::Clone( const GibbsField& _field ){
		std::shared_ptr<GibbsVariable> pointer( new BlockVariable( _field, *this ) );
		return pointer;
	}
			
	void BlockVariable::Sample(){
				
	}
	
	BlockType BlockVariable::GetState(){
		return state;
	}
			
}
