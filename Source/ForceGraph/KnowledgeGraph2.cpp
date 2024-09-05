#include "KnowledgeGraph.h"

#include <random>

#include "SimulationSystem.h"

#include "utillllllssss.h"

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
		int jtarget = FMath::RandRange(0, i - 1);
		AddEdge(jid, jsource, jtarget);
	}
}


void AKnowledgeGraph::DoWork2()
{
	switch (wayofinitnodeslinks)
	{
	//json crap


	case 0:
		{
			const FString JsonFilePath = FPaths::ProjectContentDir() + "/data/graph.json";
			FString JsonString; //Json converted to FString

			FFileHelper::LoadFileToString(JsonString, *JsonFilePath);

			TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
			TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

			if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
			{
				//Retrieving an array property and printing each field
				TArray<TSharedPtr<FJsonValue>> jnodes = JsonObject->GetArrayField("nodes");
				for (int32 i = 0; i < jnodes.Num(); i++)
				{
					auto jobj = jnodes[i]->AsObject();
					int jid = jobj->GetIntegerField("id");
					AKnowledgeNode* kn = GetWorld()->SpawnActor<AKnowledgeNode>();


					AddNode(jid, kn, FVector(0, 0, 0));
				}

				TArray<TSharedPtr<FJsonValue>> jedges = JsonObject->GetArrayField("edges");
				for (int32 i = 0; i < jedges.Num(); i++)
				{
					auto jobj = jedges[i]->AsObject();
					int jid = jobj->GetIntegerField("id");
					int jsource = jobj->GetIntegerField("source");
					int jtarget = jobj->GetIntegerField("target");

					AddEdge(jid, jsource, jtarget);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("JSON PARSING FAILED"));
			}
		}
		break;

	case 1:
		{
			//Retrieving an array property and printing each field
			int jnodes = jnodes1;
			for (int32 i = 0; i < jnodes; i++)
			{
				int jid = i;
				AKnowledgeNode* kn = GetWorld()->SpawnActor<AKnowledgeNode>();
				AddNode(jid, kn, FVector(0, 0, 0));
			}

			// Edge creation loop
			int jedges = jnodes; // Adjust the number of edges as needed to ensure coverage
			std::random_device rd;
			std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
			std::uniform_int_distribution<> dis(0, jnodes - 1); // Random number distribution

			for (int32 i = 0; i < jedges; i++)
			{
				int jid = i;
				int jsource = i % jnodes; // Ensures jsource is always valid within the index range
				int jtarget = (i + 1) % jnodes; // Connect each node to the next node; wraps around

				AddEdge(jid, jsource, jtarget);

				// Random additional edge to increase graph density
				if (i < jnodes - 1)
				{
					int random_target = dis(gen);
					// Ensure that we do not add self-loops
					if (random_target != jsource)
					{
						AddEdge(jid + jnodes, jsource, random_target); // Use jid+jnodes to keep distinct edge IDs
					}
				}
			};
		}
		break;

	case 2:
		{
			DoWork1();
		}
		break;
	case 3:

		GenerateConnectedGraph(3, 10);
		break;
	}


	if (!init)
	{
		InitNodes();
		InitForces();
	}
}
void AKnowledgeGraph::GenerateConnectedGraph(int32 NumClusters, int32 NodesPerCluster)
{
	if (!GetWorld()) return;

	TArray<int32> ClusterCenterIDs;
	ClusterCenterIDs.Reserve(NumClusters);

	// Create Cluster Centers
	for (int32 i = 0; i < NumClusters; ++i)
	{
		FVector Location = FVector(
			0,
			0,
			0
		);
		AKnowledgeNode* Node = GetWorld()->SpawnActor<AKnowledgeNode>(AKnowledgeNode::StaticClass(), Location,
																	  FRotator::ZeroRotator);
		int32 nodeId = i; // Assign node ID, assumed incremented or derived
		AddNode(nodeId, Node, Location);
		ClusterCenterIDs.Add(nodeId);
	}

	// Connect Cluster Nodes
	for (int32 i = 0; i < NumClusters; ++i)
	{
		for (int32 j = 1; j < NodesPerCluster; ++j)
		{
			FVector Location = FVector(i * 100.0f, j * 50.0f, 0); // Organize per cluster
			AKnowledgeNode* Node = GetWorld()->SpawnActor<AKnowledgeNode>(
				AKnowledgeNode::StaticClass(), Location, FRotator::ZeroRotator);
			int32 nodeId = i * NodesPerCluster + j; // Calculate unique node ID


			AddNode(nodeId, Node, Location);

			if (1)
			{
				AddEdge(nodeId,
						ClusterCenterIDs[i],
						nodeId
				); // Use node IDs for connection
			}
			else
			{
				AddEdge(nodeId,
						nodeId,

						ClusterCenterIDs[i]
				); // Use node IDs for connection
			}
		}
	}

	// Inter-cluster Connections
	for (int32 i = 0; i < NumClusters - 1; ++i)
	{
		AddEdge(i, ClusterCenterIDs[i], ClusterCenterIDs[i + 1]); // Use node IDs to connect cluster centers
	}
}
