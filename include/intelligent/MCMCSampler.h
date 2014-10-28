#ifndef _MCMC_SAMPLER_H_
#define _MCMC_SAMPLER_H_

#include "intelligent/GibbsField.h"
#include "intelligent/RandomDistributions.h"

namespace intelligent {

	class MCMCSampler {
	public:

		MCMCSampler();

		/*! \brief Seeds with the current system time. */
		void SeedDistribution();

		/*! \brief Runs Monte Carlo Markov Chain sampling on the given Gibbs field
		 * for a specified number of samples. Note that running this function with
		 * multiple samples is faster than calling it multiple times in sequence. */
		void Sample( GibbsField& field, unsigned int numSamples = 1 );

	private:

		UniformIntegerDistribution uid;
		
	};
	
}

#endif