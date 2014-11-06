#include "intelligent/DiscreteAssembly.h"

#include "intelligent/BlockVariable.h"

namespace intelligent {
	DiscreteAssembly::DiscreteAssembly() {
		MakeSize(0, 0, 0);
	}
	
	DiscreteAssembly::DiscreteAssembly(int _x, int _y, int _z) {
		MakeSize(_x, _y, _z);
	}
	
	void DiscreteAssembly::MakeSize(int _x, int _y, int _z) {
		GibbsField newField;
		Lattice newLattice;
		DiscretePoint3 pt3;
		unsigned int id = 0;
		
		for (pt3.x=0; pt3.x<_x; pt3.x++) {
			for (pt3.y=0; pt3.y<_y; pt3.y++) {
				for (pt3.z=0; pt3.z<_z; pt3.z++) {
					auto var = std::make_shared<BlockVariable>(newField, id);
					newField.AddVariable(var);
					newLattice.AddNode(id, pt3); 
					id++;
				}
			}
		}
		
		field = newField;
		lattice = newLattice;
	}
	
	GibbsField & DiscreteAssembly::GetGibbsField() {
		return field;
	}
	
	Lattice & DiscreteAssembly::GetLattice() {
		return lattice;
	}
}
