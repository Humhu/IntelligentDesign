#ifndef _DISCRETE_COMPONENT_H_
#define _DISCRETE_COMPONENT_H_

namespace intelligent {

	/*! \brief The different types of discrete blocks. */
	enum BlockType {
		BLOCK_EMPTY, 	// Completely empty voxel
		BLOCK_FULL,  	// Completely filled voxel
		BLOCK_THIN		// Thin middle column voxel
	};
	
	class DiscreteComponent : public GibbsVariable {
	public:

		BlockVariable::BlockVariable( const GibbsField& _field,
									  unsigned int id ) :
			GibbsVariable( _field, id ) {}
		
	};
	
}

#endif