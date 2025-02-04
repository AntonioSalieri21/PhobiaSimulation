// Fill out your copyright notice in the Description page of Project Settings.


#include "BFL_VisibilityCheck.h"

bool UBFL_VisibilityCheck::LineTraceTest(FVector Start, FVector End, AActor* TargetActor, AActor* IgnoredActor, bool Debug)
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
bool UBFL_VisibilityCheck::IsVisibleToPlayer(UCameraComponent* camera, AActor* object, float CameraAngle, AActor* IgnoredActor, bool Debug)
{
    bool bIsVisible = false;

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
    FVector ActorCenter;
    FVector ActorExtent;
    object->GetActorBounds(true, ActorCenter, ActorExtent);

    UWorld* World = GEngine->GameViewport->GetWorld();

    if (Debug)
    {
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
    if (LineTraceTest(CameraLocation, ActorCenter, object, IgnoredActor, Debug))
    {
        return true;
    }

    // Test the corners 
    for (int i = 0; i < 8; i++)
    {
        if (LineTraceTest(CameraLocation, Corners[i], object, IgnoredActor, Debug))
        {
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
        if (LineTraceTest(CameraLocation, MidPoint, object, IgnoredActor, Debug))
        {
            return true;
        }

        // Option 2: (Uncomment if you want more fine-grained sampling along the edge)
        /*
        const int NumSamples = 3; // includes start, midpoint, and end (or choose any number)
        for (int i = 1; i < NumSamples; i++)
        {
            float Alpha = (float)i / (float)(NumSamples);
            FVector SamplePoint = FMath::Lerp(EdgeStart, EdgeEnd, Alpha);
            if (LineTraceTest(CameraLocation, SamplePoint, object, IgnoredActor))
            {
                return true;
            }
        }
        */
    }
    return false;
}

