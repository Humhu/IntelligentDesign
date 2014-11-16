#ifndef _POTENTIAL_MASS_
#define _POTENTIAL_MASS_
#include "intelligent/GibbsField.h"


namespace intelligent {
	
	// we will order the id's so that the markov blanket comes in ordered as:
	// self, top, bottom, 4 sides

	class PotentialMass : public GibbsPotential {
	public:

		PotentialMass( const GibbsField& _field, unsigned int _id,
						  const std::vector<unsigned int>& _variableIDs );

		PotentialMass( const GibbsField& _field, const GibbsPotential& other );

		virtual GibbsPotential::Ptr Clone( const GibbsField& _field );
		
		virtual double CalculatePotential();

	};

}

#endif