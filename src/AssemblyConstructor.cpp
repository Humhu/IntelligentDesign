#include "intelligent/AssemblyConstructor.h"

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

namespace intelligent {

	AssemblySlot::AssemblySlot( const std::vector<DiscretePoint3>& _points,
								const PotentialConstructor& _constructor ) :
		boundingBox( _points ),
		points( _points ),
		constructor( _constructor ) {}

	void AssemblySlot::UpdateSlot( GibbsField& field, const Lattice& lattice,
								   const DiscretePoint3& added ) {

		DiscreteBox3 centeredBox = boundingBox;
		centeredBox.Shift( added );

		// For each point in the bounds of the slot centered on the new node, see if
		// this slot at that point can construct a potential.
		DiscreteBox3::Operator op =
			boost::bind( &AssemblySlot::CheckPoint, this, field, lattice, added, _1 );
		centeredBox.Iterate( op );
	}
	
	void AssemblySlot::CheckPoint( GibbsField& field, const Lattice& lattice,
								   const DiscretePoint3& added,
								   const DiscretePoint3& query ) {

		// If the new point is not in the clique of the query point, its addition
		// cannot result in the construction of a potential. This check also makes
		// sure we never double-construct a slot potential.
		if( !InClique( query, added ) ) { return; }

		std::vector<unsigned int> ids;
		BOOST_FOREACH( const DiscretePoint3& offset, points ) {
			
			try {
				DiscretePoint3 pos = query + offset;
				ids.push_back( lattice.GetNodeID( pos ) );
			}
			catch ( std::out_of_range e ) {
				// This means the slot's nodes do not all exist yet
				return;
			}
		}
		
		// At this point we have all the ordered IDs to construct the potential
		unsigned int potID = field.NumPotentials();
		GibbsPotential::Ptr pot = constructor( field, potID, ids );
		field.AddPotential( pot );

		// Also update the corresponding variables with the new potential
		BOOST_FOREACH( unsigned int varID, ids ) {
			GibbsVariable::Ptr var = field.GetVariable( varID );
			var->AddPotential( potID );
		}
		
	}

	bool AssemblySlot::InClique( const DiscretePoint3& base, const DiscretePoint3& query ) const {

		DiscretePoint3 offset = query - base;
		BOOST_FOREACH( const DiscretePoint3& point, points ) {
			if( point == offset ) {
				return true;
			}
		}
		return false;
	}

	AssemblyConstructor::AssemblyConstructor( VariableConstructor _constructor ) :
		constructor( _constructor ) {}

	void AssemblyConstructor::AddSlot( const AssemblySlot::Ptr& slot ) {
		slots.push_back( slot );
	}

	void AssemblyConstructor::AddVoxel( GibbsField& field, Lattice& lattice,
										const DiscretePoint3& pos ) {
		unsigned int nodeID = field.NumVariables();
		GibbsVariable::Ptr var = constructor( field, nodeID );
		field.AddVariable( var );
		lattice.AddNode( nodeID, pos );

		BOOST_FOREACH( const AssemblySlot::Ptr& slot, slots ) {
			slot->UpdateSlot( field, lattice, pos );
		}
	}
	
}