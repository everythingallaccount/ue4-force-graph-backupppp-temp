// Fill out your copyright notice in the Description page of Project Settings.


#include "KnowledgeGraph.h"

#include <random>

#include "SimulationSystem.h"
#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::White,text)

FSimpleOctree::FSimpleOctree(const FVector& InOrigin, float InExtent) : TOctree(InOrigin, InExtent)
{
}

void FOctreeSematics::SetElementId(FOctreeSematics::FOctree& thisOctree, const FOctreeElement& Element,
                                   FOctreeElementId Id)
{
	((FSimpleOctree&)thisOctree).all_elements.Emplace(Element.MyActor->id, Id);
}

AKnowledgeGraph::AKnowledgeGraph()
{
	PrimaryActorTick.bCanEverTick = true;


	SimulationSystem = new FSimulationSystem(GetWorld());
}

AKnowledgeGraph::~AKnowledgeGraph()
{
	UE_LOG(LogTemp, Warning, TEXT("DESTRUCTOR CALLED!@!!!!!!!!!!!!!!!!!!!!!!!!!!"));
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


void AKnowledgeGraph::BeginPlay()
{
	Super::BeginPlay();


	UE_LOG(LogTemp, Warning, TEXT("Begin play called, Restricting tick interval"));
	PrimaryActorTick.TickInterval = tick_interval;


	InitOctree(FBox(FVector(-200, -200, -200), FVector(200, 200, 200)));

	if (1)
	{
		//json crap
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
	else
	{
		if (0)
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
			}
		}
		else
		{
			GenerateConnectedGraph(3, 10);
		}
	}


	if (!init)
	{
		InitNodes();
		InitForces();
	}
}


void AKnowledgeGraph::AddNode(int32 id, AKnowledgeNode* kn, FVector location)
{
	if (!all_nodes.Contains(id))
	{
		kn->id = id;

		kn->strength = nodeStrength;


		all_nodes.Emplace(id, kn);
		SimulationSystem->all_nodes.Emplace(id, kn);

		FOctreeElement ote;
		ote.MyActor = kn;
		ote.strength = 1.0; // update with strength
		ote.BoxSphereBounds = FBoxSphereBounds(location, FVector(1.0f, 1.0f, 1.0f), 1.0f);
		AddOctreeElement(ote);
	}
}

void AKnowledgeGraph::AddOctreeElement(const FOctreeElement& inNewOctreeElement)
{
	OctreeData->AddElement(inNewOctreeElement);
}

void AKnowledgeGraph::AddEdge(int32 id, int32 source, int32 target)
{
	UObject* SpawnClass = Cast<UObject>(
		StaticLoadObject(UObject::StaticClass(),
		                 NULL,
		                 TEXT("Blueprint'/Game/cylinder.cylinder'")
		)
	);
	UBlueprint* GeneratedObj = Cast<UBlueprint>(SpawnClass);


	AKnowledgeEdge* e = GetWorld()->SpawnActor<AKnowledgeEdge>(
		GeneratedObj->GeneratedClass
	);

	e->source = source;
	e->target = target;
	e->strength = 1; //temp
	e->distance = edgeDistance;
	all_links.Emplace(id, e);


	SimulationSystem->all_links.Emplace(id, e);
	UE_LOG(LogTemp, Warning, TEXT("SIMULATION SYSTEM LINKS: %d"), SimulationSystem->all_links.Num());
}

void AKnowledgeGraph::InitNodes()
{
	for (auto& node : all_nodes)
	{
		float radius = initialRadius * sqrt(node.Key);
		float angle = node.Key * initialAngle;
		FVector init_pos = FVector(cos(angle), sin(angle), tan(angle)) * radius;

		// Remember that the note value stored the actual object. 
		node.Value->SetActorLocation(init_pos, false);
		//        print("init position");
		//        print(node.Value->GetActorLocation().ToString());
		node.Value->velocity = FVector(0, 0, 0);
	}
}

