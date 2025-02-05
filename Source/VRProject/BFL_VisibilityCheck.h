// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "VisibilityActorStruct.h"
#include "BFL_VisibilityCheck.generated.h"

/**
 * 
 */
UCLASS()
class VRPROJECT_API UBFL_VisibilityCheck : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
		UFUNCTION(BlueprintCallable, Category = "VisibilityCheck")
		//static bool IsVisibleToPlayer(UCameraComponent* camera, AActor* object, float CameraAngle, AActor* IgnoredActor = nullptr, bool Debug = false);
		static bool IsVisibleToPlayer(UCameraComponent* camera,  FVisibilityActorStruct ActorData, float CameraAngle, AActor* IgnoredActor = nullptr, bool Debug = false, float VisibilityThreshold = 0.0f);
		
		static bool LineTraceActor(FVector Start, FVector End, AActor* TargetActor, AActor* IgnoredActor = nullptr, bool Debug = false);
		static bool LineTracePoint(FVector Start, FVector End, UVisibilityPointComponent* TargetPoint, AActor* IgnoredActor, bool Debug);
	
};
