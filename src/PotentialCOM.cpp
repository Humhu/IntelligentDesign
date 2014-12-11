#include "intelligent/PotentialCOM.h"
#include "intelligent/BlockVariable.h"
#include "intelligent/DiscretePoint.h"

#include <iostream>
#include <memory>
#include <cmath>

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
		ContinuousPoint3 com;
		com.x = bbox.minX + dx/2;
		com.y = bbox.minY + dy/2;
		com.z = bbox.minZ;
		SetDesiredCOM(com);
		std::cout << "Constructing COM potential." << std::endl;
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

	void PotentialCOM::SetDesiredCOM(const ContinuousPoint3 & _com) {
		desiredCOM = _com;
	}
	
	double PotentialCOM::CalculatePotential() {

		ContinuousPoint3 com = CalculateCOM();
		
		auto dx = std::abs(com.x - desiredCOM.x);
		auto dy = std::abs(com.y - desiredCOM.y);
		auto dz = std::abs(com.z - desiredCOM.z);

// 		double p = 1;
// 		if( dx + dy + dz > 4 ) {
// 			p = 0.01;
// 		}
		double deviation = dx + dy + dz;
		double p = 1.0;

		// HACK Make a parameter or something
		if( deviation > 2.0 ) {
			//p = std::exp( -3*deviation );
			p = 1E-6;
		}
		else {
			p = std::exp( -deviation );
		}

// 		std::cout << "p: " << p << " for com: " << com.x << ", " << com.y << ", " << com.z << std::endl;
		return p;
	}

	ContinuousPoint3 PotentialCOM::CalculateCOM() {
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
			cx += mass * blockPosition.x;
			cy += mass * blockPosition.y;
			cz += mass * blockPosition.z;
		}
		ContinuousPoint3 com;
		com.x = cx/totalMass;
		com.y = cy/totalMass;
		com.z = cz/totalMass;
		return com;
	}
				
}


