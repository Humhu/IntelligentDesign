#ifndef _POTENTIAL_FIXED_H_
#define _POTENTIAL_FIXED_H_

#include "intelligent/GibbsField.h"
#include "intelligent/BlockVariable.h"

namespace intelligent {

	class PotentialFixed : public GibbsPotential {
	public:
		
		typedef std::shared_ptr<PotentialFixed> Ptr;

		PotentialFixed( const GibbsField& _field, unsigned int _id,
						const std::vector<unsigned int>& _variableIDs,
						BlockType fix );

		PotentialFixed( const GibbsField& _field, const PotentialFixed& other );

		virtual GibbsPotential::Ptr Clone( const GibbsField& _field );

		virtual double CalculatePotential();
		
	private:

		BlockType fixType;
	};
	
}

#endif