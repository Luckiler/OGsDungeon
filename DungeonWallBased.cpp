// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeon002.h"
#include "DungeonWallBased.h"


// Sets default values
ADungeonWallBased::ADungeonWallBased()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PrimaryActorTick.bCanEverTick = true;

	generatorRoot = CreateAbstractDefaultSubobject<USceneComponent>(TEXT("Root"));

	floor = CreateAbstractDefaultSubobject<UStaticMeshComponent>(TEXT("Floor Base Mesh"));
	wall = CreateAbstractDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Wall Base Mesh"));
	debugConnector = CreateAbstractDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Debug Connector"));

	floor->SetupAttachment(generatorRoot);
	wall->SetupAttachment(generatorRoot);
	debugConnector->SetupAttachment(generatorRoot);
}

// Called when the game starts or when spawned
void ADungeonWallBased::BeginPlay()
{
	Super::BeginPlay();

	roomInitializer(FVector(0, 0, 0), FVector(dungeonDim.X, dungeonDim.Y, dungeonDim.Z));

	for(size_t i = 0; i < rooms.size(); i++)
	{
		connectionSetUp(rooms[i]);
		positionDef(rooms[i]);
	}



	/*for(size_t i = 0; i < rooms.size(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("\tRoom %d connected to:"), rooms[i].ID);
		for(size_t j = 0; j < rooms[i].nextTo.size(); j++)
		{
			UE_LOG(LogTemp, Warning, TEXT("\t\t - %d "), rooms[i].nextTo[j]);
		}
	}*/
}

// Called every frame
void ADungeonWallBased::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADungeonWallBased::OnConstruction(const FTransform& Transform)
{
	floor->SetWorldScale3D(dungeonDim);

	wall->ClearInstances();

	//Places the 4 walls surounding the dungeon
	{
		FTransform wallTransform;

		wallTransform.SetScale3D(FVector(dungeonDim.X, wallDim[0], wallDim[1]));
		{
			wallTransform.SetTranslation(FVector(0.0f, (dungeonDim.Y - wallDim[0]) * 50, (dungeonDim.Z + wallDim[1]) * 50));
			wall->AddInstance(wallTransform);

			wallTransform.SetTranslation(FVector(0.0f, (dungeonDim.Y * -50) + wallDim[0] * 50, (dungeonDim.Z + wallDim[1]) * 50));
			wall->AddInstance(wallTransform);
		}

		wallTransform.SetScale3D(FVector(wallDim[0], dungeonDim.Y, wallDim[1]));
		{
			wallTransform.SetTranslation(FVector((dungeonDim.X - wallDim[0]) * 50, 0.0f, (dungeonDim.Z + wallDim[1]) * 50));
			wall->AddInstance(wallTransform);

			wallTransform.SetTranslation(FVector((dungeonDim.X - wallDim[0]) * -50, 0.0f, (dungeonDim.Z + wallDim[1]) * 50));
			wall->AddInstance(wallTransform);
		}
	}
}

