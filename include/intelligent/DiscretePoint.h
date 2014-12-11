#ifndef _DISCRETE_POINT_H_
#define _DISCRETE_POINT_H_

#include <boost/functional/hash.hpp>
#include <boost/function.hpp>

#include <ostream>
#include <vector>

namespace intelligent {

	struct ContinuousPoint3 {
		double x;
		double y;
		double z;
		
		ContinuousPoint3();
		ContinuousPoint3( double _x, double _y, double _z );
	};

	std::ostream& operator<<( std::ostream& os, const ContinuousPoint3& point );
	
	/*! \brief Discrete 3D coordinates. */
	struct DiscretePoint3 {
		int x;
		int y;
		int z;
		
		DiscretePoint3();
		DiscretePoint3( int _x, int _y, int _z );
		
		bool operator==( const DiscretePoint3& other ) const;
		
		DiscretePoint3 operator+() const;
		DiscretePoint3 operator-() const;
	};
	
	DiscretePoint3 operator+( const DiscretePoint3& first, const DiscretePoint3& second );
	DiscretePoint3 operator-( const DiscretePoint3& first, const DiscretePoint3& second );

	/*! \brief Prints the point as "(x, y, z)" */
	std::ostream& operator<<( std::ostream& os, const DiscretePoint3& point );
	
	/*! \brief Discrete bounding box in 3D. */
	struct DiscreteBox3 {

		typedef boost::function<void(const DiscretePoint3&)> Operator;
		
		int minX;
		int maxX;
		int minY;
		int maxY;
		int minZ;
		int maxZ;
		
		/*! \brief Initializes the box minima to +infinity and maxima to -infinity. */
		DiscreteBox3();
		
		/*! \brief Initializes the box to contain only the specified point. */
		DiscreteBox3( const DiscretePoint3& point );
		
		/*! \brief Initializes the box to contain the specified points. */
		DiscreteBox3( const std::vector<DiscretePoint3>& points );
		
		/*! \brief Returns whether the point is contained in the box, inclusive. */
		bool Contains( const DiscretePoint3& point ) const;
		
		/*! \brief Extends the limits to include the specified point. */
		void ExpandToInclude( const DiscretePoint3& point );
		
		/*! \brief Shifts the box by the specified amount. */
		void Shift( const DiscretePoint3& offset );

		/*! \brief Execute the operator over all points in the box. */
		void Iterate( Operator op ) const;
	};
	
}

namespace std {
	
	/*! \brief Hashing specialization for unordered_map. */
	template <>
	struct hash<intelligent::DiscretePoint3> {
		std::size_t operator()( const intelligent::DiscretePoint3& in ) const {
			std::size_t seed = 0;
			boost::hash_combine( seed, in.x );
			boost::hash_combine( seed, in.y );
			boost::hash_combine( seed, in.z );
			return seed;
		}
	};
	
}

#endif