void AKnowledgeGraph::InitOctree(const FBox& inNewBounds)
{
	//OctreeData = new FSimpleOctree(FVector(0.0f, 0.0f, 0.0f), 100.0f);
	OctreeData = new FSimpleOctree(inNewBounds.GetCenter(), inNewBounds.GetExtent().GetMax());
}

void AKnowledgeGraph::InitForces()
{
	//link forces
	float n = all_nodes.Num();
	float m = all_links.Num();

	for (auto& link : all_links)
	{
		all_nodes[link.Value->source]->numberOfConnected += 1;
		all_nodes[link.Value->target]->numberOfConnected += 1;
	}

	for (auto& link : all_links)
	{
		float bias = all_nodes[link.Value->source]->numberOfConnected /
		(
			all_nodes[link.Value->source]->numberOfConnected +
			all_nodes[link.Value->target]->numberOfConnected
		);

		if (0)
		{
			link.Value->bias = bias > 0.5 ? (1 - bias) * 0.5 + bias : bias * 0.5;
		}
		else
		{
			link.Value->bias = bias;
		}


		link.Value->strength = 1.0 / fmin(all_nodes[link.Value->source]->numberOfConnected,
		                                  all_nodes[link.Value->target]->numberOfConnected);
	}

	//charge forces
	for (auto& node : all_nodes)
	{
		node.Value->strength = node.Value->strength; // nothing for now
	}

	//center forces
	//nothing
	init = true;
}

void AKnowledgeGraph::RemoveElement(int key)
{
	OctreeData->RemoveElement(OctreeData->all_elements[key]);
	all_nodes.Remove(key);
}


FVector Jiggle(const FVector& Vec, float Magnitude)
{
	FVector RandomJitter;
	RandomJitter.X = FMath::RandRange(-0.5f, 0.5f) * Magnitude;
	RandomJitter.Y = FMath::RandRange(-0.5f, 0.5f) * Magnitude;
	RandomJitter.Z = FMath::RandRange(-0.5f, 0.5f) * Magnitude;

	return Vec + RandomJitter;
}

void AKnowledgeGraph::ApplyForces()
{
	// In here velocity of all notes are zeroed
	// In the following for loop, In the first few loop, the velocity is 0. 

	// link forces
	// After loop, the velocity of all notes have been altered a little bit because of the link force already. 
	for (auto& link : all_links)
	{
		auto source_node = all_nodes[link.Value->source];
		auto target_node = all_nodes[link.Value->target];

		FVector source_pos = source_node->GetActorLocation();
		FVector source_velocity = source_node->velocity;

		FVector target_pos = target_node->GetActorLocation();
		FVector target_velocity = target_node->velocity;

		// UE_LOG(LogTemp, Warning, TEXT("source VELOCITY1: %s"), *source_velocity.ToString());
		// UE_LOG(LogTemp, Warning, TEXT("target VELOCITY1: %s"), *target_velocity.ToString());


		FVector new_v = target_pos + target_velocity - source_pos - source_velocity;


		if (new_v.IsNearlyZero())
		{
			new_v = Jiggle(new_v, 1e-6f);
		}

		float l = new_v.Size();
		// UE_LOG(LogTemp, Warning, TEXT("!!!link.Value->distance: %f"), link.Value->distance);


		// By looking at the javascript code, we can see strength Will only be computed when there is a change to the graph.
		l = (l - link.Value->distance) / l * alpha * link.Value->strength;
		new_v *= l;


		if (1)
		{
			target_node->velocity -= new_v * (1 - link.Value->bias);
		}
		else
		{
			target_node->velocity -= new_v * (link.Value->bias);
		}

		source_node->velocity += new_v * (link.Value->bias);

		// if (target_node->id == 7 && alpha > 0.2)
		// 	print("LINK VEL: " + (-1 * new_v * (1 - link.Value->bias)).ToString());
		// if (source_node->id == 7 && alpha > 0.2)
		// 	print("LINK VEL: " + (new_v * (1 - link.Value->bias)).ToString());
	}

	if (1)
	{
		//charge forces
		octree_node_strengths.Empty();
		for (auto& node : all_nodes)
		{
			int key = node.Key;
			auto kn = node.Value;


			// Because the actor location hasn't changed when we compute the link force, so These two lines could actually be put in the start of the function. 

			// If they are remove here，then why we do AddOctreeElement(ote) in AddNode?
			RemoveElement(node.Key); //need to remove then update with new location when adding

			AddNode(key, kn, kn->GetActorLocation());
		}


		// Most of the questions come from here。 
		Accumulate();


		for (auto& node : all_nodes)
		{
			ApplyManyBody(node.Value);
		}
	}
}

