// #include "octreeeeeeeeee2.h"
//
//
//
// OctreeNode::OctreeNode(FVector center, FVector extent) :
// Center(center),
// Extent(extent),
// CenterOfMass(center),
// TotalDataPoints(0) {
// 	Children.SetNum(8, false);
// }
//
// OctreeNode::~OctreeNode() {
// 	for (auto* child : Children) {
// 		delete child;
// 	}
// 	if (DataPoint != nullptr) {
// 		delete DataPoint;
// 	}
// }
//
// bool OctreeNode::IsLeaf() const {
// 	return Children[0] == nullptr;
// }
//
// bool OctreeNode::ContainsPoint(const FVector point) const {
// 	return (FMath::Abs(point.X - Center.X) <= Extent.X &&
// 			FMath::Abs(point.Y - Center.Y) <= Extent.Y &&
// 			FMath::Abs(point.Z - Center.Z) <= Extent.Z);
// }
//
// void OctreeNode::Subdivide() {
// 	if (!IsLeaf()) return;
//
// 	FVector NewExtent = Extent * 0.5;
// 	delete DataPoint;
// 	DataPoint = nullptr;
//
// 	for (int i = 0; i < 8; ++i) {
// 		FVector NewCenter = Center + FVector(
// 			(i & 4) ? NewExtent.X : -NewExtent.X,
// 			(i & 2) ? NewExtent.Y : -NewExtent.Y,
// 			(i & 1) ? NewExtent.Z : -NewExtent.Z
// 		);
// 		Children[i] = new OctreeNode(NewCenter, NewExtent);
// 	}
// 	TotalDataPoints = 0;
// }
//
// void OctreeNode::CalculateCenterOfMass() {
// 	if (IsLeaf()) {
// 		if (DataPoint) {
// 			CenterOfMass = *DataPoint;
// 			TotalDataPoints = 1;
// 		} else {
// 			CenterOfMass = FVector(0);
// 			TotalDataPoints = 0;
// 		}
// 	} else {
// 		
// 		FVector totalMassLocation = FVector(0);
// 		
// 		int totalPoints = 0;
//
// 		for (OctreeNode* child : Children) {
// 			if (child != nullptr) {
// 				child->CalculateCenterOfMass();
// 				totalMassLocation += child->CenterOfMass * child->TotalDataPoints;
// 				totalPoints += child->TotalDataPoints;
// 			}
// 		}
// 		TotalDataPoints = totalPoints;
// 		if (TotalDataPoints > 0) {
// 			CenterOfMass = totalMassLocation / TotalDataPoints;
// 		}
// 	}
// }
//
// void AddDataPoint(OctreeNode* node, FVector newPoint) {
// 	if (!node->IsLeaf()) {
// 		for (auto* child : node->Children) {
// 			if (child->ContainsPoint(newPoint)) {
// 				AddDataPoint(child, newPoint);
// 				return;
// 			}
// 		}
// 	} else if (node->DataPoint != nullptr) {
// 		node->Subdivide();
// 		AddDataPoint(node, newPoint);
// 	} else {
// 		node->DataPoint = new FVector(newPoint);
// 		node->TotalDataPoints = 1; // Set to 1 since it holds 1 data point now
// 	}
// }