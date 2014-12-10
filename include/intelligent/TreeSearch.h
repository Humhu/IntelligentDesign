#ifndef _TREE_SEARCH_H_
#define _TREE_SERACH_H_

#include <queue>

#include "intelligent/DiscreteAssembly.h"
#include "intelligent/AssemblySampler.h"

namespace intelligent {

	/*! \brief Class to perform tree search over discrete assemblies. */
	class TreeSearch {
	public:
		
		TreeSearch( AssemblySampler& _sampler );

		/*! \brief Specify the number of successors to generate at each node (branch factor) */
		void SetNumSuccessors( unsigned int n );

		/*! \brief Specify the number of times to run MCMC when generating successors. */
		void SetSampleDepth( unsigned int d );

		/*! \brief Add a discrete assembly to the search queue. */
		void Add(DiscreteAssembly::Ptr _da);

		/*! \brief Add a vector of discrete assemblies to the search queue. */
		void Add(const std::vector<DiscreteAssembly::Ptr> & _das);

		/*! \brief Retrieve and dequeue the assembly in the queue with the highest reward. */
		DiscreteAssembly::Ptr Next();

		/*! \brief Retrieve the assembly in the queue with the highest reward. */
		DiscreteAssembly::Ptr Peek();

		size_t Size();
		
	private:

		AssemblySampler& sampler;
		unsigned int numSuccessors;
		unsigned int sampleDepth;
		
		/*! \brief Generates samples for the specified assembly. */
		std::vector<DiscreteAssembly::Ptr> 
		GetSuccessors( DiscreteAssembly::Ptr& _da );

		/*! \brief Compute the cost/reward for an assembly. */
		double ComputeCost(const DiscreteAssembly & _da);
		
		std::priority_queue<std::pair<double,DiscreteAssembly::Ptr>> pq;
		
		// FIXME: Need to keep track of previously seen assemblies?
	};

}

#endif
