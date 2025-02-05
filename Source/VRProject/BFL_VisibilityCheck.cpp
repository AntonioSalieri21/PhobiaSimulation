// Fill out your copyright notice in the Description page of Project Settings.


#include "BFL_VisibilityCheck.h"

bool UBFL_VisibilityCheck::LineTraceActor(FVector Start, FVector End, AActor* TargetActor, AActor* IgnoredActor, bool Debug)
{

	UWorld* World = GEngine->GameViewport->GetWorld();
	FHitResult HitResult; 
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(IgnoredActor);
	bool bHit = World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);

	// Debug
    if (Debug)
    {
        DrawDebugLine(World, Start, HitResult.TraceEnd, bHit ? FColor::Red : FColor::Green, false, 0.5f);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
            HitResult.GetActor() ? HitResult.GetActor()->GetName() : TEXT("No Actor"));

        if (bHit)
        {
            DrawDebugBox(World, HitResult.ImpactPoint, FVector(5, 5, 5), FColor::Emerald, false, 2.0f);
        }
    }


	return (HitResult.GetActor() == TargetActor);

}

bool UBFL_VisibilityCheck::LineTracePoint(FVector Start, FVector End, UVisibilityPointComponent* TargetPoint, AActor* IgnoredActor, bool Debug)
{
    if (!TargetPoint) return false; 

    UWorld* World = GEngine->GameViewport->GetWorld();
    if (!World) return false;

    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(IgnoredActor);
    bool bHit = World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);

    // Debug visualization
    if (Debug)
    {
        DrawDebugLine(World, Start, HitResult.TraceEnd, bHit ? FColor::Red : FColor::Green, false, 0.5f);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
            HitResult.GetActor() ? HitResult.GetActor()->GetName() : TEXT("No Actor"));

        if (bHit)
        {
            DrawDebugBox(World, HitResult.ImpactPoint, FVector(5, 5, 5), FColor::Emerald, false, 2.0f);
        }
    }

    if (bHit && HitResult.GetComponent())
    {
        return HitResult.GetComponent()->GetClass() == TargetPoint->GetClass();
    }

    return false;
}


bool UBFL_VisibilityCheck::IsVisibleToPlayer(UCameraComponent* camera, FVisibilityActorStruct ActorData, float CameraAngle, AActor* IgnoredActor, bool Debug, float VisibilityThreshold)
{
    bool bIsVisible = false;
    
    AActor* object = ActorData.Actor;
    FVector CameraLocation = camera->GetComponentLocation();
    FVector CameraForward = camera->GetForwardVector();
    FVector ObjectLocation = object->GetActorLocation();
    FVector DirectionToObject = (ObjectLocation - CameraLocation).GetSafeNormal();

    // Quick dot product test to see if the object is generally in front of the camera
    float DotProduct = FVector::DotProduct(CameraForward, DirectionToObject);
    bool bIsInFOV = FMath::Abs(DotProduct) > FMath::Cos(FMath::DegreesToRadians(CameraAngle));

    if (!bIsInFOV)
    {
        return false;
    }
    
    // If Actor priority is 1 then perform simple visibility check by actor bounds
    if (ActorData.Priority == 1)
    {

        FVector ActorCenter = ActorData.BoxCenter;
        FVector ActorExtent = ActorData.BoxExtent;
        
        int32 Threshold = FMath::FloorToInt(21 * VisibilityThreshold);
        int32 VisiblePointsCnt = 0;

        if (Debug)
        {
            UWorld* World = GEngine->GameViewport->GetWorld();
            DrawDebugBox(World, ActorCenter, ActorExtent, FColor::Green, false, 1.0f, 0, 2.0f);
        }


        // Calculate the 8 corners of the axis-aligned bounding box
        FVector Corners[8];
        Corners[0] = ActorCenter + FVector(ActorExtent.X, ActorExtent.Y, ActorExtent.Z);
        Corners[1] = ActorCenter + FVector(ActorExtent.X, ActorExtent.Y, -ActorExtent.Z);
        Corners[2] = ActorCenter + FVector(ActorExtent.X, -ActorExtent.Y, ActorExtent.Z);
        Corners[3] = ActorCenter + FVector(ActorExtent.X, -ActorExtent.Y, -ActorExtent.Z);
        Corners[4] = ActorCenter + FVector(-ActorExtent.X, ActorExtent.Y, ActorExtent.Z);
        Corners[5] = ActorCenter + FVector(-ActorExtent.X, ActorExtent.Y, -ActorExtent.Z);
        Corners[6] = ActorCenter + FVector(-ActorExtent.X, -ActorExtent.Y, ActorExtent.Z);
        Corners[7] = ActorCenter + FVector(-ActorExtent.X, -ActorExtent.Y, -ActorExtent.Z);

        // Test the center of the actor
        if (LineTraceActor(CameraLocation, ActorCenter, object, IgnoredActor, Debug))
        {
            VisiblePointsCnt++;
            if (VisiblePointsCnt >= Threshold)
                return true;
        }

        // Test the corners 
        for (int i = 0; i < 8; i++)
        {
            if (LineTraceActor(CameraLocation, Corners[i], object, IgnoredActor, Debug))
            {
                VisiblePointsCnt++;
                if (VisiblePointsCnt >= Threshold)
                    return true;
            }
        }

        // Define the 12 edges of the cube using indices into the Corners array.
        // Each pair represents the start and end of an edge.
        TArray<TPair<int, int>> Edges;
        Edges.Add(TPair<int, int>(0, 1));
        Edges.Add(TPair<int, int>(0, 2));
        Edges.Add(TPair<int, int>(0, 4));
        Edges.Add(TPair<int, int>(1, 3));
        Edges.Add(TPair<int, int>(1, 5));
        Edges.Add(TPair<int, int>(2, 3));
        Edges.Add(TPair<int, int>(2, 6));
        Edges.Add(TPair<int, int>(3, 7));
        Edges.Add(TPair<int, int>(4, 5));
        Edges.Add(TPair<int, int>(4, 6));
        Edges.Add(TPair<int, int>(5, 7));
        Edges.Add(TPair<int, int>(6, 7));

        // For each edge, sample along the line.

        for (TPair<int, int>& Edge : Edges)
        {
            FVector EdgeStart = Corners[Edge.Key];
            FVector EdgeEnd = Corners[Edge.Value];
            FVector MidPoint = (EdgeStart + EdgeEnd) * 0.5f;

            // Option 1: Test just the midpoint
            if (LineTraceActor(CameraLocation, MidPoint, object, IgnoredActor, Debug))
            {
                VisiblePointsCnt++;
                if (VisiblePointsCnt >= Threshold)
                    return true;
            }
        }

        if (VisiblePointsCnt >= Threshold)
            return true;
    }
    // Else if its priority is 0 then perform visibility check by manualy placed visibility points
    else if (ActorData.Priority == 0)
    {
        int32 Threshold = FMath::FloorToInt( ActorData.VisibilityPoints.Num() * VisibilityThreshold );
        int32 VisiblePointsCnt = 0;

        // Line trace every visibility points
        for (UVisibilityPointComponent* point : ActorData.VisibilityPoints)
        {
            FVector End = point->GetComponentLocation();
            if (LineTracePoint(CameraLocation, End , point, IgnoredActor, Debug))
            {
                VisiblePointsCnt++;
                if (VisiblePointsCnt >= Threshold)
                    return true;
            }

        }


    }
    return false;
}

