#include "octreeeeeeeeee3.h"

OctreeNode::OctreeNode(FVector center, FVector extent)
    : Center(center), Extent(extent), TotalDataPoints(0), CenterOfMass(center), Data(nullptr) {
    Children.SetNum(8, false);
}

OctreeNode::~OctreeNode() {
    for (auto* child : Children) {
        delete child;
    }
    delete Data;
}

bool OctreeNode::IsLeaf() const {
    return Children[0] == nullptr;
}

bool OctreeNode::ContainsPoint(const FVector point) const {
    return (FMath::Abs(point.X - Center.X) <= Extent.X &&
            FMath::Abs(point.Y - Center.Y) <= Extent.Y &&
            FMath::Abs(point.Z - Center.Z) <= Extent.Z);
}

void OctreeNode::Subdivide() {
    if (!IsLeaf() || Data == nullptr) return;

    FVector NewExtent = Extent * 0.5;
    PointData* OldData = Data;
    Data = nullptr;  // Clear current data as it's redistributed to children

    for (int i = 0; i < 8; ++i) {
        FVector NewCenter = Center + FVector(
            (i & 4) ? NewExtent.X : -NewExtent.X,
            (i & 2) ? NewExtent.Y : -NewExtent.Y,
            (i & 1) ? NewExtent.Z : -NewExtent.Z
        );
        Children[i] = new OctreeNode(NewCenter, NewExtent);
    }

    AddDataPoint(this, OldData->Position, OldData->Velocity);
    delete OldData;  // Deleting the old data after redistributing
}

void AddDataPoint(OctreeNode* node, FVector newPoint, FVector velocity) {
    if (!node->IsLeaf()) {
        for (auto* child : node->Children) {
            if (child->ContainsPoint(newPoint)) {
                AddDataPoint(child, newPoint, velocity);
                return;
            }
        }
    } else {
        if (node->Data != nullptr) {
            node->Subdivide();
            AddDataPoint(node, newPoint, velocity);
        } else {
            node->Data = new PointData(newPoint, velocity);
            node->TotalDataPoints = 1; // Now properly accounting for the node having new data
        }
    }
}

void OctreeNode::CalculateCenterOfMass() {
    if (IsLeaf()) {
        if (Data) {
            CenterOfMass = Data->Position;
            TotalDataPoints = 1;
        }
    } else {
        FVector aggregateMass = FVector(0);
        int count = 0;
        for (OctreeNode* child : Children) {
            if (child != nullptr) {
                child->CalculateCenterOfMass();
                aggregateMass += child->CenterOfMass * child->TotalDataPoints;
                count += child->TotalDataPoints;
            }
        }
        if (count > 0) {
            CenterOfMass = aggregateMass / count;
            TotalDataPoints = count;
        }
    }
}