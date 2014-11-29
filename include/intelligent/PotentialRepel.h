#ifndef _POTENTIAL_REPEL_
#define _POTENTIAL_REPEL_
#include "intelligent/GibbsField.h"
#include "intelligent/Lattice.h"
#include "intelligent/PotentialCOM.h"


namespace intelligent {
	
	// we will order the id's so that the markov blanket comes in ordered as:
	// self, top, bottom, 4 sides

	class PotentialRepel : public GibbsPotential {
	public:

		typedef std::shared_ptr<PotentialRepel> Ptr;
		
		PotentialRepel( const GibbsField& _field, unsigned int _id,
					   const std::vector<unsigned int>& _variableIDs,
					   const Lattice& _lattice, const ContinuousPoint3 _objPos );

		PotentialRepel( const GibbsField& _field, const PotentialRepel& other );

		virtual GibbsPotential::Ptr Clone( const GibbsField& _field );
		
		virtual double CalculatePotential();
		
		double distance;

	};

}

#endif