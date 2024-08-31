#include "utillllllssss.h"
#include "Engine/Engine.h"

void ll(const FString& StringToLog)
{
	if (!StringToLog.IsEmpty())
	{
		// Log to Unreal Engine console
		UE_LOG(LogTemp, Log, TEXT("%s"), *StringToLog);
        
		// Optionally, display on screen
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, StringToLog);
		}
	}
}