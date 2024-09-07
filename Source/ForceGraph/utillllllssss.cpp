#include "utillllllssss.h"
#include "Engine/Engine.h"

// void ll(const FString& StringToLog)
// {
// 	if (!StringToLog.IsEmpty())
// 	{
// 		// Log to Unreal Engine console
// 		UE_LOG(LogTemp, Log, TEXT("%s"), *StringToLog);
//         
// 		// Optionally, display on screen
// 		if (GEngine)
// 		{
// 			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, StringToLog);
// 		}
// 	}
// }

void ll(const FString& StringToLog, int SeverityLevel, const FString& Prefix)
{
	if (!StringToLog.IsEmpty())
	{
		FString LogMessage = Prefix + StringToLog; // Prepends a prefix to the original message
		// ELogVerbosity::Type LogLevel1;

		// Map SeverityLevel to ELogVerbosity
		switch (SeverityLevel) 
		{
		case 1:
			
			UE_LOG(LogTemp, Warning, TEXT("%s"), *LogMessage);

			break;
		case 2:
			UE_LOG(LogTemp, Error, TEXT("%s"), *LogMessage);	
			break;
		default:
			UE_LOG(LogTemp, Log, TEXT("%s"), *LogMessage);
			break;
		}

		
	}
}

FVector Jiggle(const FVector& Vec, float Magnitude)
{
	FVector RandomJitter;
	RandomJitter.X = FMath::RandRange(-0.5f, 0.5f) * Magnitude;
	RandomJitter.Y = FMath::RandRange(-0.5f, 0.5f) * Magnitude;
	RandomJitter.Z = FMath::RandRange(-0.5f, 0.5f) * Magnitude;

	return Vec + RandomJitter;
}

