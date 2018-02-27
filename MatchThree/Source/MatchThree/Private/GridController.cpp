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

	SizeX = 7;
	SizeY = 6;

	bSelectOnTick = false;
	Points = 0;
}

// Called every frame
void AGridController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bSelectOnTick == true)
	{
		Select();
		HighlightSelected();
	}
}

// Called when the game starts or when spawned
void AGridController::BeginPlay()
{
	Super::BeginPlay();

	BindInput();

	SuppressCamMovement();

	InitGame(); 
}

void AGridController::InitGame()
{
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

				int32 Identifier = RandIdentifier();
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
	}

	else UE_LOG(LogTemp, Error, TEXT("Blueprint Reference missing in GridController_BP > Actor Spawning"))
}

void AGridController::SuppressCamMovement()
{
	auto PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->SetCinematicMode(true, true, true); //Prevents camera from moving with mouse movement
}

void AGridController::BindInput()
{
	auto InputComponent = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		InputComponent->BindAction("SelectBlock", IE_Pressed, this, &AGridController::OnPress);
		InputComponent->BindAction("SelectBlock", IE_Released, this, &AGridController::OnRelease);
	}
}

void AGridController::DropAndRefill()
{
		//drop blocks
		//iterate over all positions in grid
		for (int i = 0; i < (SizeX + 1) * (SizeY + 1); i++)
		{
			//if current position has no block
			if (!BlockMap.Contains(i))
			{
				bool bDrop = false;

				//iterate over column (for all above positions of current block)
				for (int j = i + 1; j % (SizeY + 1) != 0; j++)
				{
					//if there is a block, drop it
					if (BlockMap.Contains(j))
					{
						auto BlockToDrop = BlockMap[j];
						BlockMap.Remove(j);
						BlockMap.Add(i, BlockToDrop);
						bDrop = true;
						j = -1; //will be 0 in next iteration to exit for loop
					}
				}

				//if no block was dropped, spawn a new one
				if (!bDrop)
				{
					ABlock* BlockInst = nullptr;
					FVector BlockPos;

					FVector2D Position = PositionFromIndex(i);
					int32 CounterX = (int32)Position.X;
					int32 CounterY = (int32)Position.Y;

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

					int32 Identifier = RandIdentifier();
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
			FVector BlockPos;
			
			FVector2D Position = PositionFromIndex(Block.Key);
			int32 CounterX = (int32)Position.X;
			int32 CounterY = (int32)Position.Y;

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


int32 AGridController::RandIdentifier()
{
	int32 Identifier = FMath::RandRange(0, 3);
	return Identifier;
}

void AGridController::OnPress()
{
	bSelectOnTick = true;
}

void AGridController::OnRelease()
{
	AddPoints();
	DestroyBlocks();
	bSelectOnTick = false;
	DropAndRefill();
}

void AGridController::Select()
{
	auto PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	FVector WorldPos;
	FVector WorldDir;
	PlayerController->DeprojectMousePositionToWorld(WorldPos, WorldDir);

	//line trace checks for blocks at cursor position
	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, this);
	GetWorld()->LineTraceSingleByObjectType(Hit, WorldPos, WorldPos + WorldDir * 1000000000, FCollisionObjectQueryParams(ECollisionChannel::ECC_GameTraceChannel1), TraceParams);
	DrawDebugLine(GetWorld(), WorldPos, Hit.Location, FColor(255, 0, 0), false, -1.f, 0, .2);

	ABlock* BlockInst = nullptr;
	const int32* BlockIndex = nullptr;

	//gets Block that was hit 
	if (Cast<ABlock>(Hit.GetActor())) 
	{ 
		BlockInst = Cast<ABlock>(Hit.GetActor()); 	

		//finds corresponding index in BlockMap
		BlockIndex = BlockMap.FindKey(BlockInst); 

		//if SelectedBlocks array is empty, add first index to it
		if (SelectedBlocks.Num() == 0)
		{
			SelectedBlocks.Add(BlockIndex[0]);
		}
		else
		{
			int32 LastElem = 0;
			if (SelectedBlocks.IsValidIndex(SelectedBlocks.Num() - 1))
			{
				LastElem = SelectedBlocks[SelectedBlocks.Num() - 1];
			}
			//is current element identical to the last?
			if (LastElem != *BlockIndex)
			{
				//Current block already in array?
				if (!SelectedBlocks.Contains(BlockIndex[0]))
				{
					//if first and new element have the same identifier (color)
					if (BlockMap[SelectedBlocks[0]]->Identifier == BlockMap[BlockIndex[0]]->Identifier)
					{
						// adds index to Selected Blocks array
						SelectedBlocks.Add(BlockIndex[0]);
					}
					else AbortTry();
				}
				else AbortTry();
			}
		}
	}
	else OnRelease();
}

void AGridController::DebugLog()
{
	UE_LOG(LogTemp, Warning, TEXT("Called"))
}

void AGridController::AbortTry()
{
	SelectedBlocks.Empty();
	bSelectOnTick = false;
}

void AGridController::DestroyBlocks()
{
	if (SelectedBlocks.Num() > 2)
	{
		for (auto& Index : SelectedBlocks)
		{
			if (BlockMap.Contains(Index))
			{
				auto BlockToDestroy = BlockMap[Index];
				BlockMap.Remove(Index);
				BlockToDestroy->Destroy();
			}
		}
		for (auto& SelectedBlock : SelectedBlocks)
		{
			BlockMap.Remove(SelectedBlock); 
		}
	}
	SelectedBlocks.Empty();
}

FVector2D AGridController::PositionFromIndex(int32 Index)
{
	int32 PosX = Index / (SizeY + 1);
	int32 PosY = Index % (SizeY + 1);
	return FVector2D(PosX, PosY);
}

void AGridController::HighlightSelected()
{
	if (SelectedBlocks.IsValidIndex(1))
	{
		for (int i = 0; i < SelectedBlocks.Num()-1; i++)
		{
			DrawDebugLine(GetWorld(), BlockMap[SelectedBlocks[i]]->GetActorLocation(), BlockMap[SelectedBlocks[i+1]]->GetActorLocation(), FColor(255, 0, 0), false, -1.f, 0, 4.f);
		}
	}
}

void AGridController::AddPoints()
{
	if (SelectedBlocks.Num() > 2)
	{
		for (int i = 0; i < SelectedBlocks.Num(); i++)
		{
			Points = Points + i;
		}
		UE_LOG(LogTemp, Warning, TEXT("Points: %d"), Points)
	}
}

//bool AGridController::bIsSameColor(TArray<int32> SelectedBlocks)
//{
//	if (SelectedBlocks.IsValidIndex(0))
//	{
//		auto FirstBlock = BlockMap[SelectedBlocks[0]];
//		auto FirstBlockIdent = FirstBlock->Identifier;
//		for (auto& Index : SelectedBlocks)
//		{
//			if (BlockMap[Index]->Identifier != FirstBlockIdent) return false;
//		}
//		return true;
//	}
//	return false;
//}