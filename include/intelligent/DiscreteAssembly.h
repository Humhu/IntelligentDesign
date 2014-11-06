#ifndef _DISCRETE_ASSEMBLY_H_
#define _DISCRETE_ASSEMBLY_H_

#include "intelligent/GibbsField.h"
#include "intelligent/Lattice.h"
#include "intelligent/BlockVariable.h"

namespace intelligent {

	class DiscreteAssembly {
	public:

		DiscreteAssembly();

		// TODO Figure out how to do sampling so we can return const references
		// here instead
		GibbsField& GetField();
		const GibbsField& GetField() const;

		Lattice& GetLattice();
		const Lattice& GetLattice() const;

		BlockVariable::Ptr GetBlock( unsigned int id ) const;
		
	private:

		GibbsField field;
		Lattice lattice;
		
	};
	
}

#endif

