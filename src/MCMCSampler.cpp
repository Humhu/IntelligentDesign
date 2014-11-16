#include "intelligent/MCMCSampler.h"

#include <cassert>
#include <iostream>
#include <random>

namespace intelligent {

	MCMCSampler::MCMCSampler() :
		generator(rd()) {}

	void MCMCSampler::SeedDistribution() {

		generator.seed(rd());
	}
		
	void MCMCSampler::Sample( GibbsField& field, unsigned int numSamples ) {

		// First pull nodes to initialize random distribution
		std::vector<GibbsVariable::Ptr> variables = field.GetVariables();
		
		assert(variables.size() > 0);
		std::uniform_int_distribution<> uid( 0, variables.size()-1 );
		std::uniform_real_distribution<> rid( 0, 1 );
		
		for( unsigned int i = 0 ; i < numSamples; i++ ) {

			int index = uid(generator);
			assert(variables[index] != nullptr);
			variables[ index ]->Sample( rid(generator) );
		}
	}
	
}
