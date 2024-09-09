#pragma once

#include "CoreMinimal.h"
// #include "PointData.h" // Make sure this include points to your actual PointData structure file location

struct PointData {
	FVector Position;
	FVector Velocity;

	PointData(FVector position = FVector(), FVector velocity = FVector())
		: Position(position), Velocity(velocity) {}

	// Optionally add methods that manipulate the point data if needed
};

struct OctreeNode {
	FVector Center;
	FVector Extent;
	TArray<OctreeNode*> Children;
	PointData* Data = nullptr; 
	FVector CenterOfMass;
	int TotalDataPoints = 0;

	OctreeNode(FVector center, FVector extent);
	~OctreeNode();

	bool IsLeaf() const;
	bool ContainsPoint(const FVector point) const;
    
	void Subdivide();
	void CalculateCenterOfMass();
};

void AddDataPoint(OctreeNode* node, FVector newPoint, FVector velocity);