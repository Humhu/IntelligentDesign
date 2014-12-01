#include "intelligent/DiscreteAssembly.h"

namespace intelligent {

	DiscreteAssembly::DiscreteAssembly() {}

	DiscreteAssembly::DiscreteAssembly( const DiscreteAssembly& other ) :
		field( other.field ),
		lattice( other.lattice ) {}

	GibbsField& DiscreteAssembly::GetField() {
		return field;
	}

	const GibbsField& DiscreteAssembly::GetField() const {
		return field;
	}

	Lattice& DiscreteAssembly::GetLattice() {
		return lattice;
	}

	const Lattice& DiscreteAssembly::GetLattice() const {
		return lattice;
	}
	
	BlockVariable::Ptr DiscreteAssembly::GetBlock( unsigned int id ) const {
		GibbsVariable::Ptr var = field.GetVariable( id );
		return std::dynamic_pointer_cast<BlockVariable>( var );
	}
	
}
