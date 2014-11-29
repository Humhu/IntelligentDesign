#ifndef _POTENTIAL_HEIGHT_
#define _POTENTIAL_HEIGHT_

#include "intelligent/GibbsField.h"
#include "intelligent/Lattice.h"

namespace intelligent {
	
	// we will order the id's so that the markov blanket comes in ordered as:
	// self, top, bottom, 4 sides

	class PotentialHeight : public GibbsPotential {
	public:

		typedef std::shared_ptr<PotentialHeight> Ptr;
		
		PotentialHeight( const GibbsField& _field, unsigned int _id,
					   const std::vector<unsigned int>& _variableIDs,
					   const Lattice& _lattice );

		PotentialHeight( const GibbsField& _field, const PotentialHeight& other );

		virtual GibbsPotential::Ptr Clone( const GibbsField& _field );
		
		virtual double CalculatePotential();

	private:
		
		int nodeHeight;
		int latticeHeight;

	};

}

#endif