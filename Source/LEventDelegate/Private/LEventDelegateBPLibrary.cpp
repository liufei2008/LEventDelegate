// Copyright 2019-Present LexLiu. All Rights Reserved.

#include "LEventDelegateBPLibrary.h"
#include "LEventDelegateModule.h"

#pragma region EventDelegate
#define IMPLEMENT_EVENTDELEGATE_BP(EventDelegateParamType, ParamType)\
FLEventDelegateHandleWrapper ULEventDelegateBPLibrary::LEventDelegate_##EventDelegateParamType##_Register(const FLEventDelegate_##EventDelegateParamType& InEvent, FLEventDelegate_##EventDelegateParamType##_DynamicDelegate InDelegate)\
{\
	auto delegateHandle = InEvent.Register([InDelegate](ParamType value) {\
		if (InDelegate.IsBound())\
		{\
			InDelegate.Execute(value);\
		}\
		});\
	return FLEventDelegateHandleWrapper(delegateHandle);\
}\
void ULEventDelegateBPLibrary::LEventDelegate_##EventDelegateParamType##_Unregister(const FLEventDelegate_##EventDelegateParamType& InEvent, const FLEventDelegateHandleWrapper& InDelegateHandle)\
{\
	InEvent.Unregister(InDelegateHandle.DelegateHandle);\
}

FLEventDelegateHandleWrapper ULEventDelegateBPLibrary::LEventDelegate_Empty_Register(const FLEventDelegate_Empty& InEvent, FLEventDelegate_Empty_DynamicDelegate InDelegate)
{
	auto delegateHandle = InEvent.Register([InDelegate]() {
		if (InDelegate.IsBound())
		{
			InDelegate.Execute();
		}
		});
	return FLEventDelegateHandleWrapper(delegateHandle);
}
void ULEventDelegateBPLibrary::LEventDelegate_Empty_Unregister(const FLEventDelegate_Empty& InEvent, const FLEventDelegateHandleWrapper& InDelegateHandle)
{
	InEvent.Unregister(InDelegateHandle.DelegateHandle);
}

IMPLEMENT_EVENTDELEGATE_BP(Bool, bool);
IMPLEMENT_EVENTDELEGATE_BP(Float, float);
IMPLEMENT_EVENTDELEGATE_BP(Double, double);
//IMPLEMENT_EVENTDELEGATE_BP(Int8, int8);
IMPLEMENT_EVENTDELEGATE_BP(UInt8, uint8);
//IMPLEMENT_EVENTDELEGATE_BP(Int16, int16);
//IMPLEMENT_EVENTDELEGATE_BP(UInt16, uint16);
IMPLEMENT_EVENTDELEGATE_BP(Int32, int32);
//IMPLEMENT_EVENTDELEGATE_BP(UInt32, uint32);
IMPLEMENT_EVENTDELEGATE_BP(Int64, int64);
//IMPLEMENT_EVENTDELEGATE_BP(UInt64, uint64);
IMPLEMENT_EVENTDELEGATE_BP(Vector2, FVector2D);
IMPLEMENT_EVENTDELEGATE_BP(Vector3, FVector);
IMPLEMENT_EVENTDELEGATE_BP(Vector4, FVector4);
IMPLEMENT_EVENTDELEGATE_BP(Color, FColor);
IMPLEMENT_EVENTDELEGATE_BP(LinearColor, FLinearColor);
IMPLEMENT_EVENTDELEGATE_BP(Quaternion, FQuat);
IMPLEMENT_EVENTDELEGATE_BP(String, FString);
IMPLEMENT_EVENTDELEGATE_BP(Object, UObject*);
IMPLEMENT_EVENTDELEGATE_BP(Actor, AActor*);
//IMPLEMENT_EVENTDELEGATE_BP(PointerEvent, ULGUIPointerEventData*);
IMPLEMENT_EVENTDELEGATE_BP(Class, UClass*);
IMPLEMENT_EVENTDELEGATE_BP(Rotator, FRotator);
IMPLEMENT_EVENTDELEGATE_BP(Text, FText);
IMPLEMENT_EVENTDELEGATE_BP(Name, FName);

#pragma endregion
