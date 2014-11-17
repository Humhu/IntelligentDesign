#ifndef _POTENTIAL_COM_
#define _POTENTIAL_COM_
#include "intelligent/GibbsField.h"
#include "intelligent/Lattice.h"
#include "intelligent/DiscretePoint.h"

namespace intelligent {

	struct ContinuousPoint3 {
		double x;
		double y;
		double z;

		ContinuousPoint3();
		ContinuousPoint3( double _x, double _y, double _z );
	};
	
	class PotentialCOM : public GibbsPotential {
	public:

		typedef std::shared_ptr<PotentialCOM> Ptr;
		
		PotentialCOM(const GibbsField& _field, unsigned int _id,
			const std::vector<unsigned int> & _vids, 
			const Lattice & _lattice);

		PotentialCOM(const GibbsField & _field, const GibbsPotential & other);

		virtual GibbsPotential::Ptr Clone(const GibbsField& _field);
		
		virtual double CalculatePotential();
		
		void SetDesiredCOM(const ContinuousPoint3& _com);
		
		const Lattice * lattice;
		ContinuousPoint3 desiredCOM;

	private:

		ContinuousPoint3 CalculateCOM();
		
	};

}

#endif
