#ifndef _TREE_SEARCH_H_
#define _TREE_SERACH_H_

#include <queue>
#include <stdexcept>

#include "intelligent/DiscreteAssembly.h"
#include "intelligent/AssemblySampler.h"

#include "intelligent/MinMaxHeap.hpp"

namespace intelligent {
	struct SearchEntry {
		double priority;
		DiscreteAssembly::Ptr assembly;
	};

	bool operator<( const intelligent::SearchEntry& lhs,
					const intelligent::SearchEntry& rhs );

	template <class C>
	struct Mat3D {
	public:

		Mat3D( unsigned int xS, unsigned int yS, unsigned int zS ) :
			xDim( xS ), yDim( yS ), zDim( zS ), data( nullptr ) {

			data = new (std::nothrow) C[xS*yS*zS];
			if( data == nullptr ) {
				throw std::runtime_error("Could not allocate");
			}
		}
		
		~Mat3D() {
			if( data != nullptr ) {
				delete[] data;
			}
		}

		void Zero() {
			memset( data, 0, xDim*yDim*zDim*sizeof(C) );
		}
		
		C& At( unsigned int x, unsigned int y, unsigned int z ) {
 			return data[ z*xDim*yDim + y*xDim + x ];
		}
		
// 		void Set( unsigned int x, unsigned int y, unsigned int z, const C& val ) {
// 			data[ z*xDim*yDim + y*xDim + x ] = val;
// 		}
		
	private:

		unsigned int xDim;
		unsigned int yDim;
		unsigned int zDim;
		C* data;
	};

	struct SearchProperties {
		
		ContinuousPoint3 desiredCOM;
		double volume;
		
		ContinuousPoint3 com;
		unsigned int totalBlocks;
		double totalMass;
		unsigned long totalWavefront;
		double zFill;
	};
	
	/*! \brief Class to perform tree search over discrete assemblies. */
	class TreeSearch {
	public:
		
		TreeSearch( AssemblySampler& _sampler );

		/*! \brief Specify the number of successors to generate at each node (branch factor) */
		void SetNumSuccessors( unsigned int n );

		/*! \brief Specify the number of times to run MCMC when generating successors. */
		void SetSampleDepth( unsigned int d );

		/*! \brief Specify maximum number of successors to keep in the queue. */
		void SetMaxQueueSize( unsigned int q );
		
		/*! \brief Add a discrete assembly to the search queue. */
		void Add(DiscreteAssembly::Ptr _da);

		/*! \brief Add a vector of discrete assemblies to the search queue. */
		void Add(const std::vector<DiscreteAssembly::Ptr> & _das);

		/*! \brief Retrieve and dequeue the assembly in the queue with the highest reward. */
		DiscreteAssembly::Ptr Next();

		/*! \brief Retrieve the assembly in the queue with the highest reward. */
		DiscreteAssembly::Ptr Peek();

		size_t Size();

		SearchProperties ComputeProperties( const DiscreteAssembly& da );
		
		/*! \brief Compute the cost/reward for an assembly. */
		double ComputeCost( const SearchProperties& properties);

		/*! \brief Check if an assembly has all blocks touching the ground. */
		bool CheckConnectivity( const DiscreteAssembly& da );

		/*! \brief Compute the sum shortest distances from each block to the ground. */
		unsigned long ComputeWavefront( const DiscreteAssembly& da );
		
	private:
		
		AssemblySampler& sampler;
		unsigned int numSuccessors;
		unsigned int sampleDepth;
		unsigned int maxQueueSize;
		
		/*! \brief Generates samples for the specified assembly. */
		std::vector<DiscreteAssembly::Ptr> 
		GetSuccessors( DiscreteAssembly::Ptr& _da );
		
// 		std::priority_queue<std::pair<double,DiscreteAssembly::Ptr>> pq;
		MinMaxHeap<SearchEntry> pq;
		
	};

}

#endif
