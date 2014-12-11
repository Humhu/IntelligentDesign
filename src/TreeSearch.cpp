#include "intelligent/TreeSearch.h"
#include "intelligent/DiscretePoint.h"
#include "intelligent/PotentialCOM.h"

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

namespace intelligent {

	bool operator<( const intelligent::SearchEntry& lhs,
					const intelligent::SearchEntry& rhs ) {
		return lhs.priority < rhs.priority;
	}
	
	TreeSearch::TreeSearch( AssemblySampler& _sampler ) :
		sampler( _sampler ),
		numSuccessors( 5 ),
		sampleDepth( 10 ),
		maxQueueSize( 400 ) {}

	void TreeSearch::SetNumSuccessors( unsigned int n ) {
		numSuccessors = n;
	}

	void TreeSearch::SetSampleDepth( unsigned int d ) {
		sampleDepth = d;
	}

	void TreeSearch::SetMaxQueueSize( unsigned int q ) {
		maxQueueSize = q;
	}
	
	void TreeSearch::Add(DiscreteAssembly::Ptr _da) {

		bool connected = CheckConnectivity( *_da );
		if( !connected ) {
			return;
		}

		SearchProperties properties = ComputeProperties( *_da );
		double cost = ComputeCost( properties );
		SearchEntry entry;
		entry.priority = -cost;
		entry.assembly = _da;
		pq.push( entry );

		if( pq.size() > maxQueueSize ) {
			pq.popMin();
		}
		
// 		pq.emplace(-cost, _da);
	}
	
	void TreeSearch::Add(const std::vector<DiscreteAssembly::Ptr> & _das) {
		for (auto & _da : _das) {
			Add(_da);
		}
	}
	
	DiscreteAssembly::Ptr TreeSearch::Next() {
		
		if (pq.empty()) { return DiscreteAssembly::Ptr(); }		
		SearchEntry myvar = pq.findMax();
		DiscreteAssembly::Ptr da = myvar.assembly;
		auto das = GetSuccessors( da );
		Add(das);
		return da;
	}
	
	DiscreteAssembly::Ptr TreeSearch::Peek() {
		if (pq.empty()) { return DiscreteAssembly::Ptr(); }
		return pq.findMax().assembly;
	}
	
	std::vector<DiscreteAssembly::Ptr> 
	TreeSearch::GetSuccessors( DiscreteAssembly::Ptr& _da) {
		sampler.SetBase( _da );
		return sampler.Sample( numSuccessors, sampleDepth );
	}

	size_t TreeSearch::Size() {
		return pq.size();
	}
	
	SearchProperties TreeSearch::ComputeProperties( const DiscreteAssembly& da ) {

		SearchProperties properties;

		properties.totalWavefront = ComputeWavefront( da );
		
		const auto & lattice = da.GetLattice();
		const std::vector<unsigned> & nodeIds = lattice.GetNodeIDs();
		const auto & bbox = lattice.GetBoundingBox();
		
		std::vector<GibbsVariable::Ptr> vars;
		for (const auto & nid : nodeIds) {
			vars.push_back( da.GetField().GetVariable(nid) );
		}
		
		double wx = bbox.maxX - bbox.minX;
		double wy = bbox.maxY - bbox.minY;
		double wz = bbox.maxZ - bbox.minZ;
		properties.volume = wx*wy*wz;
		
		properties.desiredCOM.x = bbox.minX + wx/2;
		properties.desiredCOM.y = bbox.minY + wy/2;
		properties.desiredCOM.z = bbox.minZ;

		BlockVariable::Ptr blockVariable;
		DiscretePoint3 blockPosition;
		double cx = 0.0, cy = 0.0, cz = 0.0;
		properties.totalMass = 0.0;
		properties.totalBlocks = 0;
		properties.zFill = 0;
		for (const auto & v : vars) {
			double mass = 0.0;
			blockVariable = std::dynamic_pointer_cast<BlockVariable>(v);
			blockPosition = lattice.GetNodePosition(blockVariable->id);
			switch (blockVariable->GetState()) {
				case BLOCK_FULL:  mass = 1.0; break;
				case BLOCK_HALF:  mass = 0.1; break;
				case BLOCK_EMPTY: mass = 0.0; break;
				default: throw std::runtime_error("Invalid block state");
			}
			
			properties.totalMass += mass;
			cx += mass * blockPosition.x;
			cy += mass * blockPosition.y;
			cz += mass * blockPosition.z;

			if( mass != 0 ) {
				properties.totalBlocks++;
				double height = blockPosition.z - bbox.minZ + 1;
				properties.zFill += height*height;
			}
		}

		double cDenom = properties.totalMass;
		if( cDenom == 0 ) {
			cDenom = 1;
		}
		properties.com.x = cx/cDenom;
		properties.com.y = cy/cDenom;
		properties.com.z = cz/cDenom;

		return properties;
	}

