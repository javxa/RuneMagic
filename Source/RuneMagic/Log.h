#pragma once

#define DEBUG_TEXT(x) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::White, x)
#define WARNING_TEXT(x) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Yellow, x)
#define ERROR_TEXT(x) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, x)

#define DEBUG(x, ...) DEBUG_TEXT(FString::Printf(TEXT(x), ##__VA_ARGS__))
#define WARNING(x, ...) WARNING_TEXT(FString::Printf(TEXT(x), ##__VA_ARGS__))
#define ERROR(x, ...) ERROR_TEXT(FString::Printf(TEXT(x), ##__VA_ARGS__))
