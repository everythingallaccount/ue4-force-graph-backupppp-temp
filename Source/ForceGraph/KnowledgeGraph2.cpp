#include "KnowledgeGraph.h"

#include <random>

#include "SimulationSystem.h"
#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::White,text)



void AKnowledgeGraph::DoWork1()
{
	//Retrieving an array property and printing each field
	int jnodes11 = jnodes1;
	for (int32 i = 0; i < jnodes11; i++)
	{
		int jid = i;
		AKnowledgeNode* kn = GetWorld()->SpawnActor<AKnowledgeNode>();
		AddNode(jid, kn, FVector(0, 0, 0));
	}

	// Edge creation loop
	int jedges11 = jnodes11; // Adjust the number of edges as needed to ensure coverage

	for (int32 i = 0; i < jedges11; i++)
	{
		int jid = i;
		int jsource = i; // Ensures jsource is always valid within the index range

		// Connected to random node 
		int jtarget = FMath::RandRange(0, jnodes11 - 1);
		AddEdge(jid, jsource, jtarget);
	}
}