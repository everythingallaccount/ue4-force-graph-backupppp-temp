#pragma once

#include "CoreMinimal.h"

struct OctreeNode {
	FVector Center;
	FVector Extent;
	TArray<OctreeNode*> Children;
	FVector* DataPoint = nullptr;
	FVector CenterOfMass;
	int TotalDataPoints = 0;

	OctreeNode(FVector center, FVector extent);  // Constructor
	~OctreeNode();  // Destructor

	bool IsLeaf() const;
	bool ContainsPoint(const FVector point) const;

	void Subdivide();
	void CalculateCenterOfMass();
};

void AddDataPoint(OctreeNode* node, FVector newPoint);