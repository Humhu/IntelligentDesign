#include "intelligent/PotentialCOM.h"
#include "intelligent/BlockVariable.h"
#include "intelligent/DiscretePoint.h"

#include <memory>

namespace intelligent {

	PotentialCOM::PotentialCOM(const GibbsField & _field, unsigned int _id,
		const std::vector<unsigned int> & _vids, const Lattice & _lattice) 
	: GibbsPotential(_field, _id, _vids)
	, lattice(&_lattice) 
	{
		const auto & bbox = lattice->GetBoundingBox(); 
		auto dx = bbox.maxX - bbox.minX + 1;
		auto dy = bbox.maxY - bbox.minY + 1;
		auto dz = bbox.maxZ - bbox.minZ + 1;
		DiscretePoint3 com;
		com.x = bbox.minX + dx/2;
		com.y = bbox.minY + dy/2;
		com.z = bbox.minZ;
		SetDesiredCOM(com);
	}  
    
	// FIXME: Pointer to lattice and desiredCOM
	// Calling this constructor from anywhere other than Clone() is a bad idea
	PotentialCOM::PotentialCOM(const GibbsField & _field, 
		const GibbsPotential & _other) 
	: GibbsPotential(_field, _other) 
	{}

	GibbsPotential::Ptr PotentialCOM::Clone(const GibbsField & _field) {
		return std::make_shared<PotentialCOM>(_field, *this);
	}

	void PotentialCOM::SetDesiredCOM(const DiscretePoint3 & _com) {
		desiredCOM = _com;
	}
	
	double PotentialCOM::CalculatePotential() {
		const auto & bbox = lattice->GetBoundingBox(); 
		std::vector<GibbsVariable::Ptr> clique = GetClique();
		BlockVariable::Ptr blockVariable;
		DiscretePoint3 blockPosition;
		double cx = 0.0, cy = 0.0, cz = 0.0;
		double totalMass = 0.0;
		for (size_t i=0; i<clique.size(); i++) {
			double mass = 0.0;
			blockVariable = std::dynamic_pointer_cast<BlockVariable>(clique[i]);
			blockPosition = lattice->GetNodePosition(blockVariable->id);
			switch (blockVariable->GetState()) {
			case BLOCK_FULL:  mass = 1.0; break;
			case BLOCK_HALF:  mass = 0.5; break;
			case BLOCK_EMPTY: mass = 0.0; break;
			default: throw std::runtime_error("Invalid block state");
			}
			totalMass += mass;
			cx += mass * (blockPosition.x - bbox.minX);
			cy += mass * (blockPosition.y - bbox.minY);
			cz += mass * (blockPosition.z - bbox.minZ);
		}
		cx /= totalMass;
		cy /= totalMass;
		cz /= totalMass;
		auto dx = std::abs(cx + bbox.minX - desiredCOM.x)
		        / std::max(std::abs(bbox.maxX-desiredCOM.x),
		                   std::abs(bbox.minX-desiredCOM.x));
		auto dy = std::abs(cy + bbox.minY - desiredCOM.y)
		        / std::max(std::abs(bbox.maxY-desiredCOM.y),
		                   std::abs(bbox.minY-desiredCOM.y));
		auto dz = std::abs(cz + bbox.minZ - desiredCOM.z)
		        / std::max(std::abs(bbox.maxZ-desiredCOM.z),
		                   std::abs(bbox.minZ-desiredCOM.z));
		double p = (dx + dy + dz)/3.0;
		
		return p;
	}
				
}


