#ifndef _TREE_SEARCH_H_
#define _TREE_SERACH_H_

#include <queue>

#include "intelligent/DiscreteAssembly.h"

namespace intelligent {

	class TreeSearch {
	public:
		TreeSearch();
		
		void Add(DiscreteAssembly::Ptr _da);
		void Add(const std::vector<DiscreteAssembly::Ptr> & _das);
		DiscreteAssembly::Ptr Next();
		DiscreteAssembly::Ptr Peek();
		
	private:
		std::vector<DiscreteAssembly::Ptr> 
		GetSuccessors(const DiscreteAssembly & _da);
		
		double ComputeCost(const DiscreteAssembly & _da);
		
		std::priority_queue<std::pair<double,DiscreteAssembly::Ptr>> pq;
		
		// FIXME: Need to keep track of previously seen assemblies?
	};

}

#endif
