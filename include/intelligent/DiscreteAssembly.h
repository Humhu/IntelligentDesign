#ifndef _DISCRETE_ASSEMBLY_H_
#define _DISCRETE_ASSEMBLY_H_

#include "intelligent/GibbsField.h"
#include "intelligent/Lattice.h"

namespace intelligent {

	/*! \brief A discrete assembly of a specified size */
	class DiscreteAssembly {
	public:
		DiscreteAssembly();
	
		DiscreteAssembly(int _x, int _y, int _z);
		
		GibbsField & GetGibbsField();
		
		Lattice & GetLattice();
	
	private:
		void MakeSize(int _x, int _y, int _z);
	
		GibbsField field;
		Lattice lattice;
	};

}

#endif
