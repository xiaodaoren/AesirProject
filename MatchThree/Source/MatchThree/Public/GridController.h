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

	//Spawns hexagons and initial blocks according to selected grid size in blueprint
	void InitGame();

	void SuppressCamMovement();

	void BindInput();

	//drops blocks into empty spaces, or spawns new ones if none drop
	void DropAndRefill();

	void DestroyBlocks();

	//gets block position in 2d hexgrid from its index
	FVector2D PositionFromIndex(int32 Index);

	void HighlightSelected();

	void AddPoints();

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
		int32 SizeX;
	UPROPERTY(EditAnywhere)
		int32 SizeY;
	
	FActorSpawnParameters SpawnParams;
	
	TMap<int32, ABlock*> BlockMap;
	TArray<int32> SelectedBlocks;

	bool bOddNum;
	bool bSelectOnTick;

	int32 Points;

	//Identifier represents color
	int32 RandIdentifier();

	void OnPress();
	void OnRelease();
	void Select();
	void DebugLog();
	void AbortTry();

	//bool bIsSameColor(TArray<int32> SelectedBlocks);
};