NodeStrength AKnowledgeGraph::AddUpChildren(
	const FSimpleOctree::FNode& node,
	FString node_id
)
{
	if (!octree_node_strengths.Contains(node_id))
	{
		octree_node_strengths.Emplace(node_id, NodeStrength());
	}
	else
	{
		//reset
		octree_node_strengths[node_id].strength = 0;
		octree_node_strengths[node_id].direction = FVector(0, 0, 0);
	}

	if (!node.IsLeaf())
	{
		int count = 0;
		float c = 0.0;
		float strength = 0.0;
		float weight = 0.0;
		FVector tempvec;


		FOREACH_OCTREE_CHILD_NODE(ChildRef)
		{
			//go find the leaves
			if (

				// ChildRef is a special structure, but basically store a number from one to 7. 
				node.HasChild(ChildRef)
			)
			{
				NodeStrength ns = AddUpChildren(
					*node.GetChild(ChildRef),
					node_id + FString::FromInt(count)
				);
				//add up children
				//math for vector and strength
				c = abs(ns.strength);

				// Typically should be a positive integer, so strength will be same as weight. 
				strength += ns.strength;

				weight += c;


				tempvec += c * ns.direction;

				count++;
			}
		}
		octree_node_strengths[node_id].strength = strength; //hash of ID of node for map
		octree_node_strengths[node_id].direction = tempvec / weight;
	}
	else
	{
		for (FSimpleOctree::ElementConstIt ElementIt(node.GetElementIt()); ElementIt; ++ElementIt)
		{
			//all the way down to elements
			const FOctreeElement& Sample = *ElementIt;


			// So basically we add up all the strength inside a leaf node
			octree_node_strengths[node_id].strength += Sample.MyActor->strength;
			// When we summing up all the actual locations as a vector, it seems that it take an average location. 
			octree_node_strengths[node_id].direction += Sample.MyActor->GetActorLocation();
		}
	}

	// octree_node_strengths[node_id].direction.ToString(); //?
	return octree_node_strengths[node_id];
}

//get weights for every node before applying
void AKnowledgeGraph::Accumulate()
{
	for (
		FSimpleOctree::TConstIterator<> NodeIt(*OctreeData);
		NodeIt.HasPendingNodes();
		NodeIt.Advance()
	)
	{
		const FSimpleOctree::FNode& CurrentNode = NodeIt.GetCurrentNode();
		// UE_LOG(LogTemp, Warning, TEXT("Ready to add up children"));
		AddUpChildren(CurrentNode, "0");
		break;
	}
}

