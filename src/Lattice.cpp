#include "intelligent/Lattice.h"

#include <stdexcept>
#include <sstream>

#include <boost/foreach.hpp>
#include <boost/functional/hash.hpp>

namespace intelligent {

	DiscretePoint3::DiscretePoint3() :
		x( 0 ), y( 0 ), z( 0 ) {}

	bool DiscretePoint3::operator==( const DiscretePoint3& other ) const {
		return x == other.x && y == other.y && z == other.z;
	}

	std::size_t DiscretePoint3Hash::operator()( const DiscretePoint3& in ) const {

		std::size_t seed = 0;
		boost::hash_combine( seed, in.x );
		boost::hash_combine( seed, in.y );
		boost::hash_combine( seed, in.z );
		return seed;
	}
	
	Lattice::Lattice() :
		minX( std::numeric_limits<int>::max() ),
		maxX( std::numeric_limits<int>::min() ),
		minY( minX ), maxY( maxX ),
		minZ( minX ), maxZ( maxX ) {}

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

		// Update bounding box
		if( pos.x < minX ) {
			minX = pos.x;
		}
		if( pos.x > maxX ) {
			maxX = pos.x;
		}
		if( pos.y < minY ) {
			minY = pos.y;
		}
		if( pos.y > maxY ) {
			maxY = pos.y;
		}
		if( pos.z < minZ ) {
			minZ = pos.z;
		}
		if( pos.z > maxZ ) {
			maxZ = pos.z;
		}
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

// 	GibbsPotential::Ptr LatticeSlot::CheckSlot( const Lattice& lattice,
// 												const DiscretePoint3& pos ) {
// 
// 		std::vector<unsigned int> ids;
// 		BOOST_FOREACH( const DiscretePoint3& offset, points ) {
// 
// 			try {
// 				
// 				DiscretePoint3 query = pos;
// 				std::get<0>( query ) += std::get<0>( offset );
// 				std::get<1>( query ) += std::get<1>( offset );
// 				std::get<2>( query ) += std::get<2>( offset );
// 
// 				ids.push_back( lattice.GetNodeID( query );
// 			}
// 			catch ( std::out_of_range e ) {
// 				
// 				// This means the slot is not valid for this position
// 				return nullptr;
// 			}
// 		}

		// At this point we have all the ordered IDs to construct the potential
		
		
	//}
}