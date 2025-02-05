// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include <Components/SphereComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 충돌체 인스턴스 생성
	collisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	// 충돌 프로파일 설정
	collisionComp->SetCollisionProfileName(TEXT("BlockAll"));
	// 충돌체 크기 설정
	collisionComp->SetSphereRadius(13);
	// 루트로 등록
	RootComponent = collisionComp;

	// 총알 외관 설정
	bodyMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMeshComp"));
	// 부모 컴포넌트 지정
	bodyMeshComp->SetupAttachment(collisionComp);
	// 충돌 비활성화
	bodyMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// 외관 크기 설정
	bodyMeshComp->SetRelativeScale3D(FVector(0.25f));

	// 발사체 컴포넌트 생성
	movementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	// movement 컴포넌트가 갱신시킬 컴포넌트 지정
	movementComp->SetUpdatedComponent(collisionComp);

	// 총알의 초기 속도 설정
	movementComp->InitialSpeed = 5000;
	// 총알의 최대 속도 설정
	movementComp->MaxSpeed = 5000;
	// 반동 생성
	movementComp->bShouldBounce = true;
	// 반동(탄성) 값
	movementComp->Bounciness = 0.3;

	// 총알 생명 주기
	// InitialLifeSpan = 2.0f;
	
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();

	// 총알 제거 타이머 설정
	FTimerHandle deathTimer;
	// GetWorld()->GetTimerManager().SetTimer(deathTimer, this, &ABullet::Die, 2.0f, false);
	GetWorld()->GetTimerManager().SetTimer(
		deathTimer, 
		FTimerDelegate::CreateLambda(
			[this]()->void { Destroy(); }
		), 
		2.0f, 
		false
	);
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABullet::Die()
{
	Destroy();
}

// 액터의 특정 속성을 수정하면 호출되는 이벤트 함수
void ABullet::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	// speed 값이 수정되었는지 체크
	if (PropertyChangedEvent.GetPropertyName() == TEXT("speed"))
	{
		// 프로젝타일 무브먼트 컴포넌트에 speed 값 적용
		movementComp->InitialSpeed = speed;
		movementComp->MaxSpeed = speed;
	}
}

