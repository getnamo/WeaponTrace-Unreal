#include "WeaponTraceComponent.h"

#include "Kismet/KismetSystemLibrary.h"

UWeaponTraceComponent::UWeaponTraceComponent()
{
	ObjectTypesToTrace = { ObjectTypeQuery1, ObjectTypeQuery2, ObjectTypeQuery4 };
	
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

UWeaponTraceComponent* UWeaponTraceComponent::GetWeaponTraceComponent_Implementation()
{
	return this;
}

AActor* UWeaponTraceComponent::GetOwnerActor_Implementation()
{
	return OwnerActor;
}

void UWeaponTraceComponent::SetOwnerActor_Implementation(AActor* InOwnerActor)
{
	OwnerActor = InOwnerActor;

	for (TSet<UActorComponent*>::TConstIterator It = GetOwner()->GetComponents().CreateConstIterator();
		It;
		++It)
	{
		if (USceneComponent* SceneComponent = Cast<USceneComponent>(*It))
		{
			if (SceneComponent->GetName() == "Base")
			{
				WeaponStartSceneComponent = SceneComponent;
			}
			else if (SceneComponent->GetName() == "Tip")
			{
				WeaponEndSceneComponent = SceneComponent;
			}
		}
	}

	if (OwnerActor)
	{
		ActorsToIgnore.Add(OwnerActor);
		ActorsToIgnore.Add(GetOwner());

		CalculateTraceCount();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red,
			TEXT("Owner actor is null. Cannot set owner actor."));
		SetComponentTickEnabled(false);
	}
}

TArray<FVector> UWeaponTraceComponent::GetTraceLocations_Implementation() const
{
	TArray<FVector> SocketLocations;

	if (AreWeaponSceneComponentsValid())
	{
		SocketLocations.Add(WeaponStartSceneComponent->GetComponentLocation());
		SocketLocations.Add(WeaponEndSceneComponent->GetComponentLocation());

		return SocketLocations;
	}

	if (UActorComponent* StaticMeshComponent = GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass()))
	{
		if (UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(StaticMeshComponent))
		{
			if (MeshComponent->DoesSocketExist("WeaponStart") && MeshComponent->DoesSocketExist("WeaponEnd"))
			{
				SocketLocations.Add(MeshComponent->GetSocketLocation("WeaponStart"));
				SocketLocations.Add(MeshComponent->GetSocketLocation("WeaponEnd"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Socket 'WeaponStart' or 'WeaponEnd' not found on static mesh component"));
			}
		}
	}

	return SocketLocations;
}

bool UWeaponTraceComponent::SetSwingActive_Implementation(bool bInSwingActive)
{
	bIsSwingActive = bInSwingActive;

	SetComponentTickEnabled(bInSwingActive);

	return bIsSwingActive;
}

bool UWeaponTraceComponent::IsSwingActive_Implementation() const
{
	return bIsSwingActive;
}

void UWeaponTraceComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bRegisterOwnerOnBeginPlay)
	{
		Execute_SetOwnerActor(this, GetOwner()->GetParentActor());
	}
}

TArray<FVector> UWeaponTraceComponent::CalculateTracePoints_Implementation()
{
	TArray<FVector> TracePoints;
	
	TArray<FVector> TraceLocations = GetTraceLocations();

	if (TraceLocations.Num() != 2)
	{
		return {};
	}

	FVector StartLocation = TraceLocations[0];
	FVector EndLocation = TraceLocations[1];
	
	FVector Direction = (EndLocation - StartLocation).GetSafeNormal();

	for (int32 i = 0; i < TraceCount - 1; ++i)
	{		
		FVector Point = StartLocation + (Direction * RequiredSpacing * i);
		TracePoints.Add(Point);
	}

	return TracePoints;
}

void UWeaponTraceComponent::CalculateTraceCount_Implementation()
{
	TArray<FVector> TraceLocations = GetTraceLocations();

	if (TraceLocations.Num() == 2)
	{
		FVector StartLocation = TraceLocations[0];
		FVector EndLocation = TraceLocations[1];

		float Distance = FVector::Dist(StartLocation, EndLocation);

		TraceCount = FMath::CeilToInt(Distance / RequiredSpacing);
	}
	else
	{
		TraceCount = 0;
	}
}

void UWeaponTraceComponent::Trace_Implementation()
{
	TArray<FVector> TracePoints = CalculateTracePoints();

	TArray<FWeaponHitResult> HitResults;
	FWeaponHitResult NearestHitResult;
	float NearestHitDistance = FLT_MAX;

	bool bHitAnything = false;

	for (int i = 0; i < TracePoints.Num(); i++)
	{
		FHitResult HitResult;

		if (PrevTracePoints.IsEmpty())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
			                                 TEXT("PrevTracePoints is empty"));
			return;
		}

		FVector TraceStart = PrevTracePoints[i];
		FVector TraceEnd = TracePoints[i];

		bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(
			GetWorld(),
			TraceStart,
			TraceEnd,
			ObjectTypesToTrace,
			false,
			ActorsToIgnore,
			DrawDebugType0,
			HitResult,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			DrawDebugTime0
		);

		GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red,
		                                 TEXT("Traced"));

		if (bHit)
		{
			bHitAnything = true;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
			                                 FString::Printf(TEXT("Hit: %s"), *HitResult.GetActor()->GetName()));

			FVector Velocity = (HitResult.TraceEnd - HitResult.TraceStart * GetWorld()->DeltaTimeSeconds) * 100.0f;

			FWeaponHitResult WeaponHitResult(HitResult, Velocity);

			HitResults.Add(WeaponHitResult);

			if (HitResult.Distance < NearestHitDistance)
			{
				NearestHitDistance = HitResult.Distance;
				NearestHitResult = WeaponHitResult;
			}
		}
	}

	if (bHitAnything)
	{
		if (WeaponTraceHitDelegate.IsBound())
		{
			WeaponTraceHitDelegate.Broadcast(HitResults, NearestHitResult);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
		                                 TEXT("No hit detected"));
	}
}

bool UWeaponTraceComponent::AreWeaponSocketsValid() const
{
	return GetTraceLocations().Num() == 2;
}

bool UWeaponTraceComponent::AreWeaponSceneComponentsValid() const
{
	return WeaponStartSceneComponent && WeaponEndSceneComponent;
}

void UWeaponTraceComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
