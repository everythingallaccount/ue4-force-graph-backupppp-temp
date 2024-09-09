#include "octreeeeeeeeee3.h"

#include <queue>

#include "utillllllssss.h"

OctreeNode::OctreeNode(FVector center, FVector extent)
    : Center(center),
Extent(extent),
TotalDataPoints(0),
CenterOfMass(center),
Data(nullptr) {
    Children.SetNum(8, false);
    isCenterSet=true;
}

OctreeNode::OctreeNode()
    :
// Center(center),
// Extent(extent),
TotalDataPoints(0),
// CenterOfMass(center),
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
    if (
        !IsLeaf()   // 
        ||
        Data == nullptr // 

        // These two conditions shall never matter, because dysfunctions before calling shall already have the checking. 
        ) return;

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

    AddDataPoint(this, OldData->Node);  // Add the old data point to the children
    delete OldData;  // Deleting the old data after redistributing
}

void AddDataPoint(OctreeNode* node, AKnowledgeNode* kn) {

    FVector newPoint = kn->GetActorLocation();


    if (!node->IsLeaf()) {
        // Recursively found the leaf node to add the data point
        for (auto* child : node->Children) {
            if (child->ContainsPoint(newPoint)) {
                AddDataPoint(child, kn);
                return;
            }
        }
    } else {
        if (
            node->Data != nullptr
            ) {
            // If the node already has data, subdivide and add the new data point
            node->Subdivide();
            AddDataPoint(node, kn);
        } else {

            // No data is associated with the current node
            node->Data = new PointData(kn);
            node->TotalDataPoints = 1; // Now properly accounting for the node having new data
        }
    }
}

void OctreeNode::CalculateCenterOfMass() {
    if (IsLeaf()) {
        if (Data) {
            CenterOfMass = Data->Node->GetActorLocation();
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

void OctreeNode::Cover(float x, float y, float z)
{
    // intended to call on the lower bound and the highest bound of all the data combined. 

    
    FVector point(x, y, z);
    if (!isCenterSet) {
        // Set the initial bounds as the data point acting as the lowest bound.
        FVector minBound = FVector(FMath::FloorToFloat(x), FMath::FloorToFloat(y), FMath::FloorToFloat(z));
        
        // Assuming the initial extent should be (1,1,1)
        FVector initialExtent = FVector(1, 1, 1);
        
        // The center will be the minBound plus half of the initial extent
        Center = minBound + initialExtent * 0.5f;
        Extent = initialExtent * 0.5f;
        
        isCenterSet = true;
        return;
    }

    while (!ContainsPoint(point)) {


        FVector temp= Center-Extent;

        
        Extent *= 2; // Double the size of the octree
        // Determine the direction to adjust the center based on where the point lies

        Center=temp+Extent;

        // FVector direction = point - Center;
        // Center += FVector(
        //     (direction.X < 0) ? -Extent.X / 2 : Extent.X / 2,
        //     (direction.Y < 0) ? -Extent.Y / 2 : Extent.Y / 2,
        //     (direction.Z < 0) ? -Extent.Z / 2 : Extent.Z / 2
        // );
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
    if (node->Data->Node->GetActorLocation().X > threshold) {
        // Print/log some information (in Unreal it could be UE_LOG or GLog)
        UE_LOG(LogTemp, Warning, TEXT("Node with Data exceeding threshold found at X: %f"), node->Data->Node->GetActorLocation().X);
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


    Cover(X0, Y0, Z0);
    Cover(X1, Y1, Z1);

    ll("!!!!New center and you extend will be set to: " + Center.ToString() + " " + Extent.ToString());
    // Add the new points
    for (int i = 0; i < N; ++i) {
        AddDataPoint(this, Map[i]);
                            
            
    }
}
