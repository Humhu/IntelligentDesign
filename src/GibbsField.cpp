#include "intelligent/GibbsField.h"

#include <boost/foreach.hpp>

#include <stdexcept>
#include <sstream>
#include <iostream>
#include <cmath>

namespace intelligent {
	
	GibbsPotential::GibbsPotential( const GibbsField& _field, unsigned int _id,
									const std::vector<unsigned int>& _variableIDs ) :
		field( _field ),
		id( _id ),
		variableIDs( _variableIDs ) {}

	GibbsPotential::GibbsPotential( const GibbsField& _field,
									const GibbsPotential& other ) :
		field( _field ),
		id( other.id ),
		variableIDs( other.variableIDs ) {}

	std::vector<GibbsVariable::Ptr> GibbsPotential::GetClique() const {
		
		std::vector<GibbsVariable::Ptr> clique;
		BOOST_FOREACH( unsigned int varID, variableIDs ) {
			clique.push_back( field.GetVariable( varID ) );
		}
		return clique;
	}
		
	GibbsVariable::GibbsVariable( const GibbsField& _field, unsigned int _id ) :
		field( _field ),
		id( _id ) {}

	GibbsVariable::GibbsVariable( const GibbsField& _field, const GibbsVariable& other ) :
		field( _field ),
		id( other.id ),
		potentialIDs( other.potentialIDs ) {}

	double GibbsVariable::CalculatePotential() {

		double prod = 1.0;
		std::vector <GibbsPotential::Ptr> potentials = GetPotentials();
		BOOST_FOREACH( const GibbsPotential::Ptr& pot, potentials ) {
			prod *= pot->CalculatePotential();
		}

		return prod;
	}

	std::vector<GibbsPotential::Ptr> GibbsVariable::GetPotentials() const {
		
		std::vector<GibbsPotential::Ptr> potentials;
		BOOST_FOREACH( unsigned int potID, potentialIDs ) {
			potentials.push_back( field.GetPotential( potID ) );
		}
		return potentials;
	}

	void GibbsVariable::AddPotential( unsigned int potID ) {
		potentialIDs.push_back( potID );
	}
		
	GibbsField::GibbsField() {}

	GibbsField::GibbsField( const GibbsField& other ) {

		BOOST_FOREACH( const VariableMap::value_type& item, other.variables ) {
			GibbsVariable::Ptr var = item.second->Clone( *this );
			AddVariable( var );
		}

		BOOST_FOREACH( const PotentialMap::value_type& item, other.potentials ) {
			GibbsPotential::Ptr pot = item.second->Clone( *this );
			AddPotential( pot );
		}
		
	}

	void GibbsField::AddVariable( const GibbsVariable::Ptr& var ) {
		
		if( variables.count( var->id ) > 0 ) {
			std::stringstream ss;
			ss << "Field already has variable with id " << var->id;
			throw std::runtime_error( ss.str() );
		}
		variables[ var->id ] = var;
	}

	void GibbsField::AddPotential( const GibbsPotential::Ptr& pot ) {
		
		if( potentials.count( pot->id ) > 0 ) {
			std::stringstream ss;
			ss << "Field already has potential with id " << pot->id;
			throw std::runtime_error( ss.str() );
		}
		potentials[ pot->id ] = pot;
	}

	GibbsVariable::Ptr GibbsField::GetVariable( unsigned int id ) const {
		return variables.at( id );
	}

	GibbsPotential::Ptr GibbsField::GetPotential( unsigned int id ) const {
		return potentials.at( id );
	}
	
	std::vector<GibbsVariable::Ptr> GibbsField::GetVariables() const {
		std::vector<GibbsVariable::Ptr> vars( variables.size() );
		unsigned int idx = 0;
		BOOST_FOREACH( const VariableMap::value_type& item, variables ) {
			vars[idx] = item.second;
			idx++;
		}
		return vars;
	}

	std::vector<GibbsPotential::Ptr> GibbsField::GetPotentials() const {
		std::vector<GibbsPotential::Ptr> pots( potentials.size() );
		BOOST_FOREACH( const PotentialMap::value_type& item, potentials) {
			pots.push_back( item.second );
		}
		return pots;
	}

	std::size_t GibbsField::NumVariables() const {
		return variables.size();
	}

	std::size_t GibbsField::NumPotentials() const {
		return potentials.size();
	}
	
}