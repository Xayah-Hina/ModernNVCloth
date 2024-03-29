// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2020 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

#include "foundation/PxStrideIterator.h"
#include "NvClothExt/ClothMeshQuadifier.h"

// from shared foundation
#include "ps/PsSort.h"
#include "NvCloth/ps/Ps.h"
#include "NvCloth/ps/PsMathUtils.h"
#include "NvCloth/Allocator.h"

using namespace physx;

namespace nv
{
namespace cloth
{

struct ClothMeshQuadifierImpl : public ClothMeshQuadifier
{
	virtual bool quadify(const ClothMeshDesc& desc) override;
	ClothMeshDesc getDescriptor() const override;

public:
	ClothMeshDesc mDesc;
	nv::cloth::Vector<PxU32>::Type mQuads;
	nv::cloth::Vector<PxU32>::Type mTriangles;
};

namespace 
{
	struct UniqueEdge
	{
		PX_FORCE_INLINE bool operator()(const UniqueEdge& e1, const UniqueEdge& e2) const
		{
			return e1 < e2;
		}

		PX_FORCE_INLINE bool operator==(const UniqueEdge& other) const
		{
			return vertex0 == other.vertex0 && vertex1 == other.vertex1;
		}
		PX_FORCE_INLINE bool operator<(const UniqueEdge& other) const
		{
			if (vertex0 != other.vertex0)
			{
				return vertex0 < other.vertex0;
			}

			return vertex1 < other.vertex1;
		}

		///////////////////////////////////////////////////////////////////////////////
		UniqueEdge() 
			: vertex0(0), vertex1(0), vertex2(0), vertex3(0xffffffff),
			maxAngle(0.0f), isQuadDiagonal(false), isUsed(false) {}

		UniqueEdge(PxU32 v0, PxU32 v1, PxU32 v2) 
		    : vertex0(PxMin(v0, v1)), vertex1(PxMax(v0, v1)), vertex2(v2), vertex3(0xffffffff),
			maxAngle(0.0f), isQuadDiagonal(false), isUsed(false) {}

		
		PxU32 vertex0, vertex1;
		PxU32 vertex2, vertex3;
		PxF32 maxAngle;
		bool isQuadDiagonal;
		bool isUsed;
	};

	struct SortHiddenEdges
	{
		SortHiddenEdges(nv::cloth::Vector<UniqueEdge>::Type& uniqueEdges) : mUniqueEdges(uniqueEdges) {}

		bool operator()(PxU32 a, PxU32 b) const
		{
			return mUniqueEdges[a].maxAngle < mUniqueEdges[b].maxAngle;
		}