	double TreeSearch::ComputeCost( const SearchProperties& properties ) {

		double cdx = properties.com.x - properties.desiredCOM.x;
		double cdy = properties.com.y - properties.desiredCOM.y;
		double cdz = properties.com.z - properties.desiredCOM.z;

		double costComXY = std::exp( cdx*cdx + cdy*cdy );
		double costComZ = std::exp( cdz*cdz );

		double volDenom = properties.volume/100;
		double costTotalMass = std::exp( -3 + properties.totalMass/volDenom );
		double costTotalBlocks = std::exp( -3 + properties.totalBlocks/volDenom );
		double costHeight = properties.zFill;
		double costWave = properties.totalWavefront;
		
		double cost = 5*costComXY + 10*costComZ + 0.5*costTotalBlocks - 0.4*costHeight + costWave;
		return cost;
	}

	bool TreeSearch::CheckConnectivity( const DiscreteAssembly& da ) {

 		DiscreteBox3 bounds = da.GetLattice().GetBoundingBox();

		int xDim = bounds.maxX - bounds.minX + 1;
		int yDim = bounds.maxY - bounds.minY + 1;
		int zDim = bounds.maxZ - bounds.minZ + 1;

		Mat3D<char> connRef( xDim, yDim, zDim );
		DiscretePoint3 offset( bounds.minX, bounds.minY, bounds.minZ );

		const std::vector<unsigned int>& nodeIds = da.GetLattice().GetNodeIDs();
		BOOST_FOREACH( unsigned int id, nodeIds ) {

			GibbsVariable::Ptr gibbsVariable = da.GetField().GetVariable( id );
			BlockVariable::Ptr blockVariable = std::dynamic_pointer_cast<BlockVariable>( gibbsVariable );

			char val = 0;
			switch( blockVariable->GetState() ) {
				case BLOCK_FULL: val = 2; break;
				case BLOCK_HALF: val = 1; break;
				case BLOCK_EMPTY: val = 0; break;
			}

			DiscretePoint3 ind = da.GetLattice().GetNodePosition( id ) - offset;
			connRef.At( ind.x, ind.y, ind.z ) = val;
		}

		// 0 means empty, 1 means half, 2 means full, 3 means grounded

		// First we identify all ground connectivity points from the lowest level
		for( int x = 0; x < xDim; x++ ) {
			for( int y = 0; y < yDim; y++ ) {
				if( connRef.At(x,y,0) != 0 ) {
					connRef.At(x,y,0) = 3;
				}
			}
		}
		
		unsigned int numChanges = 1;	

		// Then we raster scan the interior
		while( numChanges != 0 ) {

			numChanges = 0;
			for( int z = 1; z < zDim-1; z++ ) {
				for( int x = 1; x < xDim-1; x++ ) {
					for( int y = 1; y < yDim-1; y++ ) {

						double val = connRef.At(x,y,z);
						// Don't need to update empty blocks
						if( val == 0 ) { continue; }
						else if( val == 3 ) {
							if( connRef.At(x-1,y,z) != 0 && connRef.At(x-1,y,z) != 3 ) {
								connRef.At(x-1,y,z) = 3;
								numChanges++;
							}
							if( connRef.At(x+1,y,z) != 0 && connRef.At(x+1,y,z) != 3 ) {
								connRef.At(x+1,y,z) = 3;
								numChanges++;
							}
							if( connRef.At(x,y-1,z) != 0 && connRef.At(x,y-1,z) != 3 ) {
								connRef.At(x,y-1,z) = 3;
								numChanges++;
							}
							if( connRef.At(x,y+1,z) != 0 && connRef.At(x,y+1,z) != 3 ) {
								connRef.At(x,y+1,z) = 3;
								numChanges++;
							}
							if( connRef.At(x,y,z-1) != 0 && connRef.At(x,y,z-1) != 3 ) {
								connRef.At(x,y,z-1) = 3;
								numChanges++;
							}
							if( connRef.At(x,y,z+1) != 0 && connRef.At(x,y,z+1) != 3 ) {
								connRef.At(x,y,z+1) = 3;
								numChanges++;
							}
							continue;
						}

						else if( connRef.At(x-1,y,z) == 3 || connRef.At(x+1,y,z) == 3 ||
								 connRef.At(x,y-1,z) == 3 || connRef.At(x,y+1,z) == 3 ||
								 connRef.At(x,y,z-1) == 3 || connRef.At(x,y,z+1) == 3 ) {
							connRef.At(x,y,z) = 3;
							numChanges++;
						}
						
					}
				}
			}
		}

		// Finally see if any cells are not 0 or 3
		for( int x = 1; x < xDim-1; x++ ) {
			for( int y = 1; y < yDim-1; y++ ) {
				for( int z = 1; z < zDim-1; z++ ) {
					if( connRef.At(x,y,z) != 0 && connRef.At(x,y,z) != 3 ) {
						return false;
					}
				}
			}
		}
		return true;		
		
	}

