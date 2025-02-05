// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyActor.generated.h"

UCLASS()
class PRACTICE_TPS_API AEnemyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// �ڽ� ������Ʈ
	UPROPERTY(EditAnywhere)
	class UBoxComponent* boxComp;

	// ����ƽ �޽� ������Ʈ
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* meshComp;

	// �÷��̾� ���� Ȯ��
	UPROPERTY(EditAnywhere)
	int32 traceRate = 50;

	UPROPERTY(EditAnywhere)
	float moveSpeed = 800;

	UFUNCTION()
	void OnEnemyOverlap(UPrimitiveComponent* OverlappedComponent,
						AActor* OtherActor,
						UPrimitiveComponent* OtherComp,
						int32 OtherBodyIndex,
						bool bFromSweep,
						const FHitResult& SweepResult);

private:
	FVector dir;
};