	private:
		SortHiddenEdges& operator=(const SortHiddenEdges&);
		nv::cloth::Vector<UniqueEdge>::Type& mUniqueEdges;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	void copyIndices(const ClothMeshDesc &desc, nv::cloth::Vector<PxU32>::Type &triangles, nv::cloth::Vector<PxU32>::Type &quads)
	{
		triangles.resize(desc.triangles.count*3);
		PxStrideIterator<const T> tIt = PxMakeIterator(reinterpret_cast<const T*>(desc.triangles.data), desc.triangles.stride);
		for(PxU32 i=0; i<desc.triangles.count; ++i, ++tIt)
			for(PxU32 j=0; j<3; ++j)
				triangles[i*3+j] = tIt.ptr()[j];

		quads.resize(desc.quads.count*4);
		PxStrideIterator<const T> qIt = PxMakeIterator(reinterpret_cast<const T*>(desc.quads.data), desc.quads.stride);
		for(PxU32 i=0; i<desc.quads.count; ++i, ++qIt)
			for(PxU32 j=0; j<4; ++j)
				quads[i*4+j] = qIt.ptr()[j];
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void computeUniqueEdges(nv::cloth::Vector<UniqueEdge>::Type &uniqueEdges, const PxVec3* positions, const nv::cloth::Vector<PxU32>::Type& triangles)
	{		
		uniqueEdges.resize(0);
		uniqueEdges.reserve(triangles.size());

		PxU32 indexMap[3][3] = { { 0, 1, 2 }, { 1, 2, 0 }, { 0, 2, 1 } };

		const PxF32 rightAngle = PxCos(ps::degToRad(85.0f));

		for(PxU32 i=0; i<triangles.size(); i+=3)
		{
			UniqueEdge edges[3];
			PxF32 edgeLengths[3];
			PxF32 edgeAngles[3];

			for (PxU32 j = 0; j < 3; j++)
			{
				edges[j] = UniqueEdge(triangles[i+indexMap[j][0]], triangles[i+indexMap[j][1]], triangles[i+indexMap[j][2]]);
				edgeLengths[j] = (positions[edges[j].vertex0] - positions[edges[j].vertex1]).magnitude();
				const PxVec3 v1 = positions[edges[j].vertex2] - positions[edges[j].vertex0];
				const PxVec3 v2 = positions[edges[j].vertex2] - positions[edges[j].vertex1];
				edgeAngles[j] = PxAbs(v1.dot(v2)) / (v1.magnitude() * v2.magnitude());
			}

			// find the longest edge
			PxU32  longest = 0;
			for (PxU32 j = 1; j < 3; j++)
			{
				if (edgeLengths[j] > edgeLengths[longest])
					longest = j;
			}

			// check it's angle
			if (edgeAngles[longest] < rightAngle)
				edges[longest].isQuadDiagonal = true;
		
			for (PxU32 j = 0; j < 3; j++)
				uniqueEdges.pushBack(edges[j]);
		}

		ps::sort(uniqueEdges.begin(), uniqueEdges.size(), UniqueEdge(0, 0, 0), ps::NonTrackingAllocator());

		PxU32 writeIndex = 0, readStart = 0, readEnd = 0;
		PxU32 numQuadEdges = 0;
		while (readEnd < uniqueEdges.size())
		{
			while (readEnd < uniqueEdges.size() && uniqueEdges[readStart] == uniqueEdges[readEnd])
				readEnd++;

			const PxU32 count = readEnd - readStart;

			UniqueEdge uniqueEdge = uniqueEdges[readStart];

			if (count == 2)
				// know the other diagonal
				uniqueEdge.vertex3 = uniqueEdges[readStart + 1].vertex2;
			else
				uniqueEdge.isQuadDiagonal = false;

			for (PxU32 i = 1; i < count; i++)
				uniqueEdge.isQuadDiagonal &= uniqueEdges[readStart + i].isQuadDiagonal;

			numQuadEdges += uniqueEdge.isQuadDiagonal ? 1 : 0;

			uniqueEdges[writeIndex] = uniqueEdge;

			writeIndex++;
			readStart = readEnd;
		}

		uniqueEdges.resize(writeIndex, UniqueEdge(0, 0, 0));
	}

	///////////////////////////////////////////////////////////////////////////////
	PxU32 findUniqueEdge(const nv::cloth::Vector<UniqueEdge>::Type &uniqueEdges, PxU32 index1, PxU32 index2)
	{
		UniqueEdge searchFor(index1, index2, 0);

		PxU32 curMin = 0;
		PxU32 curMax = uniqueEdges.size();
		while (curMax > curMin)
		{
			PxU32 middle = (curMin + curMax) >> 1;

			const UniqueEdge& probe = uniqueEdges[middle];
			if (probe < searchFor)
				curMin = middle + 1;
			else
				curMax = middle;		
		}

		return curMin;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void refineUniqueEdges(nv::cloth::Vector<UniqueEdge>::Type &uniqueEdges, const PxVec3* positions)
	{
		nv::cloth::Vector<PxU32>::Type hideEdges;
		hideEdges.reserve(uniqueEdges.size());

		for (PxU32 i = 0; i < uniqueEdges.size(); i++)
		{
			UniqueEdge& uniqueEdge = uniqueEdges[i];
			uniqueEdge.maxAngle = 0.0f;
			uniqueEdge.isQuadDiagonal = false; // just to be sure

			if (uniqueEdge.vertex3 != 0xffffffff)
			{
				PxU32 indices[4] = { uniqueEdge.vertex0, uniqueEdge.vertex2, uniqueEdge.vertex1, uniqueEdge.vertex3 };

				// compute max angle of the quad
				for (PxU32 j = 0; j < 4; j++)
				{
					PxVec3 e0 = positions[indices[ j + 0   ]] - positions[indices[(j + 1) % 4]];
					PxVec3 e1 = positions[indices[(j + 1) % 4]] - positions[indices[(j + 2) % 4]];

					//From physx
					//PxF32 cosAngle = PxAbs(e0.dot(e1)) / (e0.magnitude() * e1.magnitude());
					//uniqueEdge.maxAngle = PxMax(uniqueEdge.maxAngle, cosAngle);

					//From Apex
					float denominator = e0.magnitude() * e1.magnitude();
					if (denominator != 0.0f)
					{
						float cosAngle =  PxAbs(e0.dot(e1)) / denominator;
						uniqueEdge.maxAngle =  PxMax(uniqueEdge.maxAngle, cosAngle);
					}
				}

				hideEdges.pushBack(i);
			}
		}

		ps::sort(hideEdges.begin(), hideEdges.size(), SortHiddenEdges(uniqueEdges), ps::NonTrackingAllocator());

		const PxF32 maxAngle = PxSin(ps::degToRad(60.0f));

		PxU32 numHiddenEdges = 0;

		for (PxU32 i = 0; i < hideEdges.size(); i++)
		{
			UniqueEdge& uniqueEdge = uniqueEdges[hideEdges[i]];

			// find some stop criterion
			if (uniqueEdge.maxAngle > maxAngle)
				break;
		
			// check if all four adjacent edges are still visible?
			PxU32 indices[5] = { uniqueEdge.vertex0, uniqueEdge.vertex2, uniqueEdge.vertex1, uniqueEdge.vertex3, uniqueEdge.vertex0 };

			PxU32 numVisible = 0;
			for (PxU32 j = 0; j < 4; j++)
			{
				const PxU32 edgeIndex = findUniqueEdge(uniqueEdges, indices[j], indices[j + 1]);
				NV_CLOTH_ASSERT(edgeIndex < uniqueEdges.size());
	
				numVisible += uniqueEdges[edgeIndex].isQuadDiagonal ? 0 : 1;
			}

			if (numVisible == 4)
			{
				uniqueEdge.isQuadDiagonal = true;
				numHiddenEdges++;
			}
		}
	}


	// calculate the inclusive prefix sum, equivalent of std::partial_sum
	template <typename T>
	void prefixSum(const T* first, const T* last, T* dest)
	{
		if (first != last)
		{	
			*(dest++) = *(first++);
			for (; first != last; ++first, ++dest)
				*dest = *(dest-1) + *first;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void quadifyTriangles(const nv::cloth::Vector<UniqueEdge>::Type &uniqueEdges, nv::cloth::Vector<PxU32>::Type& triangles, nv::cloth::Vector<PxU32>::Type &quads)
	{
		nv::cloth::Vector<PxU32>::Type valency(uniqueEdges.size()+1, 0); // edge valency
		nv::cloth::Vector<PxU32>::Type adjacencies; // adjacency from unique edge to triangles
		PxU32 numTriangles = triangles.size() / 3;

		// compute edge valency w.r.t triangles
		for(PxU32 i=0; i<numTriangles; ++i)
		{
			for (PxU32 j=0; j < 3; j++)
			{
				PxU32 uniqueEdgeIndex = findUniqueEdge(uniqueEdges, triangles[i*3+j], triangles[i*3+(j+1)%3]);
				++valency[uniqueEdgeIndex];
			}
		}

		// compute adjacency from each edge to triangle, the value also encodes which side of the triangle this edge belongs to
		prefixSum(valency.begin(), valency.end(), valency.begin());
		adjacencies.resize(valency.back());
		for(PxU32 i=0; i<numTriangles; ++i)
		{
			for (PxU32 j=0; j < 3; j++)
			{
				PxU32 uniqueEdgeIndex = findUniqueEdge(uniqueEdges, triangles[i*3+j], triangles[i*3+(j+1)%3]);
				adjacencies[--valency[uniqueEdgeIndex]] = i*3+j;
			}
		}

		// now go through unique edges that are identified as diagonal, and build a quad out of two adjacent triangles
		nv::cloth::Vector<PxU32>::Type mark(numTriangles, 0);
		for (PxU32 i = 0; i < uniqueEdges.size(); i++)
		{
			const UniqueEdge& edge = uniqueEdges[i];
			if (edge.isQuadDiagonal)
			{
				PxU32 vi = valency[i];
				if ((valency[i+1]-vi) != 2)
					continue; // we do not quadify around non-manifold edges

				PxU32 adj0 = adjacencies[vi], adj1 = adjacencies[vi+1];
				PxU32 tid0 = adj0 / 3, tid1 = adj1 / 3;
				PxU32 eid0 = adj0 % 3, eid1 = adj1 % 3;

				quads.pushBack(triangles[tid0 * 3 + eid0]);
				quads.pushBack(triangles[tid1 * 3 + (eid1+2)%3]);
				quads.pushBack(triangles[tid0 * 3 + (eid0+1)%3]);
				quads.pushBack(triangles[tid0 * 3 + (eid0+2)%3]);

				mark[tid0] = 1;
				mark[tid1] = 1;
#if 0 // PX_DEBUG
				printf("Deleting %d, %d, %d - %d, %d, %d, creating %d, %d, %d, %d\n",
					triangles[tid0*3],triangles[tid0*3+1],triangles[tid0*3+2],
					triangles[tid1*3],triangles[tid1*3+1],triangles[tid1*3+2],
					v0,v3,v1,v2);
#endif
			}
		}

		// add remaining triangles that are not marked as already quadified
		nv::cloth::Vector<PxU32>::Type oldTriangles = triangles;
		triangles.resize(0);
		for (PxU32 i = 0; i < numTriangles; i++)
		{
			if (mark[i]) continue;

			triangles.pushBack(oldTriangles[i*3]);
			triangles.pushBack(oldTriangles[i*3+1]);
			triangles.pushBack(oldTriangles[i*3+2]);
		}
	}

} // namespace 


///////////////////////////////////////////////////////////////////////////////
bool ClothMeshQuadifierImpl::quadify(const ClothMeshDesc &desc)
{
	mDesc = desc;
	nv::cloth::Vector<PxVec3>::Type particles(desc.points.count);
	PxStrideIterator<const PxVec3> pIt(reinterpret_cast<const PxVec3*>(desc.points.data), desc.points.stride);
	for(PxU32 i=0; i<desc.points.count; ++i)
		particles[i] = *pIt++;

	// copy triangle indices
	if(desc.flags & MeshFlag::e16_BIT_INDICES)
		copyIndices<PxU16>(desc, mTriangles, mQuads);
	else
		copyIndices<PxU32>(desc, mTriangles, mQuads);

	nv::cloth::Vector<UniqueEdge>::Type uniqueEdges;

	computeUniqueEdges(uniqueEdges, particles.begin(), mTriangles);

	refineUniqueEdges(uniqueEdges, particles.begin());

//	printf("before %d triangles, %d quads\n", mTriangles.size()/3, mQuads.size()/4);
	quadifyTriangles(uniqueEdges, mTriangles, mQuads);

//	printf("after %d triangles, %d quads\n", mTriangles.size()/3, mQuads.size()/4);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
ClothMeshDesc 
ClothMeshQuadifierImpl::getDescriptor() const
{
	// copy points and other data
	ClothMeshDesc desc = mDesc;

	// for now use only 32 bit for temporary indices out of quadifier
	desc.flags &= ~MeshFlag::e16_BIT_INDICES;

	desc.triangles.count = mTriangles.size() / 3;
	desc.triangles.data = mTriangles.begin();
	desc.triangles.stride = 3 * sizeof(PxU32);

	desc.quads.count = mQuads.size() / 4;
	desc.quads.data = mQuads.begin();
	desc.quads.stride = 4 * sizeof(PxU32);

	NV_CLOTH_ASSERT(desc.isValid());

	return desc;
}

} // namespace cloth
} // namespace nv

NV_CLOTH_API(nv::cloth::ClothMeshQuadifier*) NvClothCreateMeshQuadifier()
{
	return NV_CLOTH_NEW(nv::cloth::ClothMeshQuadifierImpl);
}
