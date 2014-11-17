#ifndef _POTENTIAL_SUPPORT_
#define _POTENTIAL_SUPPORT_
#include "intelligent/GibbsField.h"

namespace intelligent {
	
	// we will order the id's so that the markov blanket comes in ordered as:
	// self, top, bottom, 4 sides

	class PotentialSupport : public GibbsPotential {
	public:

		typedef std::shared_ptr<PotentialSupport> Ptr;
		
		PotentialSupport( const GibbsField& _field, unsigned int _id,
						  const std::vector<unsigned int>& _variableIDs );

		PotentialSupport( const GibbsField& _field, const GibbsPotential& other );

		virtual GibbsPotential::Ptr Clone( const GibbsField& _field );
		
		virtual double CalculatePotential();
		
	};

}

#endif