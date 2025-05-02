#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponTraceComponentInterface.generated.h"

class UWeaponTraceComponent;
// This class does not need to be modified.
UINTERFACE()
class WEAPONTRACE_API UWeaponTraceComponentInterface : public UInterface
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FWeaponHitResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FHitResult HitResult;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Velocity;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponTraceHitDelegate,
	const TArray<FWeaponHitResult>&, HitResults,
	const FWeaponHitResult&, NearestHitResult);

class WEAPONTRACE_API IWeaponTraceComponentInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "WeaponTrace")
	UWeaponTraceComponent* GetWeaponTraceComponent();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "WeaponTrace")
	void SetOwnerActor(AActor* InOwnerActor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "WeaponTrace")
	AActor* GetOwnerActor();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "WeaponTrace")
	bool SetSwingActive(bool bInSwingActive);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "WeaponTrace")
	bool IsSwingActive() const;
};
