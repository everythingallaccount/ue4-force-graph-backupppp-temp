#pragma once

#include "CoreMinimal.h"

// Declare the function
// void ll(const FString& StringToLog);
void ll(const FString& StringToLog, int SeverityLevel = 0, const FString& Prefix = TEXT("[Info]"));
