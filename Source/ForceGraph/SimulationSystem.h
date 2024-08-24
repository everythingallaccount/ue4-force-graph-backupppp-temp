#pragma once

#include "CoreMinimal.h"
#include "Node.h"
#include "TimerManager.h"
#include <map>

#include "KnowledgeEdge.h"
#include "KnowledgeNode.h"

// class UWorld;

class  FORCEGRAPH_API FSimulationSystem
{
public:
    FSimulationSystem(UWorld* InWorld);
    ~FSimulationSystem();

    void Tick(int iterations);
    void Tick111(int iterations);

    void InitializeNodes(int NumNodes, int NumDimensions);
    void ApplyForces();
    FNode* FindClosestNode(const FVector& Point, float MaxRadius);

public:
    TArray<FNode> Nodes;      // this one contain the positions.
    TArray<int32> NodesID;      // this one contain the id of the nodes.

    TMap<int32, AKnowledgeNode*> all_nodes;
    TMap<int32, AKnowledgeEdge*> all_links; // this one contain the links.


    
    UWorld* World;

    int NumDimensions;
    float Alpha;
    float AlphaMin;
    float AlphaDecay;
    float AlphaTarget;
    float VelocityDecay;

    std::map<FString, TFunction<void(float)>> Forces; // Map of forces

    void InitializeNodePositions();
    void UpdateSimulationParameters();

    FTimerHandle TimerHandle;
};