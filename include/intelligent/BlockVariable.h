#ifndef _BLOCK_VARIABLE_H_
#define _BLOCK_VARIABLE_H_
#include "intelligent/GibbsField.h"


namespace intelligent {

	enum BlockType {
		BLOCK_EMPTY,
		BLOCK_FULL,
		BLOCK_THIN
	};

	class BlockVariable : public GibbsVariable {
	public:
		
			typedef std::shared_ptr<BlockVariable> Ptr;

			virtual GibbsVariable::Ptr Clone( const GibbsField& _field );
			
			virtual void Sample();

			BlockVariable( const GibbsField& _field, unsigned int id );
			
			BlockVariable( const GibbsField& _field, const BlockVariable& other );	

			BlockType GetState();
			

	private:
		
			BlockType state;

	};

}

#endif