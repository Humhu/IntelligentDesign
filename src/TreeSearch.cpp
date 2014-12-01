#include "intelligent/TreeSearch.h"

namespace intelligent {

	TreeSearch::TreeSearch( AssemblySampler& _sampler ) :
		sampler( _sampler ),
		numSuccessors( 5 ),
		sampleDepth( 10 ) {}

	void TreeSearch::SetNumSuccessors( unsigned int n ) {
		numSuccessors = n;
	}

	void TreeSearch::SetSampleDepth( unsigned int d ) {
		sampleDepth = d;
	}
	
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
		if (pq.empty()) { return DiscreteAssembly::Ptr(); }
		DiscreteAssembly::Ptr da = pq.top().second;
		pq.pop();
		auto das = GetSuccessors( da );
		Add(das);
		return da;
	}
	
	DiscreteAssembly::Ptr TreeSearch::Peek() {
		if (pq.empty()) { return DiscreteAssembly::Ptr(); }
		return pq.top().second;
	}
	
	std::vector<DiscreteAssembly::Ptr> 
	TreeSearch::GetSuccessors( DiscreteAssembly::Ptr& _da) {
		// TODO: Generate successors for an assembly
		sampler.SetBase( _da );
		return sampler.Sample( numSuccessors, sampleDepth );
	}
	
	double TreeSearch::ComputeCost(const DiscreteAssembly & _da) {
		// TODO: Compute cost of an assembly
		return 0;
	}
}
