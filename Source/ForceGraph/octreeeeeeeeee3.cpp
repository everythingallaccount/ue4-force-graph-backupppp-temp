#include "octreeeeeeeeee3.h"

#include <queue>

OctreeNode::OctreeNode(FVector center, FVector extent)
    : Center(center),
Extent(extent),
TotalDataPoints(0),
CenterOfMass(center),
Data(nullptr) {
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


void TraverseBFS(OctreeNode* root, OctreeCallback callback) {
    if (!root) return; // If the root is null, return immediately

    std::queue<OctreeNode*> nodeQueue;
    nodeQueue.push(root);

    while (!nodeQueue.empty()) {
        OctreeNode* currentNode = nodeQueue.front();
        nodeQueue.pop();

        // Execute the callback on the current node
        bool skipChildren = callback(currentNode);

        // If callback returns true, do not enqueue children
        if (skipChildren) {
            continue;
        }

        // Otherwise, enqueue all non-null children
        for (OctreeNode* child : currentNode->Children) {
            if (child) {
                nodeQueue.push(child);
            }
        }
    }
}

bool SampleCallback(OctreeNode* node) {
    if (!node || !node->Data) return false;  // If no data, continue traversal

    // Set a threshold value for some condition
    const float threshold = 10.0f;

    // Example condition: stop traversal if any point's x-coordinate is greater than threshold
    if (node->Data->Position.X > threshold) {
        // Print/log some information (in Unreal it could be UE_LOG or GLog)
        UE_LOG(LogTemp, Warning, TEXT("Node with Data exceeding threshold found at X: %f"), node->Data->Position.X);
        return true;  // Stop visiting further children of this node
    }

    return false;  // Continue to visit children
}
// Assuming `root` is the root of your Octree and it's properly initialized
// TraverseBFS(root, SampleCallback);




void OctreeNode::AddAll1(TMap<int32, AKnowledgeNode*> Map){

    
    int32 N = Map.Num();


    TArray<float> Xz, Yz, Zz;
    Xz.Reserve(N);
    Yz.Reserve(N);
    Zz.Reserve(N);

    float X0 = std::numeric_limits<float>::infinity();
    float Y0 = std::numeric_limits<float>::infinity();
    float Z0 = std::numeric_limits<float>::infinity();
    float X1 = -std::numeric_limits<float>::infinity();
    float Y1 = -std::numeric_limits<float>::infinity();
    float Z1 = -std::numeric_limits<float>::infinity();

    // Compute the points and their extent
    for (int i = 0; i < N; ++i) {
        FVector D = Map[i]->GetActorLocation();
        float X = D.X;
        float Y = D.Y;
        float Z = D.Z;
        Xz.Add(X);
        Yz.Add(Y);
        Zz.Add(Z);

        X0 = FMath::Min(X0, X);
        Y0 = FMath::Min(Y0, Y);
        Z0 = FMath::Min(Z0, Z);
        X1 = FMath::Max(X1, X);
        Y1 = FMath::Max(Y1, Y);
        Z1 = FMath::Max(Z1, Z);
    }

    // Cover the extent


    // Cover(X0, Y0, Z0);
    // Cover(X1, Y1, Z1);
    //
    // // Add the new points
    // for (int i = 0; i < N; ++i) {
    //     Add(Xz[i], Yz[i], Zz[i], Data[i]);
    // }
}