void ADungeonWallBased::roomInitializer(FVector position, FVector scale)
{
	char split = '1';
	float splitCoord;
	FTransform wallTrans;

	iterations++;

	//UE_LOG(LogTemp, Warning, TEXT("Scale is %f, %f"), scale.X, scale.Y);

	// Decide wether we should split, if yes on which axis
	if(scale.X <= minRoomSize * 2 && scale.Y <= minRoomSize * 2 || iterations >= 100000 || depth >= 10)
	{
		split = '0';
		//UE_LOG(LogTemp, Warning, TEXT("Iterations %d, Depth %d"), iterations, depth);
	} else
	{
		if(scale.X <= minRoomSize * 2)
		{
			split = 'y';
			//UE_LOG(LogTemp, Warning, TEXT("Split Y"));
		} else
			if(scale.Y <= minRoomSize * 2)
			{
				split = 'x';
				//UE_LOG(LogTemp, Warning, TEXT("Split X"));
			} else
			{
				if(FMath::RandBool())
				{
					split = 'x';
					//UE_LOG(LogTemp, Warning, TEXT("Split X"));
				} else
				{
					split = 'y';
					//UE_LOG(LogTemp, Warning, TEXT("Split Y"));
				}
			}
	}

	float minSplit = 0;
	float maxSplit = 0;

	switch(split)
	{
		case 'x':
			/*
			* Random coordinate to split
			* leaving a margin so the room isn't
			* smaller than the minimum size
			*/
			minSplit = (scale.X * 50 - minRoomSize * 100) * -1;

			maxSplit = (scale.X * 50 - minRoomSize * 100);

			splitCoord = FMath::FRandRange(minSplit, maxSplit);
			//UE_LOG(LogTemp, Warning, TEXT("Split coord X =  %f"), splitCoord);

			wallTrans.SetTranslation(FVector(position.X + splitCoord, position.Y, (scale.Z + wallDim[1]) * 50));
			wallTrans.SetScale3D(FVector(wallDim[0], scale.Y, wallDim[1]));
			wall->AddInstance(wallTrans);

			/*
			* We then initilasize two other rooms
			* in the middle of the two rooms we
			* created by placing the wall
			*/
			depth++;
			roomInitializer(FVector(position.X + (splitCoord + ((scale.X * 50 - splitCoord) / 2)), position.Y, position.Z), FVector(FGenericPlatformMath::Abs(scale.X * (((scale.X * 50) - (splitCoord)) / (scale.X * 100))), scale.Y, scale.Z));
			roomInitializer(FVector(position.X - (((scale.X * 50 + splitCoord) / 2) - splitCoord), position.Y, position.Z), FVector(FGenericPlatformMath::Abs(scale.X * (((scale.X * 50) + (splitCoord)) / (scale.X * 100))), scale.Y, scale.Z));
			depth--;
			break;
		case'y':
			/*
			* Random coordinate to split
			* leaving a margin so the room isn't
			* smaller than the minimum size
			*/
			minSplit = (scale.Y * 50 - minRoomSize * 100) * -1;

			maxSplit = (scale.Y * 50 - minRoomSize * 100);

			splitCoord = FMath::FRandRange(minSplit, maxSplit);
			//UE_LOG(LogTemp, Warning, TEXT("Split coord Y =  %f"), splitCoord);

			splitCoord = FMath::FRandRange((scale.Y * 50 - minRoomSize * 100) * -1, (scale.Y * 50 - minRoomSize * 100));

			wallTrans.SetTranslation(FVector(position.X, position.Y + splitCoord, (scale.Z + wallDim[1]) * 50));
			wallTrans.SetScale3D(FVector(scale.X, wallDim[0], wallDim[1]));
			wall->AddInstance(wallTrans);

			/*
			* We then initilasize two other rooms
			* in the middle of the two rooms we
			* created by placing the wall
			*/
			depth++;
			roomInitializer(FVector(position.X, position.Y + (splitCoord + ((scale.Y * 50 - splitCoord) / 2)), position.Z), FVector(scale.X, FGenericPlatformMath::Abs(scale.Y * (((scale.Y * 50) - (splitCoord)) / (scale.Y * 100))), scale.Z));
			roomInitializer(FVector(position.X, position.Y - (((scale.Y * 50 + splitCoord) / 2) - splitCoord), position.Z), FVector(scale.X, FGenericPlatformMath::Abs(scale.Y * (((scale.Y * 50) + (splitCoord)) / (scale.Y * 100))), scale.Z));
			depth--;
			break;
		case'0':
			rooms.push_back({});
			rooms[roomID].ID = roomID;
			rooms[roomID].position = position;
			rooms[roomID].size = FVector(scale.X, scale.Y, scale.Z);
			roomID++;
			break;
		default:
			UE_LOG(LogTemp, Error, TEXT("Split not initialised"));
			break;
	}
}

