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

// Called every frame
void AGridController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
			FVector HexPos;
			FVector BlockPos;
			for (int32 CounterY = 0; CounterY <= SizeY; CounterY++)
			{
				if (bOddNum)
				{
					HexPos = FVector(float(CounterX * 35), 0, float(CounterY * 40 + 20));
					BlockPos = FVector(float(CounterX * 35), 24.f, float(CounterY * 40 + 20));
				}
				else
				{
					HexPos = FVector(float(CounterX * 35), 0, float(CounterY * 40));
					BlockPos = FVector(float(CounterX * 35), 24.f, float(CounterY * 40));
				}

				//Spawn Hexgrid
				FTransform HexTransform;
				HexTransform.SetLocation(HexPos);
				GetWorld()->SpawnActor<AActor>(HexBP, HexTransform, SpawnParams);
				
				//Spawn Blocks
				
				ABlock *BlockInst = nullptr;
				FTransform BlockTransform;
				BlockTransform.SetLocation(BlockPos);
				int32 Identifier = SetIdentifier();
				switch (Identifier)
				{
				case 0:
					BlockInst = GetWorld()->SpawnActor<ABlock>(BlueBlockBP, BlockTransform, SpawnParams);
					BlockInst->Identifier = Identifier;
					break;
				case 1:
					BlockInst = GetWorld()->SpawnActor<ABlock>(GreenBlockBP, BlockTransform, SpawnParams);
					BlockInst->Identifier = Identifier;
					break;
				case 2:
					BlockInst = GetWorld()->SpawnActor<ABlock>(RedBlockBP, BlockTransform, SpawnParams);
					BlockInst->Identifier = Identifier;
					break;
				case 3:
					BlockInst = GetWorld()->SpawnActor<ABlock>(YellowBlockBP, BlockTransform, SpawnParams);
					BlockInst->Identifier = Identifier;
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

		//GetInput, get selected blocks
		auto InputComponent = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->FindComponentByClass<UInputComponent>();
		if (InputComponent)
		{
			InputComponent->BindAction("SelectBlock", IE_Pressed, this, &AGridController::Select); 
		}
		
		if (SelectedBlocks.Num() != 0)
		{
			DestroyBlocks(SelectedBlocks);
			//drop blocks
			for (int i = 0; i < (SizeX + 1) * (SizeY + 1); i++)
			{
				if (!BlockMap.Contains(i))
				{
					bool bDrop = false;
					for (int j = i + 1; j % (SizeY + 1) != 0; j++)
					{
						if (BlockMap.Contains(j))
						{
							auto BlockToDrop = BlockMap[j];
							BlockMap.Remove(j);
							BlockMap.Add(i, BlockToDrop);
							bDrop = true;
						}
					}
					if (!bDrop)
					{
						ABlock* BlockInst = nullptr;
						FVector Position = PositionFromIndex(i);
						int32 CounterX = (int32)Position.X;
						int32 CounterY = (int32)Position.Z;
						FVector BlockPos;
						if (CounterX % 2 == 0) bOddNum = false;
						else bOddNum = true;
						if (bOddNum)
						{
							BlockPos = FVector(float(CounterX * 35), 24.f, float(CounterY * 40 + 20));
						}
						else
						{
							BlockPos = FVector(float(CounterX * 35), 24.f, float(CounterY * 40));
						}

						FTransform BlockTransform;
						BlockTransform.SetLocation(BlockPos);
						int32 Identifier = SetIdentifier();
						switch (Identifier)
						{
						case 0:
							BlockInst = GetWorld()->SpawnActor<ABlock>(BlueBlockBP, BlockTransform, SpawnParams);
							BlockInst->Identifier = Identifier;
							break;
						case 1:
							BlockInst = GetWorld()->SpawnActor<ABlock>(GreenBlockBP, BlockTransform, SpawnParams);
							BlockInst->Identifier = Identifier;
							break;
						case 2:
							BlockInst = GetWorld()->SpawnActor<ABlock>(RedBlockBP, BlockTransform, SpawnParams);
							BlockInst->Identifier = Identifier;
							break;
						case 3:
							BlockInst = GetWorld()->SpawnActor<ABlock>(YellowBlockBP, BlockTransform, SpawnParams);
							BlockInst->Identifier = Identifier;
							break;
						}
						
						BlockMap.Add(i, BlockInst);
					}
				}
			}

			//set blocks new position
			for (auto& Block : BlockMap)
			{
				FVector Position = PositionFromIndex(Block.Key);
				int32 CounterX = (int32)Position.X;
				int32 CounterY = (int32)Position.Z;
				FVector BlockPos;
				if (CounterX % 2 == 0) bOddNum = false;
				else bOddNum = true;
				if (bOddNum)
				{
					BlockPos = FVector(float(CounterX * 35), 24.f, float(CounterY * 40 + 20));
					Block.Value->SetActorLocation(BlockPos);
				}
				else
				{
					BlockPos = FVector(float(CounterX * 35), 24.f, float(CounterY * 40));
					Block.Value->SetActorLocation(BlockPos);
				}
			}
		}

	}
	else UE_LOG(LogTemp, Error, TEXT("Blueprint Reference missing in GridController_BP > Actor Spawning"))
}


int32 AGridController::SetIdentifier()
{
	int32 Identifier = FMath::RandRange(0, 3);
	return Identifier;
}

void AGridController::Select()
{
	auto PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	FVector WorldPos;
	FVector WorldDir;
	PlayerController->DeprojectMousePositionToWorld(WorldPos, WorldDir);
	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, this);
	GetWorld()->LineTraceSingleByObjectType(Hit, WorldPos, WorldPos + WorldDir * 1000000000, FCollisionObjectQueryParams(ECollisionChannel::ECC_GameTraceChannel1), TraceParams);
	DrawDebugLine(GetWorld(), WorldPos, Hit.Location, FColor(255, 0, 0), false, -1.f, 0, .2);
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
}

void AGridController::DebugLog()
{
	UE_LOG(LogTemp, Warning, TEXT("Called"))
}

bool AGridController::bIsSameColor(TArray<int32> SelectedBlocks)
{
	if (SelectedBlocks.IsValidIndex(0))
	{
		auto FirstBlock = BlockMap[SelectedBlocks[0]];
		auto FirstBlockIdent = FirstBlock->Identifier;
		UE_LOG(LogTemp, Warning, TEXT("first block: %d"), FirstBlockIdent)
		for (auto& Index : SelectedBlocks)
		{
			UE_LOG(LogTemp, Warning, TEXT("other block: %d"), BlockMap[Index]->Identifier)
			if (BlockMap[Index]->Identifier != FirstBlockIdent) return false;
		}
		return true;
	}
	return false;
}

void AGridController::DestroyBlocks(TArray<int32> &Selection)
{
	if (Selection.Num() > 2)
	{
		if (bIsSameColor(Selection))
		{
			for (auto& Index : Selection)
			{
				if (BlockMap.Contains(Index))
				{
					auto BlockToDestroy = BlockMap[Index];
					BlockMap.Remove(Index);
					BlockToDestroy->Destroy();
				}
			}
		}
	}
}

FVector AGridController::PositionFromIndex(int32 Index)
{
	int32 PosX = Index / (SizeY + 1);
	int32 PosY = Index % (SizeY + 1);
	return FVector(PosX, 0.f, PosY);
}