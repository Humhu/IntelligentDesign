#ifndef _ASSEMBLY_SAMPLER_H_
#define _ASSEMBLY_SAMPLER_H_

#include "intelligent/DiscreteAssembly.h"
#include "intelligent/MCMCSampler.h"

namespace intelligent {

	/*! \brief Utility class to perform DiscreteAssembly sample generation. */
	class AssemblySampler {
	public:

		/*! \brief Construct an assembly sampler using a specified MCMC sampler. */
		AssemblySampler( MCMCSampler& _sampler );

		/*! \brief Set the assembly from which to generate samples. */
		void SetBase( DiscreteAssembly::Ptr assembly );

		/*! \brief Generate a number of samples by Gibbs sampling for a
		 * specified number of iterations each from the base assembly. */
		std::vector<DiscreteAssembly::Ptr> Sample( unsigned int numSamples,
												   unsigned int sampleDepth );

	protected:

		MCMCSampler& sampler;
		DiscreteAssembly::Ptr baseAssembly;
		
	};
	
}

#endif