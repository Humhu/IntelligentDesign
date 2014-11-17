#ifndef _ASSEMBLY_CONSTRUCTOR_H_
#define _ASSEMBLY_CONSTRUCTOR_H_

#include "intelligent/Lattice.h"
#include "intelligent/DiscreteAssembly.h"

namespace intelligent {

	/*! \brief Describes a "slot" in a 3D lattice for a potential. The slot is
	 * centered on a node and identifies other required nodes with relative
	 * x, y, and z coordinates. Ordering of IDs is respected.
	 *
	 * The slot object can be used to efficiently check and add potentials upon
	 * the addition of a new node/variable in a field-lattice pair. */
	class AssemblySlot {
	public:

		typedef std::shared_ptr<AssemblySlot> Ptr;
		typedef boost::function <GibbsPotential::Ptr
			( const GibbsField&, const Lattice&, unsigned int, const std::vector<unsigned int>& )>
			PotentialConstructor;

		AssemblySlot( const std::vector <DiscretePoint3>& _points,
					 const PotentialConstructor& _constructor );

		/*! \brief Performs slot update for the given field and lattice with a
		 * specified new lattice position. */
		void UpdateSlot( DiscreteAssembly& assembly,
						 const DiscretePoint3& added );

	private:

		DiscreteBox3 boundingBox;
		const std::vector<DiscretePoint3> points;
		const PotentialConstructor& constructor;
		
		/*! \brief Attempt to construct a potential centered at the query position. */
		void CheckPoint( DiscreteAssembly& assembly,
						 const DiscretePoint3& added, const DiscretePoint3& query );
		
		/*! \brief Returns if the query position is in the clique centered at
		 * the base position. */
		bool InClique( const DiscretePoint3& base, const DiscretePoint3& query ) const;

	};

	/*! \brief Constructs 3D lattice Gibbs fields and adds potentials into
	 * basic "slot" types. Can also be used to extend existing fields. */
	class AssemblyConstructor {
	public:

		typedef boost::function<GibbsVariable::Ptr( const GibbsField&, unsigned int )>
				VariableConstructor;
		
		AssemblyConstructor( VariableConstructor constructor );
		
		/*! \brief Adds a new slot checking object to this constructor. */
		void AddSlot( const AssemblySlot::Ptr& slot );

		/*! \brief Adds the specified voxel to the assembly. */
		void AddVoxel( DiscreteAssembly& assembly, const DiscretePoint3& pos );

		/*! \brief Check all slots over all positions. */
		void BuildPotentials( DiscreteAssembly& assembly );
		
	private:

		VariableConstructor constructor;
		std::vector<AssemblySlot::Ptr> slots;
		
	};

}
	
#endif