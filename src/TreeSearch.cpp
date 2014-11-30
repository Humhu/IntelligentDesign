#include "intelligent/TreeSearch.h"

namespace intelligent {

	TreeSearch::TreeSearch() 
	{}
	
	void TreeSearch::Add(DiscreteAssembly::Ptr _da) {
		double cost = ComputeCost(*_da);
		pq.emplace(cost, _da);
	}
	
	void TreeSearch::Add(const std::vector<DiscreteAssembly::Ptr> & _das) {
		for (auto & _da : _das) {
			Add(_da);
		}
	}
	
	DiscreteAssembly::Ptr TreeSearch::Next() {
		DiscreteAssembly::Ptr da = pq.top().second;
		pq.pop();
		auto das = GetSuccessors(*da);
		Add(das);
		return da;
	}
	
	DiscreteAssembly::Ptr TreeSearch::Peek() {
		return pq.top().second;
	}
	
	std::vector<DiscreteAssembly::Ptr> 
	TreeSearch::GetSuccessors(const DiscreteAssembly & _da) {
		// TODO: Generate successors for an assembly
	}
	
	double TreeSearch::ComputeCost(const DiscreteAssembly & _da) {
		// TODO: Compute cost of an assembly
	}
}
