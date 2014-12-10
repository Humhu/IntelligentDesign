#include "intelligent/TreeSearch.h"
#include "intelligent/DiscretePoint.h"
#include "intelligent/PotentialCOM.h"

namespace intelligent {

	bool operator<( const intelligent::SearchEntry& lhs,
					const intelligent::SearchEntry& rhs ) {
		return lhs.priority < rhs.priority;
	}
	
	TreeSearch::TreeSearch( AssemblySampler& _sampler ) :
		sampler( _sampler ),
		numSuccessors( 5 ),
		sampleDepth( 10 ),
		maxQueueSize( 400 ) {}

	void TreeSearch::SetNumSuccessors( unsigned int n ) {
		numSuccessors = n;
	}

	void TreeSearch::SetSampleDepth( unsigned int d ) {
		sampleDepth = d;
	}

	void TreeSearch::SetMaxQueueSize( unsigned int q ) {
		maxQueueSize = q;
	}
	
	void TreeSearch::Add(DiscreteAssembly::Ptr _da) {
		double cost = ComputeCost(*_da);
		SearchEntry entry;
		entry.priority = -cost;
		entry.assembly = _da;
		pq.push( entry );

		if( pq.size() > maxQueueSize ) {
			pq.popMin();
		}
		
// 		pq.emplace(-cost, _da);
	}
	
	void TreeSearch::Add(const std::vector<DiscreteAssembly::Ptr> & _das) {
		for (auto & _da : _das) {
			Add(_da);
		}
	}
	
	DiscreteAssembly::Ptr TreeSearch::Next() {
		
		if (pq.empty()) { return DiscreteAssembly::Ptr(); }		
		SearchEntry myvar = pq.findMax();
		std::cout << "priority " << myvar.priority << std::endl;
		DiscreteAssembly::Ptr da = myvar.assembly;
		auto das = GetSuccessors( da );
		Add(das);
		return da;
	}
	
	DiscreteAssembly::Ptr TreeSearch::Peek() {
		if (pq.empty()) { return DiscreteAssembly::Ptr(); }
		return pq.findMax().assembly;
	}
	
	std::vector<DiscreteAssembly::Ptr> 
	TreeSearch::GetSuccessors( DiscreteAssembly::Ptr& _da) {
		sampler.SetBase( _da );
		return sampler.Sample( numSuccessors, sampleDepth );
	}

	size_t TreeSearch::Size() {
		return pq.size();
	}
	
	double TreeSearch::ComputeCost(const DiscreteAssembly & _da) {
		const auto & lattice = _da.GetLattice();
		const std::vector<unsigned> & nodeIds = lattice.GetNodeIDs();
		const auto & bbox = lattice.GetBoundingBox();
		std::vector<GibbsVariable::Ptr> vars;
		for (const auto & nid : nodeIds) {
			vars.push_back(_da.GetField().GetVariable(nid));
		}
		
		double dx = bbox.maxX - bbox.minX + 1;
		double dy = bbox.maxY - bbox.minY + 1;
		double dz = bbox.maxZ - bbox.minZ + 1;
		ContinuousPoint3 desiredCOM;
		desiredCOM.x = bbox.minX + dx/2;
		desiredCOM.y = bbox.minY + dy/2;
		desiredCOM.z = bbox.minZ;

		double bx = 0, by = 0, bz = 0;
		BlockVariable::Ptr blockVariable;
		DiscretePoint3 blockPosition;
		double cx = 0.0, cy = 0.0, cz = 0.0;
		double totalMass = 0.0;
		double someZ = 0;
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
			bx += std::abs(desiredCOM.x - blockPosition.x);
			by += std::abs(desiredCOM.y - blockPosition.y);
			bz += std::abs(desiredCOM.z - blockPosition.z);
			
			if (mass != 0) {
			someZ += blockPosition.z;
			}
		}
		ContinuousPoint3 com;
		com.x = cx/totalMass;
		com.y = cy/totalMass;
		com.z = cz/totalMass;
		
		dx = std::abs(com.x - desiredCOM.x);
		dy = std::abs(com.y - desiredCOM.y);
		dz = std::abs(com.z - desiredCOM.z);

		double cost_diffCOM = std::sqrt(dx*dx + dy*dy + dz*dz);
		double cost_totalMass = totalMass;
		double cost_Z = -1*someZ;
// 		cost += std::sqrt(bx*bx + by*by + bz*bz);
// 		cost += totalMass;
		double cost = cost_diffCOM + cost_totalMass + cost_Z;
		
		std::cout << "cost_diffCOM = " << cost_diffCOM << std::endl;
		std::cout << "cost_totalMass = " << cost_totalMass << std::endl;
		std::cout << "cost_Z = " << cost_Z << std::endl;

		return  cost;
	}
}
