#ifndef _GIBBS_FIELD_H_
#define _GIBBS_FIELD_H_

#include <memory>
#include <vector>
#include <unordered_map>

namespace intelligent {

	class GibbsField;
	class GibbsVariable;

	/*! \note This Gibbs field implementation uses runtime lookup for speed.
	 * I expect the fields to be copied a lot, so we trade off sampling speed for copy
	 * construction complexity. Specifically, variables (nodes) and potentials (edges) do
	 * not store pointers to each other. All operations requiring this information depend
	 * on ID-based lookup from the owning field.
	 *
	 * Note that we assume IDs are assigned sequentially, starting from 0. This means
	 * that the largest ID in a field is the number of variables minus one, and the same
	 * for potentials.
	 */
	
	/*! \brief Superclass for all potentials in a Gibbs field. Equivalent to an
	 * edge in the Gibbs graph. */
	class GibbsPotential {
	public:
		
		typedef std::shared_ptr<GibbsPotential> Ptr;

		/*! \brief The field this potential belongs to. */
		const GibbsField& field;

		/*! \brief This potential's field-unique ID. */
		const unsigned int id;

		/*! \brief Construct a new potential with a specified ID and specified variable IDs. */
		GibbsPotential( const GibbsField& _field, unsigned int _id,
						const std::vector<unsigned int>& _variableIDs );

		/*! \brief Copy a potential but assign it to a different field. */
		GibbsPotential( const GibbsField& _field, const GibbsPotential& other );

		/*! \brief Return a smart pointer to a copy-constructed potential. */
		virtual GibbsPotential::Ptr Clone( const GibbsField& _field ) = 0;
		
		/*! \brief Return the exponent potential for this potential for
		 * the current values of its relevant variables. */
		virtual double CalculatePotential() = 0;

		// NOTE No way around this typedef hack
		/*! \brief Get the variables that this potential operates over. */
		std::vector < std::shared_ptr<GibbsVariable> > GetClique() const;

	private:

		/*! \brief The IDs corresponding to this potential's variables. */
		std::vector<unsigned int> variableIDs;

	};

	/*! \brief Superclass for all variables in a Gibbs field. Equivalent to a
	 * node in the Gibbs graph. */
	class GibbsVariable {
	public:

		typedef std::shared_ptr<GibbsVariable> Ptr;

		/*! \brief The field this variable belongs to. */
		const GibbsField& field;

		/*! \brief This variable's field-unique ID. */
		const unsigned int id;

		/*! \brief Construct a new variable with specified ID. */
		GibbsVariable( const GibbsField& _field, unsigned int _id );

		/*! \brief Copy a variable but assign it to a different field. */
		GibbsVariable( const GibbsField& _field, const GibbsVariable& other );

		/*! \brief Return a smart poitner to a copy-constructed variable. */
		virtual GibbsVariable::Ptr Clone( const GibbsField& _field ) = 0;
		
		/*! \brief Given a random sample in [0,1], sample this variable
		 * proportional to its potentials. */
		virtual void Sample( double rng ) = 0;

		/*! \brief Calculate the product of potentials connected
		 * to this variable. */
		double CalculatePotential();

		/*! \brief Get the potentials that operate on this variable. */
		std::vector<GibbsPotential::Ptr> GetPotentials() const;

		/*! \brief Adds an additional potential to this variable. */
		void AddPotential( unsigned int potID );
		
	private:

		/*! \brief The IDs corresponding to this variable's potentials. */
		std::vector<unsigned int> potentialIDs;
		
	};
	
	class GibbsField {
	public:

		GibbsField();
		GibbsField( const GibbsField& other );

		void AddVariable( const GibbsVariable::Ptr& var );
		void AddPotential( const GibbsPotential::Ptr& pot );

		GibbsVariable::Ptr GetVariable( unsigned int id ) const;
		std::vector<GibbsVariable::Ptr> GetVariables() const;
		std::size_t NumVariables() const;
		
		GibbsPotential::Ptr GetPotential( unsigned int id ) const;
		std::vector<GibbsPotential::Ptr> GetPotentials() const;
		std::size_t NumPotentials() const;

		double CalculateLogPotential();
		
	private:

		typedef std::unordered_map <unsigned int, GibbsVariable::Ptr> VariableMap;
		typedef std::unordered_map <unsigned int, GibbsPotential::Ptr> PotentialMap;
		
		/*! \brief Map from variable IDs to pointers. */
		VariableMap variables;

		/*! \brief Map from potential IDs to pointers. */
		PotentialMap potentials;
		
	};
	
}

#endif