	unsigned long TreeSearch::ComputeWavefront( const DiscreteAssembly& da ) {
		
		DiscreteBox3 bounds = da.GetLattice().GetBoundingBox();
		// 		DiscreteBox3 bounds;
		// 		bounds.maxX = 1;
		// 		bounds.maxY = 1;
		// 		bounds.maxZ = 1;
		
		int xDim = bounds.maxX - bounds.minX + 1;
		int yDim = bounds.maxY - bounds.minY + 1;
		int zDim = bounds.maxZ - bounds.minZ + 1;
		
		Mat3D<int> connRef( xDim, yDim, zDim );
		DiscretePoint3 offset( bounds.minX, bounds.minY, bounds.minZ );
		
		const std::vector<unsigned int>& nodeIds = da.GetLattice().GetNodeIDs();
		BOOST_FOREACH( unsigned int id, nodeIds ) {
			
			GibbsVariable::Ptr gibbsVariable = da.GetField().GetVariable( id );
			BlockVariable::Ptr blockVariable = std::dynamic_pointer_cast<BlockVariable>( gibbsVariable );
			
			int val = 0;
			switch( blockVariable->GetState() ) {
				case BLOCK_FULL: val = std::numeric_limits<int>::max() - 1; break;
				case BLOCK_HALF: val = std::numeric_limits<int>::max() - 1; break;
				case BLOCK_EMPTY: val = -1; break;
			}
			
			DiscretePoint3 ind = da.GetLattice().GetNodePosition( id ) - offset;
			connRef.At( ind.x, ind.y, ind.z ) = val;
		}
		
		// -1 means empty, 0 and over is # steps to ground level
		
		// First we identify all ground connectivity points from the lowest level
		for( int x = 0; x < xDim; x++ ) {
			for( int y = 0; y < yDim; y++ ) {
				if( connRef.At(x,y,0) != -1 ) {
					connRef.At(x,y,0) = 0;
				}
			}
		}
		
		unsigned int numChanges = 1;

		unsigned int iters = 0;
		// Then we raster scan the interior
		while( numChanges != 0 ) {
			
			numChanges = 0;
			iters++;
			for( int z = 1; z < zDim-1; z++ ) {
				for( int x = 1; x < xDim-1; x++ ) {
					for( int y = 1; y < yDim-1; y++ ) {
						
						int val = connRef.At(x,y,z);
						// Don't need to update empty blocks
						if( val == -1 ) { continue; }

						// Grab lowest neighbor
						if( connRef.At(x-1,y,z) != -1 && connRef.At(x-1,y,z) + 1 < val ) {
							connRef.At(x,y,z) = connRef.At(x-1,y,z) + 1;
							val = connRef.At(x,y,z);
							numChanges++;
						}
						if( connRef.At(x+1,y,z) != -1 && connRef.At(x+1,y,z) + 1 < val ) {
							connRef.At(x,y,z) = connRef.At(x+1,y,z) + 1;
							val = connRef.At(x,y,z);
							
							numChanges++;
						}
						if( connRef.At(x,y-1,z) != -1 && connRef.At(x,y-1,z) + 1 < val ) {
							connRef.At(x,y,z) = connRef.At(x,y-1,z) + 1;
							val = connRef.At(x,y,z);
							
							numChanges++;
						}
						if( connRef.At(x,y+1,z) != -1 && connRef.At(x,y+1,z) + 1 < val ) {
							connRef.At(x,y,z) = connRef.At(x,y+1,z) + 1;
							val = connRef.At(x,y,z);
							
							numChanges++;
						}
						if( connRef.At(x,y,z-1) != -1 && connRef.At(x,y,z-1) + 1 < val ) {
							connRef.At(x,y,z) = connRef.At(x,y,z-1) + 1;
							val = connRef.At(x,y,z);
							
							numChanges++;
						}
						if( connRef.At(x,y,z+1) != -1 && connRef.At(x,y,z+1) + 1 < val ) {
							connRef.At(x,y,z) = connRef.At(x,y,z+1) + 1;
							val = connRef.At(x,y,z);
							
							numChanges++;
						}

						// Assign to higher neighbors
						if( connRef.At(x-1,y,z) != -1 && connRef.At(x-1,y,z) > val + 1 ) {
							connRef.At(x-1,y,z) = val + 1;
							numChanges++;
						}
						if( connRef.At(x+1,y,z) != -1 && connRef.At(x+1,y,z) > val + 1 ) {
							connRef.At(x+1,y,z) = val + 1;
							numChanges++;
						}
						if( connRef.At(x,y-1,z) != -1 && connRef.At(x,y-1,z) > val + 1 ) {
							connRef.At(x,y-1,z) = val + 1;
							numChanges++;
						}
						if( connRef.At(x,y+1,z) != -1 && connRef.At(x,y+1,z) > val + 1 ) {
							connRef.At(x,y+1,z) = val + 1;
							numChanges++;
						}
						if( connRef.At(x,y,z-1) != -1 && connRef.At(x,y,z-1) > val+1 ) {
							connRef.At(x,y,z-1) = val + 1;
							numChanges++;
						}
						if( connRef.At(x,y,z+1) != -1 && connRef.At(x,y,z+1) > val+1 ) {
							connRef.At(x,y,z+1) = val + 1;
							numChanges++;
						}
					}
				}
			}
		}

		// Finally sum the distances
		unsigned long sum = 0;
		for( int x = 0; x < xDim; x++ ) {
			for( int y = 0; y < yDim; y++ ) {
				for( int z = 0; z < zDim; z++ ) {
					int val = connRef.At(x,y,z);
					if( val != -1 ) {
						sum += val;
					}
				}
			}
		}
		return sum;
		
	}
	
}