void ADungeonWallBased::connectionSetUp(roomProperty roomProp)
{
	std::vector<int> candidates;
	std::vector<int> connected;

	// Check all posible candidates
	for(size_t i = 0; i < rooms.size(); i++)
	{
		if(rooms[i].position.X - rooms[i].size.X * 50 < roomProp.position.X &&
		   roomProp.position.X < rooms[i].position.X + rooms[i].size.X * 50 &&
		   rooms[i].ID != roomProp.ID)
		{
			if(rooms[i].position.Y - rooms[i].size.Y * 50 < roomProp.position.Y + roomProp.size.Y * 50 + 10 &&
			   roomProp.position.Y + roomProp.size.Y * 50 + 10 < rooms[i].position.Y + rooms[i].size.Y * 50 ||
			   rooms[i].position.Y - rooms[i].size.Y * 50 < roomProp.position.Y - roomProp.size.Y * 50 - 10 &&
			   roomProp.position.Y + roomProp.size.Y * 50 - 10 < rooms[i].position.Y + rooms[i].size.Y * 50)
			{
				candidates.push_back(i);
				//UE_LOG(LogTemp, Warning, TEXT("\tFound connection: %d with %d"), rooms[i].ID, roomProp.ID);
			}
		}
		if(rooms[i].position.Y - rooms[i].size.Y * 50 < roomProp.position.Y &&
		   roomProp.position.Y < rooms[i].position.Y + rooms[i].size.Y * 50 &&
		   rooms[i].ID != roomProp.ID)
		{
			if(rooms[i].position.X - rooms[i].size.X * 50 < roomProp.position.X + roomProp.size.X * 50 + 10 &&
			   roomProp.position.X + roomProp.size.X * 50 + 10 < rooms[i].position.X + rooms[i].size.X * 50 ||
			   rooms[i].position.X - rooms[i].size.X * 50 < roomProp.position.X - minRoomSize * 50 - 10 &&
			   roomProp.position.X + roomProp.size.X * 50 - 10 < rooms[i].position.X + rooms[i].size.X * 50)
			{
				candidates.push_back(i);
				//UE_LOG(LogTemp, Warning, TEXT("\tFound connection: %d with %d"), rooms[i].ID, roomProp.ID);
			}
		}
	}
	
	connected.resize(candidates.size());
	for(size_t i = 0; i < candidates.size(); i++)
	{
		connected[i] = candidates[i];
	}
	
	FTransform debugTrans;
	for(size_t i = 0; i < connected.size(); i++)
	{
		rooms[roomProp.ID].nextTo.push_back(connected[i]);
		rooms[connected[i]].nextTo.push_back(roomProp.ID);
		/*debugTrans.SetLocation(FVector(	(rooms[roomProp.ID].position.X + rooms[connected[i]].position.X) / 2,
										(rooms[roomProp.ID].position.Y + rooms[connected[i]].position.Y) / 2,
										(dungeonDim.Z + wallDim[1] / 2) * 100));
		debugTrans.SetScale3D(FVector(FMath::Abs(rooms[roomProp.ID].position.X - rooms[connected[i]].position.X) / 100, FMath::Abs(rooms[roomProp.ID].position.Y - rooms[connected[i]].position.Y) / 100, 0.5f));
		debugConnector->AddInstance(debugTrans);*/
	}

}

void ADungeonWallBased::positionDef(roomProperty roomProp)
{
	if(roomProp.position.X + roomProp.size.X * 50 + 100 > dungeonDim.X * 50 ||
	   roomProp.position.Y + roomProp.size.Y * 50 + 100 > dungeonDim.Y * 50 ||
	   roomProp.position.X - roomProp.size.X * 50 - 100 < dungeonDim.X * -50 ||
	   roomProp.position.Y - roomProp.size.Y * 50 - 100 < dungeonDim.Y * -50)
	{
		rooms[roomProp.ID].isEdge = true;
		edgesIDs.push_back(roomProp.ID);
		wall->AddInstance(FTransform(FRotator(0.0f), roomProp.position, FVector(1.0f, 1.0f, 5.0f)));
	}
		
	//make setup for corners
}
