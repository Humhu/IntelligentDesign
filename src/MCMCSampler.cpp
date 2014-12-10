#include "intelligent/MCMCSampler.h"

#include <cassert>
#include <iostream>
#include <random>

namespace intelligent {

	MCMCSampler::MCMCSampler() :
		generator( rd() ),
		hasIndices( false ) {}

	void MCMCSampler::SeedDistribution() {

		generator.seed( rd() );
	}

	void MCMCSampler::SetIndexSet( const std::vector<unsigned int>& ind ) {
// 		std::cout << "SetIndexSet has been called" << std::endl;
		hasIndices = true;
		indices = ind;
	}
		
	void MCMCSampler::Sample( GibbsField& field, unsigned int numSamples ) {

		// First pull nodes to initialize random distribution
		std::vector<GibbsVariable::Ptr> variables = field.GetVariables();
		
		assert(variables.size() > 0);

		std::uniform_real_distribution<> rid( 0, 1 );
		
		if( !hasIndices ) {
// 			std::cout << "sampling withOUT indices" << std::endl;			
			std::uniform_int_distribution<> uid( 0, variables.size()-1 );
			for( unsigned int i = 0 ; i < numSamples; i++ ) {
				
				int index = uid(generator);
// 				std::cout << "Sampling index " << index << std::endl;
				assert(variables[index] != nullptr);
				variables[ index ]->Sample( rid(generator) );
			}
		}
		else {
			std::cout << "sampling WITH indices" << std::endl;
			std::cout << "Indices.size " << indices.size() << std::endl;

			std::uniform_int_distribution<> uid( 0, indices.size()-1 );
			for( unsigned int i = 0 ; i < numSamples; i++ ) {
				
				int index = indices[uid(generator)];
// 				std::cout << "Sampling index " << index << std::endl;
				assert(variables[index] != nullptr);
				variables[ index ]->Sample( rid(generator) );
				
// 				std::cout << variables[ index ]->id << std::endl;
			}
		}

	}
	
}
