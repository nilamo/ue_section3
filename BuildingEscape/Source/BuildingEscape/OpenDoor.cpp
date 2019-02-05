// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoor.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"

#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (PressurePlate && GetMassOnPlate() > MinMassForTriggering)
	{
		OnOpenRequest.Broadcast();
	}else {
		OnCloseRequest.Broadcast();
	}
}

float UOpenDoor::GetMassOnPlate()
{
	if (!PressurePlate) { return 0.0f; }

	TArray<AActor*> OverlappingActors;
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);

	float TotalMass = 0.0f;
	for (const auto* Actor : OverlappingActors)
	{
		auto Component = Actor->FindComponentByClass<UPrimitiveComponent>();
		if (Component)
		{
			TotalMass += Component->GetMass();
		}
	}

	return TotalMass;
}
