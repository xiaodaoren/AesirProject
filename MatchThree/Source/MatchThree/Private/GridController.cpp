// Fill out your copyright notice in the Description page of Project Settings.

#include "GridController.h"
#include "Engine/World.h"
#include "Containers/Array.h"
#include "Block.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


// Sets default values
AGridController::AGridController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGridController::BeginPlay()
{
	Super::BeginPlay();
	auto PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->SetCinematicMode(true, true, true); //Prevents camera from moving with mouse movement
	if (HexBP && BlueBlockBP && RedBlockBP && GreenBlockBP && YellowBlockBP)
	{
		int32 Index = 0;
		for (int32 CounterX = 0; CounterX <= SizeX; CounterX++)
		{
			//set different Z positions for even and odd numbered columns
			if (CounterX % 2 == 0) bOddNum = false;
			else bOddNum = true;
			for (int32 CounterY = 0; CounterY <= SizeY; CounterY++)
			{
				if (bOddNum)
				{
					HexPos = FVector(float(CounterX * 35), 0, float(CounterY * 40 + 20));
					BlockPos = FVector(float(CounterX * 35), -24.f, float(CounterY * 40 + 20));
				}
				else
				{
					HexPos = FVector(float(CounterX * 35), 0, float(CounterY * 40));
					BlockPos = FVector(float(CounterX * 35), -24.f, float(CounterY * 40));
				}
				
				FTransform HexTransform;
				FTransform BlockTransform;

				HexTransform.SetLocation(HexPos);
				BlockTransform.SetLocation(BlockPos);

				GetWorld()->SpawnActor<AActor>(HexBP, HexTransform, SpawnParams);
				int32 Identifier = SetIdentifier();
				ABlock *BlockInst = nullptr;
				switch (Identifier)
				{
				case 0:
					BlockInst = GetWorld()->SpawnActor<ABlock>(BlueBlockBP, BlockTransform, SpawnParams);
					break;
				case 1:
					BlockInst = GetWorld()->SpawnActor<ABlock>(GreenBlockBP, BlockTransform, SpawnParams);
					break;
				case 2:
					BlockInst = GetWorld()->SpawnActor<ABlock>(RedBlockBP, BlockTransform, SpawnParams);
					break;
				case 3:
					BlockInst = GetWorld()->SpawnActor<ABlock>(YellowBlockBP, BlockTransform, SpawnParams);
					break;
				}
				if (BlockInst)
				{
					BlockMap.Add(Index, BlockInst);
					BlockInst->Index = Index;
					Index++;
				}
			}
		}
		

		//auto InputComponent = FindComponentByClass<UInputComponent>();
		//if (InputComponent)
		//{
		//	InputComponent->BindAction("SelectBlock", IE_Pressed, this, &AGridController::Select); //TODO fix
		//}

	}
	else UE_LOG(LogTemp, Error, TEXT("Blueprint Reference missing in GridController_BP > Actor Spawning"))
}

// Called every frame
void AGridController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Select();
}

int32 AGridController::SetIdentifier()
{
	int32 Identifier = FMath::RandRange(0, 3);
	return Identifier;
}

TArray<int32> AGridController::Select()
{
	TArray<int32> SelectedBlocks;
	auto PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	FVector WorldPos;
	FVector WorldDir;
	PlayerController->DeprojectMousePositionToWorld(WorldPos, WorldDir);
	FHitResult Hit;
	FVector LineStart;
	FRotator ActorRot;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(LineStart, ActorRot);
	GetWorld()->LineTraceSingleByObjectType(Hit, LineStart, WorldPos + WorldDir * 10, ECollisionChannel::ECC_Visibility);
	DrawDebugLine(GetWorld(), LineStart, WorldPos, FColor(255, 0, 0), false, -1.f, 0, .2);
	ABlock* BlockInst = nullptr;
	const int32* BlockIndex = nullptr;
	//gets Block that was hit 
	if (Cast<ABlock>(Hit.GetActor())) { BlockInst = Cast<ABlock>(Hit.GetActor()); 	UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *BlockInst->GetName()) }
	//finds corresponding index in BlockMap
	if (BlockInst) BlockIndex = BlockMap.FindKey(BlockInst); 
	// adds index to Selected Blocks array
	if (BlockIndex) SelectedBlocks.Add(BlockIndex[0]); 
	for (auto& Elem : SelectedBlocks)
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected: %d"), Elem)
	}
	return SelectedBlocks;
}

TArray<int32> AGridController::FindBlocksToDestroy()
{
	TArray<int32> BlocksToDestroy;
	return BlocksToDestroy;
}