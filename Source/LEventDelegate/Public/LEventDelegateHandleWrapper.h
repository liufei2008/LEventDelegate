// Copyright 2019-Present LexLiu. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "LEventDelegateHandleWrapper.generated.h"

/**
 *Just a wrapper for blueprint to store a delegate handle
 */
USTRUCT(BlueprintType)
struct LEVENTDELEGATE_API FLEventDelegateHandleWrapper
{
	GENERATED_BODY()
public:
	FLEventDelegateHandleWrapper() {}
	FLEventDelegateHandleWrapper(FDelegateHandle InDelegateHandle)
	{
		DelegateHandle = InDelegateHandle;
	}
	FDelegateHandle DelegateHandle;
};
