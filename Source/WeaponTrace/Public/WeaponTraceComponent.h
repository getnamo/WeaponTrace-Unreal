#pragma once

#include "CoreMinimal.h"
#include "WeaponTraceComponentInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Runtime/Engine/Classes/Components/ActorComponent.h"
#include "WeaponTraceComponent.generated.h"

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WEAPONTRACE_API UWeaponTraceComponent : public UActorComponent, public IWeaponTraceComponentInterface
{
	GENERATED_BODY()

public:

	UWeaponTraceComponent();

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="WeaponTrace")
	FWeaponTraceHitDelegate WeaponTraceHitDelegate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="WeaponTrace")
	AActor* OwnerActor;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="WeaponTrace")
	bool bRegisterOwnerOnBeginPlay = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="WeaponTrace")
	float RequiredSpacing = 10.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="WeaponTrace")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesToTrace;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="WeaponTrace")
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType0 = EDrawDebugTrace::Persistent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="WeaponTrace")
	float DrawDebugTime0 = 90.0f;

	/* IWeaponTraceComponentInterface */
	virtual UWeaponTraceComponent* GetWeaponTraceComponent_Implementation() override;
	virtual AActor* GetOwnerActor_Implementation() override;
	virtual void SetOwnerActor_Implementation(AActor* InOwnerActor) override;
	virtual bool SetSwingActive_Implementation(bool bInSwingActive) override;
	virtual bool IsSwingActive_Implementation() const override;

protected:

	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> ActorsToIgnore;

	UPROPERTY(BlueprintReadWrite)
	bool bIsSwingActive;

	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> PrevTracePoints;

	UPROPERTY(BlueprintReadWrite)
	USceneComponent* WeaponStartSceneComponent;

	UPROPERTY(BlueprintReadWrite)
	USceneComponent* WeaponEndSceneComponent;
	
	UPROPERTY(BlueprintReadWrite)
	int32 TraceCount;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure)
	TArray<FVector> GetTraceLocations() const;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure)
	TArray<FVector> CalculateTracePoints();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void CalculateTraceCount();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Trace();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool AreWeaponSocketsValid() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool AreWeaponSceneComponentsValid() const;

public:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
