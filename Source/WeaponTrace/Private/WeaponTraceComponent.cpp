#include "WeaponTraceComponent.h"

UWeaponTraceComponent::UWeaponTraceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
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

TArray<FVector> UWeaponTraceComponent::GetTraceSocketLocations_Implementation() const
{
	TArray<FVector> SocketLocations;

	if (UActorComponent* StaticMeshComponent = GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass()))
	{
		if (UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(StaticMeshComponent))
		{
			if (MeshComponent->DoesSocketExist("WeaponStart"))
			{
				SocketLocations.Add(MeshComponent->GetSocketLocation("WeaponStart"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Socket 'WeaponStart' does not exist on mesh component"));
			}
			
			if (MeshComponent->DoesSocketExist("WeaponEnd"))
			{
				SocketLocations.Add(MeshComponent->GetSocketLocation("WeaponEnd"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Socket 'WeaponEnd' does not exist on mesh component"));
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

bool UWeaponTraceComponent::IsWeaponValid_Implementation() const
{
	return Execute_GetTraceSocketLocations(this).Num() == 2;
}

void UWeaponTraceComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWeaponTraceComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
