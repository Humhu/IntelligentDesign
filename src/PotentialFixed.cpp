#include "intelligent/PotentialFixed.h"

namespace intelligent {

	PotentialFixed::PotentialFixed( const GibbsField& _field, unsigned int _id,
									const std::vector<unsigned int>& _variableIDs,
									BlockType fix ) :
		GibbsPotential( _field, _id, _variableIDs ),
		fixType( fix ) {}

	PotentialFixed::PotentialFixed( const GibbsField& _field, const PotentialFixed& other ) :
		GibbsPotential( _field, other ),
		fixType( other.fixType ) {}

	GibbsPotential::Ptr PotentialFixed::Clone( const GibbsField& _field ) {
		GibbsPotential::Ptr pointer =
			std::make_shared<PotentialFixed>( _field, *this );
		return pointer;
	}

	double PotentialFixed::CalculatePotential() {

		std::vector<GibbsVariable::Ptr> clique = GetClique();
		BlockVariable::Ptr block = std::dynamic_pointer_cast<BlockVariable>( clique[0] );

		if( block->GetState() == fixType ) {
			return 1.0;
		}
		return 0.0;
	}
	
}