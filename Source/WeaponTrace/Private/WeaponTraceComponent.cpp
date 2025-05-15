#include "WeaponTraceComponent.h"

UWeaponTraceComponent::UWeaponTraceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	ObjectTypesToTrace = {ObjectTypeQuery1, ObjectTypeQuery2, ObjectTypeQuery4};
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

	if (OwnerActor)
	{
		ActorsToIgnore.Add(OwnerActor);
		ActorsToIgnore.Add(GetOwner());
	}
	else
	{
		SetComponentTickEnabled(false);
	}
}

bool UWeaponTraceComponent::SetSwingActive_Implementation(bool bInSwingActive)
{
	bIsSwingActive = bInSwingActive;

	if (bIsSwingActive)
	{
		PreviousTracePoints = GetWeaponTracePoints();
	}

	SetComponentTickEnabled(bInSwingActive);

	return bIsSwingActive;
}

bool UWeaponTraceComponent::IsSwingActive_Implementation() const
{
	return bIsSwingActive;
}

bool UWeaponTraceComponent::IsWeaponValid_Implementation()
{
	return GetTraceLocations().Num() == 2;
}

void UWeaponTraceComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bRegisterOwnerOnBeginPlay)
	{
		Execute_SetOwnerActor(this, GetOwner()->GetParentActor());
	}
}

void UWeaponTraceComponent::Trace_Implementation()
{
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Trace Called"));
	TArray<FVector> TracePoints = GetWeaponTracePoints();

	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Trace Points Count: ") + FString::FromInt(TracePoints.Num()));

	FWeaponHitResult NearestHitResult;
	float NearestDistance = FLT_MAX;

	for (int32 i = 0; i < TracePoints.Num() - 1; ++i)
	{
		FVector Start = PreviousTracePoints[i];
		FVector End = TracePoints[i];

		FHitResult HitResult;

		bool bIsHit = UKismetSystemLibrary::LineTraceSingleForObjects(
			this,
			Start,
			End,
			ObjectTypesToTrace,
			false,
			ActorsToIgnore,
			DrawDebugTraceType,
			HitResult,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			TraceDebugDuration);

		if (bIsHit)
		{
			if (HitResult.Distance < NearestDistance)
			{
				NearestDistance = HitResult.Distance;
				NearestHitResult.HitResult = HitResult;
				NearestHitResult.Velocity = (End - Start) / GetWorld()->DeltaTimeSeconds;
			}
		}
	}

	if (NearestHitResult.HitResult.GetActor())
	{
		SetComponentTickEnabled(false);
		
		if (WeaponTraceHitDelegate.IsBound())
		{
			WeaponTraceHitDelegate.Broadcast(NearestHitResult);
		}
	}

	PreviousTracePoints = TracePoints;
}

TArray<FVector> UWeaponTraceComponent::GetTraceLocations_Implementation()
{
	TArray<FVector> TraceLocations;

	if (WeaponBaseSceneComponent && WeaponTipSceneComponent)
	{
		TraceLocations.Add(WeaponBaseSceneComponent->GetComponentLocation());
		TraceLocations.Add(WeaponTipSceneComponent->GetComponentLocation());

		return TraceLocations;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponBaseSceneComponent or WeaponTipSceneComponent is null"));
	}

	if (UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(
		GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass())))
	{
		if (MeshComponent)
		{
			TraceLocations.Add(MeshComponent->GetSocketLocation("WeaponStart"));
			TraceLocations.Add(MeshComponent->GetSocketLocation("WeaponEnd"));

			return TraceLocations;
		}
	}

	return {};
}

TArray<FVector> UWeaponTraceComponent::GetWeaponTracePoints_Implementation()
{
	TArray<FVector> TraceLocations = GetTraceLocations();

	if (TraceLocations.Num() == 2)
	{
		TArray<FVector> TracePoints;

		FVector Start = TraceLocations[0];
		FVector End = TraceLocations[1];

		FVector Direction = (End - Start).GetSafeNormal();
		float Distance = FVector::Distance(Start, End);
		int32 NumPoints = FMath::CeilToInt(Distance / TraceSpacing);

		for (int32 i = 0; i <= NumPoints; ++i)
		{
			FVector TracePoint = Start + Direction * (i * TraceSpacing);
			TracePoints.Add(TracePoint);
		}

		return TracePoints;
	}

	return {};
}

void UWeaponTraceComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsSwingActive)
	{
		SetComponentTickEnabled(false);
		return;
	}

	Trace();
}
