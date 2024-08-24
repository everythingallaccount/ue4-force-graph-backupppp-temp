#include "SimulationSystem.h"




#include "Math/UnrealMathUtility.h"  // For math functions

#define MAX_DIMENSIONS 3

FSimulationSystem::FSimulationSystem(UWorld* InWorld)
: World(InWorld),
NumDimensions(3),
Alpha(1.0),
AlphaMin(0.001),
AlphaDecay(0.05),
AlphaTarget(0.0),
VelocityDecay(0.6)
{
}



FSimulationSystem::~FSimulationSystem()
{
}



void FSimulationSystem::Tick(int iterations=1)
{
    Tick111(iterations);
}

void FSimulationSystem::Tick111(int iterations=1)
{
    for (int i = 0; i < iterations; ++i)
    {
        
        ApplyForces();
        for (FNode& Node : Nodes)
        {
            if (!Node.bIsPositionFixed)
            {
                Node.Position += Node.Velocity;
            }
        }
    }
}






//
// void FSimulationSystem::AddNode()
// {
//     // this->NumDimensions = FMath::Clamp(NumDimensionsS, 1, MAX_DIMENSIONS);
//     this->NumDimensions = 3;
//     Nodes.Empty();
//
//     for(int i = 0; i < NumNodes; ++i)
//     {
//         FNode Node;
//         // Default init
//         Node.Position = FVector::ZeroVector;
//         Node.Velocity = FVector::ZeroVector;
//         Node.bIsPositionFixed = false;
//
//         Nodes.Add(Node);
//     }
//
//     InitializeNodePositions();
// }

void FSimulationSystem::InitializeNodes(int NumNodes, int NumDimensionsS)
{
    // this->NumDimensions = FMath::Clamp(NumDimensionsS, 1, MAX_DIMENSIONS);
    this->NumDimensions = 3;
    Nodes.Empty();

    for(int i = 0; i < NumNodes; ++i)
    {
        FNode Node;
        // Default init
        Node.Position = FVector::ZeroVector;
        Node.Velocity = FVector::ZeroVector;
        Node.bIsPositionFixed = false;

        Nodes.Add(Node);
    }

    InitializeNodePositions();
}

void FSimulationSystem::InitializeNodePositions()
{
    // Your logic for positions based on dimensions and irrational number distribution
    for (int i = 0; i < Nodes.Num(); ++i)
    {
        FNode& Node = Nodes[i];

        // Example: setting position based on a formula (customize this)
        float Angle = i * PI * (3 - FMath::Sqrt(5)); // Golden ratio
        float Radius = 100 * FMath::Sqrt(i + 0.5);

        if (NumDimensions == 1)
        {
            Node.Position.X = Radius;
        }
        else if (NumDimensions == 2)
        {
            Node.Position.X = Radius * FMath::Cos(Angle);
            Node.Position.Y = Radius * FMath::Sin(Angle);
        }
        else if (NumDimensions == 3)
        {
            // 3D spherical coordinates, optionally based on a second irrational number
            float YawAngle = i * PI * 20 / (9 + FMath::Sqrt(221)); // Markov irrational
            Node.Position.X = Radius * FMath::Sin(Angle) * FMath::Cos(YawAngle);
            Node.Position.Y = Radius * FMath::Cos(Angle);
            Node.Position.Z = Radius * FMath::Sin(Angle) * FMath::Sin(YawAngle);
        }
    }
}

void FSimulationSystem::ApplyForces()
{
    // Assuming you add forces to the 'Forces' map that apply certain physics
    for (auto& ForcePair : Forces)
    {
        // Each force is a function taking alpha and applying some change
        ForcePair.second(Alpha);
    }
}

FNode* FSimulationSystem::FindClosestNode(const FVector& Point, float MaxRadius)
{
    float BestDistSquared = MaxRadius * MaxRadius;
    FNode* BestNode = nullptr;
    
    for (FNode& Node : Nodes)
    {
        float DistSquared = FVector::DistSquared(Point, Node.Position);
        if (DistSquared < BestDistSquared)
        {
            BestDistSquared = DistSquared;
            BestNode = &Node;
        }
    }
    
    return BestNode;
}