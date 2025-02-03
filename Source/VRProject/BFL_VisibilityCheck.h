// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BFL_VisibilityCheck.generated.h"

/**
 * 
 */
UCLASS()
class VRPROJECT_API UBFL_VisibilityCheck : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
		UFUNCTION(BlueprintCallable, Category = "VisibilityCheck")
		static bool IsVisibleToPlayer(UCameraComponent camera, AActor* object);
	
};
