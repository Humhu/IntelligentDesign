#include "intelligent/PotentialMass.h"
#include "intelligent/BlockVariable.h"

#include <memory>
#include <cmath>
#include <iostream>

namespace intelligent {

	PotentialMass::PotentialMass( const GibbsField& _field, unsigned int _id,
								  const std::vector<unsigned int>& _variableIDs,
								  double _massCoeff, double _maxMass ) :
		GibbsPotential( _field, _id,_variableIDs ),
		massCoeff( _massCoeff ),
		maxMass( _maxMass ) {

		std::cout << "Constructing mass potential." << std::endl;
	}
  
	PotentialMass::PotentialMass( const GibbsField& _field, const PotentialMass& other ) :
		GibbsPotential( _field, other ),
		massCoeff( other.massCoeff ),
		maxMass( other.maxMass ) {}

	GibbsPotential::Ptr PotentialMass::Clone( const GibbsField& _field ) {
		std::shared_ptr<GibbsPotential> pointer =
			std::make_shared<PotentialMass>( _field, *this );
		return pointer;
	}
			
	double PotentialMass::CalculatePotential() {

		// Get summed mass over whole clique (assembly)
		std::vector <GibbsVariable::Ptr> clique = GetClique();

		double totalMass = 0;
		for(unsigned int i = 0; i < clique.size(); i++) {

			BlockType type = std::dynamic_pointer_cast<BlockVariable>( clique[i] )->GetState();

			switch( type ) {
				case BLOCK_FULL:
					totalMass += 1.0;
					break;
				case BLOCK_HALF:
					totalMass += 0.5;
					break;
				default:
					break;
			}
		}

		double p = 1.0;
		// we want to reward low masses
		if( totalMass > maxMass ) {
			p = 0;
		}
		else {
			p = std::exp( massCoeff*totalMass );
		}

		std::cout << "p: " << p << " for mass " << totalMass << std::endl;
		return p;
		
	}
				
}