//use nodes to find velocity
void AKnowledgeGraph::FindManyBodyForce(
	AKnowledgeNode* kn,
	const FSimpleOctree::FNode& node,
	const FOctreeNodeContext CurrentContext,
	FString node_id)
{
	NodeStrength ns = octree_node_strengths[node_id];

	//if no strength, ignore
	//    if(ns.strength == 0)
	//        return;

	const FBoxCenterAndExtent& CurrentBounds = CurrentContext.Bounds;
	FVector center = CurrentBounds.Center;
	FVector width = CurrentBounds.Extent;
	FVector dir = ns.direction - kn->GetActorLocation();

	// Remember that direction is the sum of all the Actor locations of the elements in that note. 
	float l = dir.Size() * dir.Size();

	// if size of current box is less than distance between nodes
	// This is used to stop recurring down the tree.
	if (width.X * width.X / theta2 < l)
	{
		//        print("GOING IN HERE");
		if (l < distancemax)
		{
			if (l < distancemin)
				l = sqrt(distancemin * l);
			if (kn->id == 7 && alpha > 0.2)
				print((dir * ns.strength * alpha / l).ToString());
			//print(FString::SanitizeFloat(ns.strength));
			float mult = pow(ns.strength / nodeStrength, 1.0);
			kn->velocity += dir * ns.strength * alpha / (l / 2.0) * mult;
		}
		return;
	}

	// if not leaf, get all children

	// People do we have to check on L bigger than distance max?
	// FOREACH_OCTREE_CHILD_NODE Will not run if the note is LeaF note. , even if L is bigger than distance max. 
	if (!node.IsLeaf() || l >= distancemax)
	{
		//recurse down this dude
		//        print("IM NO LEAF");
		//print("NOT A LEAF");
		int count = 0;
		FOREACH_OCTREE_CHILD_NODE(ChildRef)
		{
			if (node.HasChild(ChildRef))
			{
				FindManyBodyForce(
					kn,
					*node.GetChild(ChildRef),
					CurrentContext.GetChildContext(ChildRef),
					node_id + FString::FromInt(count)
				);
				count++;
			}
		}
	} //if leaf and close, apply elements directly
	else if (node.IsLeaf())
	{
		//print("IM LEAF");
		if (l < distancemin)
		{
			l = sqrt(distancemin * l);
		}
		for (FSimpleOctree::ElementConstIt ElementIt(node.GetElementIt()); ElementIt; ++ElementIt)
		{
			const FOctreeElement& Sample = *ElementIt;
			if (Sample.MyActor->id != kn->id)
			{
				dir = Sample.MyActor->GetActorLocation() - kn->GetActorLocation();
				l = dir.Size() * dir.Size();
				float mult = pow(Sample.MyActor->numberOfConnected, 3.0);


				if (0)
				{
					if (kn->id == 7 && alpha > 0.2)
					{
						print(FString::FromInt(Sample.MyActor->id));
						print((dir * Sample.MyActor->strength * alpha / l * mult).ToString());
					}
				}

				kn->velocity += dir * Sample.MyActor->strength * alpha / l * mult;
			}
		}
	}
}

void AKnowledgeGraph::ApplyManyBody(AKnowledgeNode* kn)
{
	FVector dir;
	if (alpha > 0.2 && kn->id == 7)
		print("--------------------------------------");
	for (
		FSimpleOctree::TConstIterator<> NodeIt(*OctreeData);
		NodeIt.HasPendingNodes();
		NodeIt.Advance()
	)
	{
		FindManyBodyForce(kn,
		                  NodeIt.GetCurrentNode(),
		                  NodeIt.GetCurrentContext(),
		                  "0"
		);
		break;
	}
}


void AKnowledgeGraph::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	iterations += 1;


	if (iterations > maxiterations)
	{
		return;
	}


	if (1)
	{
		UE_LOG(LogTemp, Warning, TEXT("tick,alpha: %f"), alpha);
		if (alpha < alphaMin)
		{
			UE_LOG(LogTemp, Warning, TEXT("alpha is less than alphaMin"));
			return;
		}

		alpha += (alphaTarget - alpha) * alphaDecay; //need to restart this if want to keep moving

		ApplyForces();

		for (auto& node : all_nodes)
		{
			auto kn = node.Value;
			//            print("FINAL VELOCITY!");
			//            print(kn->velocity.ToString());

			kn->velocity *= velocityDecay;


			// if (kn->id == 7 && alpha > 0.2)
			// 	print("FINAL VELOCITY: " + kn->velocity.ToString());


			FVector NewLocation = kn->GetActorLocation() + kn->velocity;

			kn->SetActorLocation(
				NewLocation
			);


			kn->velocity *= 0; //reset velocities

			//            print("FINAL POSITION!");
			//            print(FString::FromInt(node.Key));
			//            print(kn->GetActorLocation().ToString());
		}

		for (auto& link : all_links)
		{
			auto l = link.Value;
			//            print("LOCCCCCC");
			//            print(all_nodes[l->source]->GetActorLocation().ToString());
			l->ChangeLoc(
				all_nodes[l->source]->GetActorLocation(),
				all_nodes[l->target]->GetActorLocation()
			);
		}
	}
	else
	{
	}
}
