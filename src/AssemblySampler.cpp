#include "intelligent/AssemblySampler.h"

namespace intelligent {

	AssemblySampler::AssemblySampler( MCMCSampler& _sampler ) :
		sampler( _sampler ),
		baseAssembly( nullptr ) {}

	void AssemblySampler::SetBase( DiscreteAssembly::Ptr assembly ) {
		baseAssembly = assembly;
	}

	std::vector<DiscreteAssembly::Ptr> AssemblySampler::Sample( unsigned int numSamples,
																unsigned int sampleDepth ) {

		std::vector<DiscreteAssembly::Ptr> samples( numSamples );
		for( int i = 0; i < numSamples; i++ ) {

			DiscreteAssembly::Ptr sample =
				std::make_shared<DiscreteAssembly>( *baseAssembly );
			sampler.Sample( sample->GetField(), sampleDepth ); // Samplicious
			samples[i] = sample;
		}
		
		return samples;
		
	}
	
}