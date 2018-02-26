// Fill out your copyright notice in the Description page of Project Settings.

#include "Block.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ABlock::ABlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABlock::BeginPlay()
{
	Super::BeginPlay();
	BlockMesh = FindComponentByClass<UStaticMeshComponent>();
}

// Called every frame
void ABlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//puts neighbors of Block in Neighbors Array, adds their indices to NeighborIndex Array
TArray<int32> ABlock::CheckNeighbors()
{
	TArray<AActor*> Neighbors;
	TArray<int32> NeighborIndex;
	if (!BlockMesh) return NeighborIndex; 
	else BlockMesh->GetOverlappingActors(Neighbors); //OUT parameter
	for (auto& Elem : Neighbors)
	{
		if (Cast<ABlock>(Elem))
		{
			ABlock* BlockInst = Cast<ABlock>(Elem);
			NeighborIndex.Add(BlockInst->Index);
		}
	}
	return NeighborIndex;
}
