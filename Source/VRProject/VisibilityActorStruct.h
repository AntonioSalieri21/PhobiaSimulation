// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VisibilityPointComponent.h"
#include "VisibilityActorStruct.generated.h"  // This must match the file name

/**
 * 
 */
USTRUCT(BlueprintType)
struct VRPROJECT_API FVisibilityActorStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visibility")
	AActor* Actor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visibility")
	int32 Priority;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visibility")
	FVector BoxCenter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visibility")
	FVector BoxExtent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visibility")
	TArray< UVisibilityPointComponent* > VisibilityPoints;

};
