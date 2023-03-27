// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include "TPSPlayer.h"
#include "Enemy.h"
#include <Kismet/GameplayStatics.h>
#include "TPSProject.h"
#include <Components/CapsuleComponent.h>
#include "EnemyAnim.h"

// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	// 타깃과 내 정보를 할당해준다.
	// 월드에서 ATPSPlayer 타깃 찾아오기
	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), ATPSPlayer::StaticClass());
	// ATPSPlayer 타입으로 캐스팅
	target = Cast<ATPSPlayer>(actor);
	// 내 정보 찾아오기
	me = Cast<AEnemy>(GetOwner());

	// UEnemyAnim* 할당
	anim = Cast<UEnemyAnim>(me->GetMesh()->GetAnimInstance());
}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (mState)
	{
	case EEnemyState::Idle:
		IdleState();
		break;
	case EEnemyState::Move:
		MoveState();
		break;
	case EEnemyState::Attack:
		AttackState();
		break;
	case EEnemyState::Damage:
		DamageState();
		break;
	case EEnemyState::Die:
		DieState();
		break;
	}
}

void UEnemyFSM::IdleState()
{
	currentTime += GetWorld()->DeltaTimeSeconds;
	// 일정 시간 기다렸다가 이동 상태로 전환
	if (currentTime > idleDelayTime)
	{
		mState = EEnemyState::Move;
		currentTime = 0;

		// 애니메이션 상태 동기화
		anim->animState = mState;
	}
}

void UEnemyFSM::MoveState()
{
	// 1. 타깃 목적지가 필요하다.
	FVector destination = target->GetActorLocation();
	// 2. 방향이 필요하다.
	// 타깃 - 나의 위치
	FVector dir = destination - me->GetActorLocation();
	// 3. 방향으로 이동하고 싶다.
	me->AddMovementInput(dir.GetSafeNormal());

	// 타깃과 가까워지면 공격 상태로 전환하고 싶다.
	// 만약 거리가 공격 범위 안에 들어오면
	if (dir.Size() < attackRange)
	{
		// 공격 상태로 전환
		mState = EEnemyState::Attack;
		// 애니메이션 상태 동기화
		anim->animState = mState;
		// 공격 애니메이션 재생 활성화
		anim->bAttackPlay = true;
		// 공격 상태 전환 시 대기 시간이 바로 끝나도록 처리
		currentTime = attackDelayTime;
	}
}

void UEnemyFSM::AttackState()
{
	// 일정 시간에 한 번씩 공격하고 싶다.
	// 1. 시간이 흘러야 한다.
	currentTime += GetWorld()->DeltaTimeSeconds;
	// 2. 공격 시간이 됐으니까
	if (currentTime > attackDelayTime)
	{
		// 3. 공격하고 싶다.
		PRINT_LOG(TEXT("Attack!!!"));
		// 경과 시간 초기화
		currentTime = 0;
		anim->bAttackPlay = true;
	}

	// 타깃이 공격 범위를 벗어나면 상태를 이동으로 전환하고 싶다.
	// 1. 타깃과의 거리가 필요하다.
	float distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());
	// 2. 타깃과의 거리가 공격 범위를 벗어나면
	if (distance > attackRange)
	{
		// 이동으로 전환
		mState = EEnemyState::Move;
		// 애니메이션 상태 동기화
		anim->animState = mState;
	}
}

void UEnemyFSM::DamageState()
{
	currentTime += GetWorld()->DeltaTimeSeconds;
	if (currentTime > damageDelayTime)
	{
		// 대기 상태로 전환
		mState = EEnemyState::Idle;
		currentTime = 0;
		// 애니메이션 상태 동기화
		anim->animState = mState;
	}
}

void UEnemyFSM::DieState()
{
	// 아직 죽음 애니메이션이 끝나지 않았다면 바닥 내려가지 않도록 처리
	if (!anim->bDieDone)
		return;

	// 계속 아래로 내려가고 싶다.
	// 등속운동으로
	FVector P0 = me->GetActorLocation();
	FVector vt = FVector::DownVector * dieSpeed * GetWorld()->DeltaTimeSeconds;
	FVector P = P0 + vt;
	me->SetActorLocation(P);

	// 만약 2미터 이상 내려왔다면
	if (P.Z < -200.0f)
	{
		// 제거시킨다.
		me->Destroy();
	}
}

// 피격 알림 이벤트 함수
void UEnemyFSM::OnDamageProcess()
{
	// 체력 감소
	hp--;
	// 체력이 남아있으면
	if (hp > 0)
	{
		// 상태를 피격으로 전환
		mState = EEnemyState::Damage;

		currentTime = 0;

		// 피격 애니메이션 재생
		int32 index = FMath::RandRange(0, 1);
		FString sectionName = FString::Printf(TEXT("Damage%d"), 0);
		anim->PlayDamageAnim(FName(*sectionName));
	}
	else
	{
		// 상태를 죽음으로 전환
		mState = EEnemyState::Die;
		// 캡슐 충돌체 비활성화
		me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// 죽음 애니메이션 재생
		anim->PlayDamageAnim(TEXT("Die"));
	}
	// 애니메이션 상태 동기화
	anim->animState = mState;
}

