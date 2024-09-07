// Fill out your copyright notice in the Description page of Project Settings.


#include "KnowledgeGraph.h"

#include <random>

#include "SimulationSystem.h"
#include "utillllllssss.h"
#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::White,text)



AKnowledgeGraph::AKnowledgeGraph()
{
	PrimaryActorTick.bCanEverTick = true;


	// Useless not used it. 
	SimulationSystem = new FSimulationSystem(GetWorld());
}

AKnowledgeGraph::~AKnowledgeGraph()
{
	UE_LOG(LogTemp, Warning, TEXT("DESTRUCTOR CALLED!@!!!!!!!!!!!!!!!!!!!!!!!!!!"));
}




void AKnowledgeGraph::BeginPlay()
{
	Super::BeginPlay();


	UE_LOG(LogTemp, Warning, TEXT("Begin play called, Restricting tick interval"));
	PrimaryActorTick.TickInterval = tick_interval;


	InitOctree(FBox(FVector(-200, -200, -200), FVector(200, 200, 200)));


	DoWork2();
}


void AKnowledgeGraph::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// return;
	iterations += 1;


	if (iterations > maxiterations)
	{
		return;
	}


	if (1)
	{
		ll("TICK--------------------------------------");
		ll("alpha: " + FString::SanitizeFloat(alpha));
		ll("iterations: " + FString::FromInt(iterations));

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
