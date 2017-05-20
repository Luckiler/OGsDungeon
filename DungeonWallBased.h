// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include <vector>
#include "DungeonWallBased.generated.h"

UCLASS()
class DUNGEON002_API ADungeonWallBased : public AActor
{
	GENERATED_BODY()
	

public:
	int depth = 0;
	int iterations = 0;
	int dungeonType;
	int mission;
	int difficulty;
	int tier;
	std::vector<int> edgesIDs;
	struct roomProperty
	{
		int ID;
		FVector position;
		FVector size;
		bool isCorner = false;
		bool isEdge = false;
		std::vector<int> nextTo;
	};
	int roomID = 0;
	std::vector<roomProperty> rooms;

	UPROPERTY(EditAnywhere)
		FVector dungeonDim = FVector(10.0f, 10.0f, 1.0f);
	UPROPERTY(EditAnywhere)
		float wallDim[2] = {0.5f, 3.0};
	UPROPERTY(EditAnywhere)
		float minRoomSize = 10;
	UPROPERTY(EditAnywhere)
		float doorSize = 2;

	UPROPERTY(EditAnywhere)
		USceneComponent* generatorRoot;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* floor;
	UPROPERTY(EditAnywhere)
		UInstancedStaticMeshComponent* wall;
	UPROPERTY(EditAnywhere)
		UInstancedStaticMeshComponent* debugConnector;

	// Sets default values for this actor's properties
	ADungeonWallBased();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called when inetracted with in the editor
	virtual void OnConstruction(const FTransform& Transform) override;

	// Sets up the rooms in the designated zone
	void roomInitializer(FVector position, FVector size);

	void roomCreator(roomProperty roomProp);

	// Sets up the connections between the rooms
	void connectionSetUp(roomProperty roomProp);

	void positionDef(roomProperty roomProp);

};
