#ifndef _LATTICE_CONSTRUCTOR_H_
#define _LATTICE_CONSTRUCTOR_H_

#include "intelligent/GibbsField.h"
#include "intelligent/DiscretePoint.h"

#include <tuple>

#include <boost/function.hpp>

namespace intelligent {
	
	/*! \brief Prints the point as "(x, y, z)" */
	std::ostream& operator<<( std::ostream& os, const DiscretePoint3& point );

	/*! \brief Arranges node IDs into a 3D cubical lattice. */
	class Lattice {
	public:

		/*! \brief Creates an empty lattice with bounding box minima set to
		 * +infinity and maxima set to -infinity. */
		Lattice();

		// NOTE Copy construction should work fine by default since we use IDs.
		
		/*! \brief Inserts a node into this lattice. */
		void AddNode( unsigned int ID, const DiscretePoint3& pos );

		/*! \brief Retrieve the position corresponding to an ID. */
		DiscretePoint3 GetNodePosition( unsigned int id ) const;

		/*! \brief Retrieve the node from position or ID. */
		unsigned int GetNodeID( const DiscretePoint3& pos ) const;

		/*! \brief Retrieve all node IDs. Order is undefined. */
		std::vector<unsigned int> GetNodeIDs() const;

		/*! \brief Return this lattice's bounding box. */
		DiscreteBox3 GetBoundingBox() const;
		
	private:

		typedef std::unordered_map <unsigned int, DiscretePoint3> IDPositionMap;
		typedef std::unordered_map <DiscretePoint3, unsigned int> PositionIDMap;
		
		/*! \brief Map from node IDs to positions. */
		IDPositionMap idPositionMap;

		/*! \brief Map from positions to node IDs. */
		PositionIDMap positionIDMap;

		/*! \brief The bounding box around this lattice. */
		DiscreteBox3 boundingBox;

	};
	
}

#endif