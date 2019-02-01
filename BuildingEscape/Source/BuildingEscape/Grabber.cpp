// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	
	InitHandlesAndEventHandlers();
}

void UGrabber::InitHandlesAndEventHandlers()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' must have a PhysicsHandle component"), *GetOwner()->GetName());
	}

	if (InputComponent)
	{
		InputComponent->BindAction(
			TEXT("Grab"),
			EInputEvent::IE_Pressed,
			this,
			&UGrabber::TriggerGrabPressed
		);

		InputComponent->BindAction(
			TEXT("Grab"),
			EInputEvent::IE_Released,
			this,
			&UGrabber::TriggerGrabReleased
		);
	}
}

void UGrabber::RaycastLine(FVector &StartPoint, FVector &EndPoint) const
{
	FVector Location;
	FRotator Rotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT Location, OUT Rotation);

	FVector TraceEnd = Location + (Reach * Rotation.Vector());

	StartPoint = Location;
	EndPoint = TraceEnd;
}

void UGrabber::TriggerGrabPressed()
{
	if (PhysicsHandle)
	{
		FVector Location;
		FVector TraceEnd;
		RaycastLine(OUT Location, OUT TraceEnd);

		FHitResult Collision;
		bool Hit = GetWorld()->LineTraceSingleByObjectType(
			OUT Collision,
			Location,
			TraceEnd,
			FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
			FCollisionQueryParams(TEXT(""), false, GetOwner())
		);

		if (Hit)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *Collision.GetActor()->GetName());
			PhysicsHandle->GrabComponentAtLocation(Collision.GetComponent(), TEXT(""), Collision.GetActor()->GetActorLocation());
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Key pressed"));
}

void UGrabber::TriggerGrabReleased()
{
	if (PhysicsHandle)
	{
		PhysicsHandle->ReleaseComponent();
	}

	UE_LOG(LogTemp, Warning, TEXT("Key released"));
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PhysicsHandle&&PhysicsHandle->GetGrabbedComponent())
	{
		FVector LineStart;
		FVector LineEnd;
		RaycastLine(OUT LineStart, OUT LineEnd);
		PhysicsHandle->SetTargetLocation(LineEnd);
	}
}
