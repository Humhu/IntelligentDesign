#include "intelligent/AssemblyConstructor.h"

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <iostream>

namespace intelligent {

	AssemblySlot::AssemblySlot( const std::vector<DiscretePoint3>& _points,
								const PotentialConstructor& _constructor ) :
		boundingBox( _points ),
		points( _points ),
		constructor( _constructor ) {}

	void AssemblySlot::UpdateSlot( DiscreteAssembly& assembly,
								   const DiscretePoint3& query ) {

		std::vector<unsigned int> ids;
		BOOST_FOREACH( const DiscretePoint3& offset, points ) {
			
			try {
				DiscretePoint3 pos = query + offset;
				ids.push_back( assembly.GetLattice().GetNodeID( pos ) );
			}
			catch ( std::out_of_range e ) {
				// This means the slot's nodes do not all exist
				return;
			}
		}
		
		// At this point we have all the ordered IDs to construct the potential
		unsigned int potID = assembly.GetField().NumPotentials();
		GibbsPotential::Ptr pot = constructor( assembly.GetField(), assembly.GetLattice(),
											   potID, ids );
		assembly.GetField().AddPotential( pot );
		
		// Also update the corresponding variables with the new potential
		BOOST_FOREACH( unsigned int varID, ids ) {
			GibbsVariable::Ptr var = assembly.GetField().GetVariable( varID );
			var->AddPotential( potID );
		}
	}

// 	bool AssemblySlot::InClique( const DiscretePoint3& base, const DiscretePoint3& query ) const {
// 
// 		DiscretePoint3 offset = query - base;
// 		BOOST_FOREACH( const DiscretePoint3& point, points ) {
// 			if( point == offset ) {
// 				return true;
// 			}
// 		}
// 		return false;
// 	}

	AssemblyConstructor::AssemblyConstructor( VariableConstructor _constructor ) :
		constructor( _constructor ) {}

	void AssemblyConstructor::AddSlot( const AssemblySlot::Ptr& slot ) {
		slots.push_back( slot );
	}

	void AssemblyConstructor::AddVoxel( DiscreteAssembly& assembly,
										const DiscretePoint3& pos ) {
		unsigned int nodeID = assembly.GetField().NumVariables();
		GibbsVariable::Ptr var = constructor( assembly.GetField(), nodeID );
		assembly.GetField().AddVariable( var );

		assembly.GetLattice().AddNode( nodeID, pos );

	}

	void AssemblyConstructor::BuildPotentials( DiscreteAssembly& assembly ) {

		DiscreteBox3 range = assembly.GetLattice().GetBoundingBox();
		
		BOOST_FOREACH( const AssemblySlot::Ptr& slot, slots ) {
			DiscreteBox3::Operator updateOp =
				boost::bind( &AssemblySlot::UpdateSlot, slot.get(), boost::ref(assembly), _1 );
			
			range.Iterate( updateOp );
		}		
	}
	
}