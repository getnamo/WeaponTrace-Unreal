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

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FWeaponTraceHitDelegate WeaponTraceHitDelegate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="WeaponTrace")
	AActor* OwnerActor;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="WeaponTrace")
	bool bRegisterOwnerOnBeginPlay = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="WeaponTrace")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesToTrace;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="WeaponTrace")
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugTraceType = EDrawDebugTrace::Persistent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="WeaponTrace")
	float TraceDebugDuration = 5;

	//temp disable for now
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "WeaponTrace")
	float bNativeTraceEnabled = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="WeaponTrace")
	float TraceSpacing = 10.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="WeaponTrace")
	TArray<FVector> PreviousTracePoints;

	/* IWeaponTraceComponentInterface */
	virtual UWeaponTraceComponent* GetWeaponTraceComponent_Implementation() override;
	virtual AActor* GetOwnerActor_Implementation() override;
	virtual void SetOwnerActor_Implementation(AActor* InOwnerActor) override;
	virtual bool SetSwingActive_Implementation(bool bInSwingActive) override;
	virtual bool IsSwingActive_Implementation() const override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsWeaponValid();


	UFUNCTION(BlueprintCallable)
	void BeginSwing();

	UFUNCTION(BlueprintCallable)
	void EndSwing();

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="WeaponTrace")
	USceneComponent* WeaponBaseSceneComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="WeaponTrace")
	USceneComponent* WeaponTipSceneComponent;

	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> ActorsToIgnore;

	UPROPERTY(BlueprintReadWrite)
	bool bIsSwingActive;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Trace();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TArray<FVector> GetTraceLocations();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TArray<FVector> GetWeaponTracePoints();

public:
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
