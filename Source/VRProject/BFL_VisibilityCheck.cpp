// Fill out your copyright notice in the Description page of Project Settings.


#include "BFL_VisibilityCheck.h"

bool UBFL_VisibilityCheck::LineTraceTest(FVector Start, FVector End, AActor* TargetActor, AActor* IgnoredActor)
{

	UWorld* World = GEngine->GameViewport->GetWorld();
	FHitResult HitResult; 
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(IgnoredActor);
	bool bHit = World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
	DrawDebugLine(World, Start, HitResult.TraceEnd, bHit ? FColor::Red : FColor::Green, false, 0.0f, 0, 2.0f);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
		HitResult.GetActor() ? HitResult.GetActor()->GetName() : TEXT("No Actor"));

	return (HitResult.GetActor() == TargetActor);
		


}

bool UBFL_VisibilityCheck::IsVisibleToPlayer(UCameraComponent* camera, AActor* object, float CameraAngle, AActor* IgnoredActor)
{
	bool IsVisible = false;

	FVector CameraLocation = camera->GetComponentLocation();
	FVector CameraForward = camera->GetForwardVector();
	FVector ObjectLocation = object->GetActorLocation();
	FVector DirectionToObject = (ObjectLocation - CameraLocation).GetSafeNormal();

	// Check dot product to exclude from visibility check all actors outside of camera view
	float DotProduct = FVector::DotProduct( CameraForward, DirectionToObject );

	bool IsInFOV = FMath::Abs(DotProduct) > FMath::Cos(FMath::DegreesToRadians(CameraAngle));
	// If object is in FOV - test visibility with line traces
	if (IsInFOV)
	{
		FVector ActorCenter;
		FVector ActorBounds;

		object->GetActorBounds(true, ActorCenter, ActorBounds);
		UWorld* World = GEngine->GameViewport->GetWorld();
		DrawDebugBox(World, ActorCenter, ActorBounds, FColor::Green, false, 1.0f, 0, 2.0f);
		// Calculate the 8 corners of the bounding box
		FVector Corners[8];
		Corners[0] = ActorCenter + FVector(ActorBounds.X, ActorBounds.Y, ActorBounds.Z);
		Corners[1] = ActorCenter + FVector(ActorBounds.X, ActorBounds.Y, -ActorBounds.Z);
		Corners[2] = ActorCenter + FVector(ActorBounds.X, -ActorBounds.Y, ActorBounds.Z);
		Corners[3] = ActorCenter + FVector(ActorBounds.X, -ActorBounds.Y, -ActorBounds.Z);
		Corners[4] = ActorCenter + FVector(-ActorBounds.X, ActorBounds.Y, ActorBounds.Z);
		Corners[5] = ActorCenter + FVector(-ActorBounds.X, ActorBounds.Y, -ActorBounds.Z);
		Corners[6] = ActorCenter + FVector(-ActorBounds.X, -ActorBounds.Y, ActorBounds.Z);
		Corners[7] = ActorCenter + FVector(-ActorBounds.X, -ActorBounds.Y, -ActorBounds.Z);

		FVector Start = CameraLocation;

		//For each corner line trace until successfull hit
		for (int i = 0; i < 8; i++)
		{
			IsVisible = LineTraceTest(Start, Corners[i], object, IgnoredActor);
			if (IsVisible)
				return true;
		}
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,TEXT("______________"));
		
	}
	else
	{
		return false;
	}
	// Debug
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(acos(DotProduct)));
	//}

	return IsVisible;
}
