#include "intelligent/AssemblySampler.h"
#include <iostream>

namespace intelligent {

	AssemblySampler::AssemblySampler( MCMCSampler& _sampler ) :
		sampler( _sampler ),
		baseAssembly( nullptr ) { 
// 			std::cout << "inside AssemblySampler.AssemblySampler, _  " << _sampler.hasIndices << std::endl;
// 			std::cout << "inside AssemblySampler.AssemblySampler  " << sampler.hasIndices << std::endl;			
		}

	void AssemblySampler::SetBase( DiscreteAssembly::Ptr assembly ) {
		baseAssembly = assembly;
	}

	std::vector<DiscreteAssembly::Ptr> AssemblySampler::Sample( unsigned int numSamples,
																unsigned int sampleDepth ) {

// 		std::cout << "inside AssemblySampler.SetBase  " << sampler.hasIndices << std::endl;			

		
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