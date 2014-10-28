#include "intelligent/MCMCSampler.h"

namespace intelligent {

	MCMCSampler::MCMCSampler() :
		uid( 0, 1 ) {}

	void MCMCSampler::SeedDistribution() {

		uid.SetSeed( clock() );
	}
		
	void MCMCSampler::Sample( GibbsField& field, unsigned int numSamples ) {

		// First pull nodes to initialize random distribution
		std::vector<GibbsVariable::Ptr> variables = field.GetVariables();
		uid.SetBounds( 0, variables.size() );
		
		for( unsigned int i = 0 ; i < numSamples; i++ ) {

			int index = uid.Sample();
			variables[ index ]->Sample();
		}
	}
	
}