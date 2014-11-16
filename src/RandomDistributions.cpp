#include "intelligent/RandomDistributions.h"

#include <sstream>

namespace intelligent {

	unsigned int SampleNumberLine( const std::vector<double>& line, double rng ) {

		std::vector<double> acc( line.size() );
		double z = 0;
		for( unsigned int i = 0; i < line.size(); i++ ) {
			z += line[i];
			acc[i] = z;
		}

		rng = rng*z; // Scale up to max value
		for( unsigned int i = 0; i < acc.size(); i++ ) {
			if( rng < acc[i] ) {
				return i;
			}
		}
		return acc.size();
		
	}
	
	UniformDistribution::UniformDistribution( double lower, double upper ) {
		SetBounds( lower, upper );
	}

	void UniformDistribution::SetBounds( double lower, double upper ) {
		if( lower >= upper ) {
			std::stringstream ss;
			ss << "Lower bound " << lower << " is less than upper bound " << upper;
			throw std::logic_error( ss.str() );
		}

		lowerBound = lower;
		scale = upper - lower;
		density = 1.0/scale;
	}

	double UniformDistribution::GetLowerBound() const {
		return lowerBound;
	}

	double UniformDistribution::GetUpperBound() const {
		return lowerBound + scale;
	}

	double UniformDistribution::Sample() {
		ScalarType raw = SampleRaw();
		ScalarType transformed = lowerBound + raw*scale;
		return transformed;
	}

	double UniformDistribution::GetProb( double meas ) const {
		
		if( meas < lowerBound || meas > (lowerBound + scale) ) {
			return 0.0;
		}
		return density;
	}

	NormalDistribution::NormalDistribution( double _mean, double variance ) {
		SetMean( _mean );
		SetVariance( variance );
	}

	void NormalDistribution::SetMean( double _mean ) {
		mean = _mean;
	}

	void NormalDistribution::SetVariance( double variance ) {
		if( variance < 0 ) {
			std::stringstream ss;
			ss << "Variance " << variance << " is negative." << std::endl;
			throw std::logic_error( ss.str() );
		}
		sigma = std::sqrt( variance );
	}

	double NormalDistribution::GetMean() const {
		return mean;
	}

	double NormalDistribution::GetVariance() const {
		return sigma*sigma;
	}

	double NormalDistribution::Sample() {
		ScalarType raw = SampleRaw();
		ScalarType transformed = sigma*raw + mean;
		return transformed;
	}
	
	double NormalDistribution::GetProb( double meas ) const {
		double a = 1/(sigma*sqrt(2*M_PI));
		double b = mean;
		double c = sigma;
		double d = 0;
		double y = a * std::exp(-std::pow((meas-b),2) / (2*std::pow(c,2))) + d;
		return y;		
	}

	UniformIntegerDistribution::UniformIntegerDistribution( int _lower, int _upper ) :
		lower( _lower ), upper( _upper ) {}

	void UniformIntegerDistribution::SetBounds( int _lower, int _upper ) {
		lower = _lower;
		upper = _upper;
		generator = GeneratorType( lower, upper );
	}

	int UniformIntegerDistribution::Sample() {
		return SampleRaw();
	}

	double UniformIntegerDistribution::GetProb( int meas ) const {
		if( meas > lower && meas < upper ) {
			return 1.0/std::abs(upper - lower );
		}
		return 0;
	}
	
}