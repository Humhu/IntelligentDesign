#ifndef _POTENTIAL_EDGE_
#define _POTENTIAL_EDGE_
#include "intelligent/GibbsField.h"
#include "intelligent/Lattice.h"


namespace intelligent {
	
	// we will order the id's so that the markov blanket comes in ordered as:
	// self, top, bottom, 4 sides

	class PotentialEdge : public GibbsPotential {
	public:

		typedef std::shared_ptr<PotentialEdge> Ptr;
		
		PotentialEdge( const GibbsField& _field, unsigned int _id,
					   const std::vector<unsigned int>& _variableIDs,
					   const Lattice& _lattice );

		PotentialEdge( const GibbsField& _field, const PotentialEdge& other );

		virtual GibbsPotential::Ptr Clone( const GibbsField& _field );
		
		virtual double CalculatePotential();
		
		bool edge;
	};

}

#endif