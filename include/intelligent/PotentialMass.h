#ifndef _POTENTIAL_MASS_
#define _POTENTIAL_MASS_

#include "intelligent/BlockVariable.h"
#include "intelligent/GibbsField.h"

namespace intelligent {

	/*! \brief Penalizes an assembly for having large mass. */
	class PotentialMass : public GibbsPotential {
	public:

		typedef std::shared_ptr<PotentialMass> Ptr;
		
		PotentialMass( const GibbsField& _field, unsigned int _id,
					   const std::vector<unsigned int>& _variableIDs,
					   double _massCoeff, double _maxMass );

		PotentialMass( const GibbsField& _field, const PotentialMass& other );

		virtual GibbsPotential::Ptr Clone( const GibbsField& _field );
		
		virtual double CalculatePotential();

	private:

		double massCoeff;
		double maxMass;
		
	};

}

#endif