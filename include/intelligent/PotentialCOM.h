#ifndef _POTENTIAL_COM_
#define _POTENTIAL_COM_
#include "intelligent/GibbsField.h"
#include "intelligent/Lattice.h"
#include "intelligent/DiscretePoint.h"

namespace intelligent {

	class PotentialCOM : public GibbsPotential {
	public:

		PotentialCOM(const GibbsField& _field, unsigned int _id,
			const std::vector<unsigned int> & _vids, 
			const Lattice & _lattice);

		PotentialCOM(const GibbsField & _field, const GibbsPotential & other);

		virtual GibbsPotential::Ptr Clone(const GibbsField& _field);
		
		virtual double CalculatePotential();
		
		void SetDesiredCOM(const DiscretePoint3 & _com);
		
		const Lattice * lattice;
		DiscretePoint3 desiredCOM;
	};

}

#endif
