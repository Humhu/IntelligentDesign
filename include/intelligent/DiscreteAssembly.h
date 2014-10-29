#ifndef _DISCRETE_ASSEMBLY_H_
#define _DISCRETE_ASSEMBLY_H_

#include "intelligent/GibbsField.h"
#include "intelligent/Lattice.h"

namespace intelligent {

	/*! \brief A discrete assembly consists of a Gibbs field describing the state
	 * of all components in the assembly, and a Lattice describing the spatial
	 * arrangement of those components. */
	class DiscreteAssembly {
	public:

		DiscreteAssembly(  );

	private:

		GibbsField field;
		Lattice lattice;
		
	};
	
}

#endif