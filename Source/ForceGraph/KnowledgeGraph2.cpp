#include "KnowledgeGraph.h"

#include <random>


#include "utillllllssss.h"
// #include "Misc/FileHelper.h"
// #include "Serialization/JsonSerializer.h"

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

	if (0)
	{
		for (int32 i = 1; i < jedges11; i++)
		{
			int jid = i - 1;
			int jsource = i; // Ensures jsource is always valid within the index range

			// Connected to random node 
			int jtarget = FMath::RandRange(0, i - 1);
			AddEdge(jid, jsource, jtarget);
		}
	}
	else
	{
		for (int32 i = 1; i < jedges11; i++)
		{
			int jid = i - 1;
			int jsource = i - 1; // Ensures jsource is always valid within the index range

			// Connected to random node 
			int jtarget = i;
			AddEdge(jid, jsource, jtarget);
		}
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


		CalculateBiasstrengthOflinks();
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


void AKnowledgeGraph::ApplyForces()
{
	// In here velocity of all notes are zeroed
	// In the following for loop, In the first few loop, the velocity is 0. 


	int32 Index = 0;
	// link forces
	// After loop, the velocity of all notes have been altered a little bit because of the link force already. 
	for (auto& link : all_links)
	{
		ll("ApplyForcesssssssssssssssssssss Index: " + FString::FromInt(Index));
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


		// By looking at the javascript code, we can see strength Will only be computed when there is a change Of the graph structure to the graph.
		l = (l - link.Value->distance) /
			l * alpha * link.Value->strength;
		new_v *= l;


		ll("NEW V: " + new_v.ToString() + "link.Value->bias: " + FString::SanitizeFloat(link.Value->bias));
		// Record targeted original velocity.
		FVector target_original_velocity = target_node->velocity;


		if (0)
		{
			target_node->velocity -= new_v * (1 - link.Value->bias);
		}
		else
		{
			target_node->velocity -= new_v * (link.Value->bias);
		}

		// Log out the original velocity and the update velocity. 
		ll("TARGET VELOCITY: " + target_original_velocity.ToString() + " -> " + target_node->velocity.ToString());


		// Record source original velocity.
		FVector source_original_velocity = source_node->velocity;


		source_node->velocity += new_v * (1 - link.Value->bias);


		ll("SOURCE VELOCITY: " + source_original_velocity.ToString() + " -> " + source_node->velocity.ToString());

		if (0)
		{
			if (target_node->id == 7 && alpha > 0.2)
				ll("LINK VEL: " + (-1 * new_v * (1 - link.Value->bias)).ToString());


			if (source_node->id == 7 && alpha > 0.2)
				ll("LINK VEL: " + (new_v * (1 - link.Value->bias)).ToString());
		}
		else
		{
		}

		Index++;
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


		FOREACH_OCTREE_CHILD_NODE3(ChildRef)
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
	ll("Accumulate--------------------------------------");
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
	const FOctreeNodeContext3 CurrentContext,
	FString node_id
)
{
	NodeStrength ns = octree_node_strengths[node_id];

	//if no strength, ignore
	//    if(ns.strength == 0)
	//        return;

	const FBoxCenterAndExtent3& CurrentBounds = CurrentContext.Bounds;
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


			if (kn->id == 7)
			{
				ll("aaaaaaaaaa");
				ll(
					(dir * ns.strength * alpha / l).ToString(), 2
				);
				ll("bbbbbbbbbb");
			}
			//print(FString::SanitizeFloat(ns.strength));

			float mult = pow(ns.strength / nodeStrength, 1.0);
			kn->velocity += dir * ns.strength * alpha / (l / 2.0) * mult;
		}
		return;
	}

	// if not leaf, get all children

	// People do we have to check on L bigger than distance max?
	// FOREACH_OCTREE_CHILD_NODE3 Will not run if the note is LeaF note. , even if L is bigger than distance max. 
	if (!node.IsLeaf() || l >= distancemax)
	{
		//recurse down this dude
		//        print("IM NO LEAF");
		//print("NOT A LEAF");
		int count = 0;
		FOREACH_OCTREE_CHILD_NODE3(ChildRef)
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
						// print(FString::FromInt(Sample.MyActor->id));
						// print((dir * Sample.MyActor->strength * alpha / l * mult).ToString());
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
	if (kn->id == 7)
		ll(
			"ApplyManyBody--------------------------------------We are comparing all the other notes with this ID7 note. ");
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


void FOctreeSematics::SetElementId(FOctreeSematics::FOctree& thisOctree, const FOctreeElement& Element,
                                   FOctreeElementId3 Id)
{
	((FSimpleOctree&)thisOctree).all_elements.Emplace(Element.MyActor->id, Id);
}


void AKnowledgeGraph::InitOctree(const FBox& inNewBounds)
{
	//OctreeData = new FSimpleOctree(FVector(0.0f, 0.0f, 0.0f), 100.0f);
	OctreeData = new FSimpleOctree(inNewBounds.GetCenter(), inNewBounds.GetExtent().GetMax());
}

FSimpleOctree::FSimpleOctree(const FVector& InOrigin, const float InExtent) :
	TOctree3(InOrigin, InExtent)
{
}

void AKnowledgeGraph::RemoveElement(int key)
{
	OctreeData->RemoveElement(OctreeData->all_elements[key]);
	all_nodes.Remove(key);
}

void AKnowledgeGraph::AddOctreeElement(const FOctreeElement& inNewOctreeElement)
{
	ll("AddOctreeElement--------------------------------------");
	OctreeData->AddElement(inNewOctreeElement);
}

void AKnowledgeGraph::InitNodes()
{
	if (0)
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
	else
	{
		// To replicate the node indexing from the original JS function
		for (auto& node : all_nodes)
		{
			int index = node.Key;
			// Calculate index-based radius differently based on the number of dimensions
			float radius;
			int nDim = 3;
			if (nDim > 2)
			{
				radius = initialRadius * cbrt(0.5f + index);
			}
			else if (nDim > 1)
			{
				radius = initialRadius * sqrt(0.5f + index);
			}
			else
			{
				radius = initialRadius * index;
			}

			float initialAngleRoll = PI * (3 - sqrt(5)); // Roll angle

			// Following will be Math.PI * 20 / (9 + Math.sqrt(221));
			float initialAngleYaw = PI * 20 / (9 + sqrt(221)); // Yaw angle if needed (3D)


			float rollAngle = index * initialAngleRoll; // Roll angle
			float yawAngle = index * initialAngleYaw; // Yaw angle if needed (3D)

			FVector init_pos;

			if (nDim == 1)
			{
				// 1D: Positions along X axis
				init_pos = FVector(radius, 0, 0);
			}
			else if (nDim == 2)
			{
				// 2D: Circular distribution
				init_pos = FVector(radius * cos(rollAngle), radius * sin(rollAngle), 0);
			}
			else
			{
				// 3D: Spherical distribution
				init_pos = FVector(radius * sin(rollAngle) * cos(yawAngle), radius * cos(rollAngle),
				                   radius * sin(rollAngle) * sin(yawAngle));
			}

			// Set the initial position of the node Actor
			if (node.Value)
			{
				// Check if pointer is valid
				node.Value->SetActorLocation(init_pos, false);

				ll("index: " + FString::FromInt(index) + " init_pos: " + init_pos.ToString());

				// Log the initial position - Uncomment to use
				// UE_LOG(LogTemp, Warning, TEXT("Init position: %s"), *init_pos.ToString());

				// Set initial velocity to zero
				node.Value->velocity = FVector(0, 0, 0);
			}

			// Increment index for next node
			// index++;
		}
	}
}


void AKnowledgeGraph::CalculateBiasstrengthOflinks()
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
		ll("all_nodes[link.Value->source]->numberOfConnected: " + FString::FromInt(
			all_nodes[link.Value->source]->numberOfConnected));


		float ttttttttttt = all_nodes[link.Value->source]->numberOfConnected +
			all_nodes[link.Value->target]->numberOfConnected;

		float bias = all_nodes[link.Value->source]->numberOfConnected /
			ttttttttttt;


		ll("!!!!!!!!!!!!!!!!!!!!!!!bias: " + FString::SanitizeFloat(bias));


		if (biasinitway == 0)
		{
			link.Value->bias = bias > 0.5 ? (1 - bias) * 0.5 + bias : bias * 0.5;
		}
		else
		{
			link.Value->bias = bias;
		}
		ll("!!!!!!!!!!!!!!!!!!!!!!!bias: " + FString::SanitizeFloat(link.Value->bias));

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


void AKnowledgeGraph::AddNode(int32 id, AKnowledgeNode* kn, FVector location)
{
	if (!all_nodes.Contains(id))
	{
		kn->id = id;

		kn->strength = nodeStrength;


		all_nodes.Emplace(id, kn);

		FOctreeElement ote;
		ote.MyActor = kn;
		ote.strength = 1.0; // update with strength
		ote.BoxSphereBounds = FBoxSphereBounds(
			location,
			FVector(1.0f, 1.0f, 1.0f),
			1.0f
		);

		AddOctreeElement(ote);
	}
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
}
