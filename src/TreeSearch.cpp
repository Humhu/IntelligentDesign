#include "intelligent/TreeSearch.h"
#include "intelligent/DiscretePoint.h"
#include "intelligent/PotentialCOM.h"

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
		sampler.SetBase( _da );
		return sampler.Sample( numSuccessors, sampleDepth );
	}
	
	double TreeSearch::ComputeCost(const DiscreteAssembly & _da) {
		// TODO: Compute cost of an assembly
		const auto & lattice = _da.GetLattice();
		const std::vector<unsigned> & nodeIds = lattice.GetNodeIDs();
		const auto & bbox = lattice.GetBoundingBox();
		std::vector<GibbsVariable::Ptr> vars;
		for (const auto & nid : nodeIds) {
			vars.push_back(_da.GetField().GetVariable(nid));
		}
		BlockVariable::Ptr blockVariable;
		DiscretePoint3 blockPosition;
		double cx = 0.0, cy = 0.0, cz = 0.0;
		double totalMass = 0.0;
		for (const auto & v : vars) {
			double mass = 0.0;
			blockVariable = std::dynamic_pointer_cast<BlockVariable>(v);
			blockPosition = lattice.GetNodePosition(blockVariable->id);
			switch (blockVariable->GetState()) {
				case BLOCK_FULL:  mass = 1.0; break;
				case BLOCK_HALF:  mass = 0.5; break;
				case BLOCK_EMPTY: mass = 0.0; break;
				default: throw std::runtime_error("Invalid block state");
			}
			totalMass += mass;
			cx += mass * blockPosition.x;
			cy += mass * blockPosition.y;
			cz += mass * blockPosition.z;
		}
		ContinuousPoint3 com;
		com.x = cx/totalMass;
		com.y = cy/totalMass;
		com.z = cz/totalMass;
		
		double dx = bbox.maxX - bbox.minX + 1;
		double dy = bbox.maxY - bbox.minY + 1;
		double dz = bbox.maxZ - bbox.minZ + 1;
		ContinuousPoint3 desiredCOM;
		desiredCOM.x = bbox.minX + dx/2;
		desiredCOM.y = bbox.minY + dy/2;
		desiredCOM.z = bbox.minZ;
		
		dx = std::abs(com.x - desiredCOM.x);
		dy = std::abs(com.y - desiredCOM.y);
		dz = std::abs(com.z - desiredCOM.z);

		return dx + dy + dz;
	}
}
