// Copyright 2019, Institute for Artificial Intelligence - University of Bremen

#include "AvatarQnAController.h"
#include "Engine.h"

void UAvatarQnAController::Init(AActor* InAgent)
{
	if (!InAgent)
	{
		UE_LOG(LogTemp, Error, TEXT("Avatar QnA Controller not attached to an Avatar"));
	}

	Avatar = Cast<AIAIAvatarCharacter>(InAgent);
	check(Avatar != nullptr);	
}

UAvatarQnAController::UAvatarQnAController()
{
	bAnswering = false;
	CSVLine= "";
	FileName = "Preferences." + FDateTime::Now().ToString() + ".csv";
}

void UAvatarQnAController::Tick(float InDeltaTime)
{
	CancelAction();
	if (bActive)
	{
		GoalStatusList.Last().Status = 1;

		if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::RightMouseButton)) {
			if (!bAnswering) {
				
				GetWorld()->GetTimerManager().ClearTimer(TimeOutHandle);
				bAnswering = true;

				FTimerHandle CheckAnswerHandle;
				FTimerDelegate CheckAnswerDelegate;

				CheckAnswerDelegate = FTimerDelegate::CreateUObject(this, &UAvatarQnAController::CheckAnswer);
				Avatar->GetWorldTimerManager().SetTimer(CheckAnswerHandle, CheckAnswerDelegate, 5, false, 2.6);
			}
		}
	}
	if (bCancel) {
		bAnswering = false;
	}
}

void UAvatarQnAController::NotifyQuestion()
{
	
	FTimerDelegate TimeOutDelegate;

	TimeOutDelegate = FTimerDelegate::CreateUObject(this, &UAvatarQnAController::TimeOut);
	Avatar->GetWorldTimerManager().SetTimer(TimeOutHandle, TimeOutDelegate, 5, false, 5);

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Green, FString::Printf(TEXT("* %s"), *Question), true, FVector2D(4, 4));
	Avatar->enableDTW = true;
	bActive = true;
	if (bSaveCSV) {
		CSVLine = Avatar->GetName() + ",";
		CSVLine += Question + ",";
	}
}

void UAvatarQnAController::CheckAnswer()
{
	FString AnswerText;
	int8 decision;
	UDTWRecognizer* HeadMoveRecognizer = Avatar->FindComponentByClass<UDTWRecognizer>();
	decision = HeadMoveRecognizer->GetDecision();

	switch (decision)
	{
	case 0:
		AnswerText = "Yes";
		break;
	case 1:
		AnswerText = "No";
		break;
	case 2:
		AnswerText = "Thinking";
		break;
	default:
		AnswerText = "Unknown";
	}

	GiveAnswer(AnswerText);
}

void UAvatarQnAController::GiveAnswer(FString InAnswer)
{
	GoalStatusList.Last().Status = 3;
	Avatar->enableDTW = false;
	Avatar->ResetFollowCamera();
	bActive = false;
	bPublishResult = true;
	bAnswering = false;
	Answer = InAnswer;

	if (bSaveCSV) {
		CSVLine += Answer + "\n";

		FString FileString;
		FFileHelper::LoadFileToString(FileString, *(FPaths::ProjectDir() + FString("Datatables/") + FileName));
		FileString += CSVLine;
		FFileHelper::SaveStringToFile(FileString, *(FPaths::ProjectDir() + FString("Datatables/") + FileName));
	}
}

void UAvatarQnAController::TimeOut()
{
	GiveAnswer(TEXT("None"));
}