#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "KnowledgeNode.h"
// #include "PointData.h" // Make sure this include points to your actual PointData structure file location

struct PointData {
	FVector Position;
	FVector Velocity;
	AKnowledgeNode* Node;

	PointData(
		FVector position = FVector(),
		FVector velocity = FVector()
		)
		: Position(position), Velocity(velocity)
	{
		
	}

	// Optionally add methods that manipulate the point data if needed
};

struct OctreeNode {
	FVector Center;
	FVector Extent;
	TArray<OctreeNode*> Children;
	PointData* Data = nullptr; 
	FVector CenterOfMass;
	int TotalDataPoints = 0;



	// default to be zero vector.  
	OctreeNode(FVector center=FVector(0,0,0),
		FVector extent=FVector(0,0,0)
		);


	~OctreeNode();

	bool IsLeaf() const;
	bool ContainsPoint(const FVector point) const;
    
	void Subdivide();
	void CalculateCenterOfMass();
};

void AddDataPoint(OctreeNode* node, FVector newPoint, FVector velocity);


// Define the Callback Type
using OctreeCallback = std::function<bool(OctreeNode*)>;

// Declare the BFS traversal function
void TraverseBFS(OctreeNode* root, OctreeCallback callback);

bool SampleCallback(OctreeNode* node);