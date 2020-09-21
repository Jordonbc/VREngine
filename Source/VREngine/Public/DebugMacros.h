#pragma once
#include "Engine/Engine.h"

//Current Class Name + Function Name where this is called!
#define GAME_CUR_CLASS_FUNC (FString(__FUNCTION__))
//Current Class where this is called!
#define GAME_CUR_CLASS (FString(__FUNCTION__).Left(FString(__FUNCTION__).Find(TEXT(":"))) )
//Current Function Name where this is called!
#define GAME_CUR_FUNC (FString(__FUNCTION__).Right(FString(__FUNCTION__).Len() - FString(__FUNCTION__).Find(TEXT("::")) - 2 ))
//Current Line Number in the code where this is called!
#define GAME_CUR_LINE  (FString::FromInt(__LINE__))
//Current Class and Line Number where this is called!
#define GAME_CUR_CLASS_LINE (GAME_CUR_CLASS + "(" + GAME_CUR_LINE + ")")
//Current Function Signature where this is called!
#define GAME_CUR_FUNCSIG (FString(__FUNCSIG__))
// Define more colors if needed
#define COLOR_INFO        FColor::Green
#define COLOR_WARNING    FColor::Red

#define GAME_CUR_CLASS_FUNC_LINE (GAME_CUR_CLASS + GAME_CUR_CLASS_FUNC + "(" + GAME_CUR_LINE + ")")

//#if WITH_EDITOR
////     Gives you the Class name and exact line number where you print a message
//#define SW_LOG(Message)            (GEngine->AddOnScreenDebugMessage(-1, 5.f, COLOR_WARNING, *(GAME_CUR_CLASS_LINE + ": " + Message)) )
//#define SW_LOG2(Message,Message2)    (GEngine->AddOnScreenDebugMessage(-1, 5.f, COLOR_WARNING, *(GAME_CUR_CLASS_LINE + ": " + Message + " " + Message2)) )
//#define SW_LOGF(Message,Message2)    (GEngine->AddOnScreenDebugMessage(-1, 5.f, COLOR_WARNING, *(GAME_CUR_CLASS_LINE + ": " + Message + " " + FString::SanitizeFloat(Message2))) )
//#define SI_LOG(Message)            (GEngine->AddOnScreenDebugMessage(-1, 5.f, COLOR_INFO, *(GAME_CUR_CLASS_LINE + ": " + Message)) )
//#define SI_LOG2(Message,Message2)    (GEngine->AddOnScreenDebugMessage(-1, 5.f, COLOR_INFO, *(GAME_CUR_CLASS_LINE + ": " + Message + " " + Message2)) )
//#define SI_LOGF(Message,Message2)    (GEngine->AddOnScreenDebugMessage(-1, 5.f, COLOR_INFO, *(GAME_CUR_CLASS_LINE + ": " + Message + " " + FString::SanitizeFloat(Message2))) )
//#endif

#define LOG(LogCategory, Message)	UE_LOG(LogCategory, Log, TEXT("%s: %s"), *GAME_CUR_CLASS_FUNC_LINE, *FString(Message))