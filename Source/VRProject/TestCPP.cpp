// Fill out your copyright notice in the Description page of Project Settings.


#include "TestCPP.h"

// Sets default values
ATestCPP::ATestCPP()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATestCPP::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATestCPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Red, FString::Printf(TEXT("TEST")));
	}

}

void ATestCPP::SpawnActor()
{
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(actorToSpawn, GetActorTransform(), spawnParams);

}

