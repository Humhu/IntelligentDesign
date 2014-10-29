#include "intelligent/Lattice.h"

#include <stdexcept>
#include <sstream>

#include <boost/foreach.hpp>

namespace intelligent {
	
	Lattice::Lattice() {}

	void Lattice::AddNode( unsigned int id, const DiscretePoint3& pos ) {

		if( idPositionMap.count( id ) > 0 ) {
			std::stringstream ss;
			ss << "Lattice already has node with id " << id;
			throw std::runtime_error( ss.str() );
		}

		if( positionIDMap.count( pos ) > 0 ) {
			std::stringstream ss;
			ss << "Lattice already has node at position " << pos;
			throw std::runtime_error( ss.str() );
		}
		
		idPositionMap[ id ] = pos;
		positionIDMap[ pos ] = id;

		boundingBox.ExpandToInclude( pos );
	}

	DiscretePoint3 Lattice::GetNodePosition( unsigned int id ) const {
		return idPositionMap.at( id );
	}

	unsigned int Lattice::GetNodeID( const DiscretePoint3& pos ) const {
		return positionIDMap.at( pos );
	}

	std::vector<unsigned int> Lattice::GetNodeIDs() const {
		std::vector<unsigned int> ids( positionIDMap.size() );
		BOOST_FOREACH( const PositionIDMap::value_type& item, positionIDMap ) {
			ids.push_back( item.second );
		}
		return ids;
	}

	DiscreteBox3 Lattice::GetBoundingBox() const {
		return boundingBox;
	}

}