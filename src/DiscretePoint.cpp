#include "intelligent/DiscretePoint.h"

#include <boost/foreach.hpp>

#include <iostream>

namespace intelligent {

	ContinuousPoint3::ContinuousPoint3() :
	x(0), y(0), z(0) {}
	
	ContinuousPoint3::ContinuousPoint3( double _x, double _y, double _z ) :
	x( _x ), y( _y ), z( _z ) {}
	
	std::ostream& operator<<( std::ostream& os, const ContinuousPoint3& point ) {
		os << "(" << point.x << ", " << point.y << ", " << point.z << ")";
		return os;
	}
	
	DiscretePoint3::DiscretePoint3() :
		x( 0 ), y( 0 ), z( 0 ) {}

	DiscretePoint3::DiscretePoint3( int _x, int _y, int _z ) :
		x( _x ), y( _y ), z( _z ) {}
	
	bool DiscretePoint3::operator==( const DiscretePoint3& other ) const {
		return x == other.x && y == other.y && z == other.z;
	}
	
	DiscretePoint3 DiscretePoint3::operator+() const {
		DiscretePoint3 ret( *this );
		return ret;
	}
	
	DiscretePoint3 DiscretePoint3::operator-() const {
		DiscretePoint3 ret;
		ret.x = -x;
		ret.y = -y;
		ret.z = -z;
		return ret;
	}
	
	DiscretePoint3 operator+( const DiscretePoint3& first, const DiscretePoint3& second ) {
		DiscretePoint3 sum;
		sum.x = first.x + second.x;
		sum.y = first.y + second.y;
		sum.z = first.z + second.z;
		return sum;
	}
	
	DiscretePoint3 operator-( const DiscretePoint3& first, const DiscretePoint3& second ) {
		DiscretePoint3 sum;
		sum.x = first.x - second.x;
		sum.y = first.y - second.y;
		sum.z = first.z - second.z;
		return sum;
	}

	std::ostream& operator<<( std::ostream& os, const DiscretePoint3& point ) {
		os << "(" << point.x << ", " << point.y << ", " << point.z << ")";
		return os;
	}
	
	DiscreteBox3::DiscreteBox3() :
	minX( std::numeric_limits<int>::max() ),
	maxX( std::numeric_limits<int>::min() ),
	minY( minX ), maxY( maxX ),
	minZ( minX ), maxZ( maxX ) {}
	
	DiscreteBox3::DiscreteBox3( const DiscretePoint3& point ) :
	minX( point.x ), maxX( point.x ),
	minY( point.y ), maxY( point.y ),
	minZ( point.z ), maxZ( point.z ) {}
	
	DiscreteBox3::DiscreteBox3( const std::vector<DiscretePoint3>& points ) :
		minX( std::numeric_limits<int>::max() ),
		maxX( std::numeric_limits<int>::min() ),
		minY( minX ), maxY( maxX ),
		minZ( minX ), maxZ( maxX ) {
		
		BOOST_FOREACH( const DiscretePoint3& point, points ) {
			ExpandToInclude( point );
		}
	}
	
	bool DiscreteBox3::Contains( const DiscretePoint3& point ) const {
		return point.x > minX && point.x < maxX &&
		point.y > minY && point.y < maxY &&
		point.z > minZ && point.z < maxZ;
	}
	
	void DiscreteBox3::ExpandToInclude( const DiscretePoint3& point ) {
		
		// Update bounding box
		if( point.x <= minX ) {
			minX = point.x;
		}
		if( point.x >= maxX ) {
			maxX = point.x;
		}
		if( point.y <= minY ) {
			minY = point.y;
		}
		if( point.y >= maxY ) {
			maxY = point.y;
		}
		if( point.z <= minZ ) {
			minZ = point.z;
		}
		if( point.z >= maxZ ) {
			maxZ = point.z;
		}
	}
	
	void DiscreteBox3::Shift( const DiscretePoint3& offset ) {
		minX += offset.x;
		maxX += offset.x;
		minY += offset.y;
		maxY += offset.y;
		minZ += offset.z;
		maxZ += offset.z;
	}

	void DiscreteBox3::Iterate( Operator op ) const {

		for( int x = minX; x <= maxX; x++ ) {
			for( int y = minY; y <= maxY; y++ ) {
				for( int z = minZ; z <= maxZ; z++ ) {
					DiscretePoint3 query;
					query.x = x;
					query.y = y;
					query.z = z;
					op( query );
				}
			}
		}
	}

}
