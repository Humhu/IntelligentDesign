#ifndef _UNIFORM_DISTRIBUTION_H_
#define _UNIFORM_DISTRIBUTION_H_

#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_01.hpp>

namespace intelligent {

	// Samples an element from a vector in proportion to their values given a
	// random number in [0,1]
	unsigned int SampleNumberLine( const std::vector<double>& line, double rng );
	
	template < class Generator, class S = double >
	class DistributionBase {
	public:

		DistributionBase() :
			adapter( engine, generator ) {}

		void SetSeed( unsigned int seed ) {
			engine.seed( seed );
		}
		
	protected:

		typedef S ScalarType;
		typedef boost::mt19937 RandEngine;
		typedef Generator GeneratorType;
		typedef boost::variate_generator<RandEngine&, GeneratorType> RandAdapter;

		RandEngine engine;
		GeneratorType generator;
		RandAdapter adapter;

		DistributionBase::ScalarType SampleRaw() {
			return adapter();
		}
	};

	/*! \brief A continuous uniform random number generator. */
	class UniformDistribution : public DistributionBase< boost::uniform_01<double> > {
	public:

		UniformDistribution( double lower, double upper );

		void SetBounds( double lower, double upper );
		double GetLowerBound() const;
		double GetUpperBound() const;

		double Sample();
		double GetProb( double meas ) const;

	protected:

		boost::uniform_01<double> generator;

		double density;
		double lowerBound;
		double scale; // Difference of upper and lower

	};

	/*! \brief A univariate Gaussian random number generator. */
	class NormalDistribution : public DistributionBase< boost::normal_distribution<double> > {
	public:

		NormalDistribution( double _mean, double variance );

		void SetMean( double _mean );
		void SetVariance( double variance );

		double GetMean() const;
		double GetVariance() const;

		double Sample();
		double GetProb( double meas ) const;

	protected:

		double mean;
		double sigma; // Square root of variance (std dev)
		
	};

	class UniformIntegerDistribution : public DistributionBase< boost::uniform_int<> > {
	public:

		UniformIntegerDistribution( int lower, int upper );

		void SetBounds( int lower, int upper );

		int Sample();
		double GetProb( int meas ) const;

	protected:

		boost::uniform_int<> generator;
		int lower;
		int upper;
	};
	
}

#endif