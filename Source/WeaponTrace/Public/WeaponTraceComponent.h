#pragma once

#include "CoreMinimal.h"
#include "WeaponTraceComponentInterface.h"
#include "Runtime/Engine/Classes/Components/ActorComponent.h"
#include "WeaponTraceComponent.generated.h"

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WEAPONTRACE_API UWeaponTraceComponent : public UActorComponent, public IWeaponTraceComponentInterface
{
	GENERATED_BODY()

public:

	UWeaponTraceComponent();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="WeaponTrace")
	AActor* OwnerActor;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="WeaponTrace")
	bool bRegisterOwnerOnBeginPlay = true;

	/* IWeaponTraceComponentInterface */
	virtual UWeaponTraceComponent* GetWeaponTraceComponent_Implementation() override;
	virtual AActor* GetOwnerActor_Implementation() override;
	virtual void SetOwnerActor_Implementation(AActor* InOwnerActor) override;
	virtual TArray<FVector> GetTraceSocketLocations_Implementation() const override;
	virtual bool SetSwingActive_Implementation(bool bInSwingActive) override;
	virtual bool IsSwingActive_Implementation() const override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsWeaponValid() const;

protected:

	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> ActorsToIgnore;

	UPROPERTY(BlueprintReadWrite)
	bool bIsSwingActive;

	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
