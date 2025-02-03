// Fill out your copyright notice in the Description page of Project Settings.


#include "CFL_VisibilityCheck.h"

// Sets default values
ACFL_VisibilityCheck::ACFL_VisibilityCheck()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACFL_VisibilityCheck::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACFL_VisibilityCheck::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

