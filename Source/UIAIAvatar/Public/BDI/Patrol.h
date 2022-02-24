// Copyright 2017-2021, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "IAIAvatarCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "BDI/IAIAvatar_AIController.h"
#include "Patrol.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class UIAIAVATAR_API UPatrol : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:

	UPatrol(FObjectInitializer const& object_initializer);

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory);

private:
	union { // anonymous union
			AIAIAvatar_AIController* Controller;
			AIAIAvatarCharacter* Character;
	};
};
