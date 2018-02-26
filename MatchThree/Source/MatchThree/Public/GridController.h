// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridController.generated.h"

class ABlock;

UCLASS()
class MATCHTHREE_API AGridController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<int32> FindBlocksToDestroy();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "ActorSpawning")
		TSubclassOf<AActor> HexBP;
	UPROPERTY(EditDefaultsOnly, Category = "ActorSpawning")
		TSubclassOf<ABlock> BlueBlockBP;
	UPROPERTY(EditDefaultsOnly, Category = "ActorSpawning")
		TSubclassOf<ABlock> GreenBlockBP;
	UPROPERTY(EditDefaultsOnly, Category = "ActorSpawning")
		TSubclassOf<ABlock> RedBlockBP;
	UPROPERTY(EditDefaultsOnly, Category = "ActorSpawning")
		TSubclassOf<ABlock> YellowBlockBP;
	UPROPERTY(EditAnywhere)
		int32 SizeX = 7;
	UPROPERTY(EditAnywhere)
		int32 SizeY = 6;
	
	FActorSpawnParameters SpawnParams;
	bool bOddNum;

	FVector HexPos;
	FVector BlockPos;

	TMap<int32, ABlock*> BlockMap;

	int32 SetIdentifier();

	TArray<int32> Select();
};
