// Copyright 2019-Present LexLiu. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "LEventDelegate.h"
#include "LEventDelegateHandleWrapper.h"
#include "LEventDelegate_PresetParameter.generated.h"

#define MAKE_EVENTDELEGATE_PRESETPARAM_DELEGATE(EventDelegateParamType, ParamType)\
DECLARE_DELEGATE_OneParam(FLEventDelegate_##EventDelegateParamType##_Delegate, ParamType);\
DECLARE_MULTICAST_DELEGATE_OneParam(FLEventDelegate_##EventDelegateParamType##_MulticastDelegate, ParamType);


#define MAKE_EVENTDELEGATE_PRESETPARAM(EventDelegateParamType, ParamType)\
public:\
	FLEventDelegate_##EventDelegateParamType() :FLEventDelegate(ELEventDelegateParameterType::EventDelegateParamType) {}\
private:\
	mutable FLEventDelegate_##EventDelegateParamType##_MulticastDelegate eventDelegate;\
public:\
	FDelegateHandle Register(const TFunction<void(ParamType)>& function)const\
	{\
		return eventDelegate.AddLambda(function);\
	}\
	FDelegateHandle Register(const FLEventDelegate_##EventDelegateParamType##_Delegate& function)const\
	{\
		return eventDelegate.Add(function);\
	}\
	void Unregister(const FDelegateHandle& delegateHandle)const\
	{\
		eventDelegate.Remove(delegateHandle);\
	}\
	void operator() (ParamType InParam)const\
	{\
		FLEventDelegate::FireEvent(InParam);\
		if (eventDelegate.IsBound())eventDelegate.Broadcast(InParam);\
	}



DECLARE_DELEGATE(FLEventDelegate_Empty_Delegate); 
DECLARE_MULTICAST_DELEGATE(FLEventDelegate_Empty_MulticastDelegate);
DECLARE_DYNAMIC_DELEGATE(FLEventDelegate_Empty_DynamicDelegate);
USTRUCT(BlueprintType)
struct LEVENTDELEGATE_API FLEventDelegate_Empty : public FLEventDelegate
{
	GENERATED_BODY()
public:
	FLEventDelegate_Empty() :FLEventDelegate(ELEventDelegateParameterType::Empty) {}
private:
	mutable FLEventDelegate_Empty_MulticastDelegate eventDelegate;
public:
	FDelegateHandle Register(const TFunction<void()>& function)const
	{
		return eventDelegate.AddLambda(function);
	}
	FDelegateHandle Register(const FLEventDelegate_Empty_Delegate& function)const
	{
		return eventDelegate.Add(function);
	}
	void Unregister(const FDelegateHandle& delegateHandle)const
	{
		eventDelegate.Remove(delegateHandle);
	}
	void operator() ()const
	{
		FLEventDelegate::FireEvent();
		if (eventDelegate.IsBound())eventDelegate.Broadcast();
	}
};

MAKE_EVENTDELEGATE_PRESETPARAM_DELEGATE(Bool, bool);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLEventDelegate_Bool_DynamicDelegate, bool, value);
USTRUCT(BlueprintType)
struct LEVENTDELEGATE_API FLEventDelegate_Bool : public FLEventDelegate
{
	GENERATED_BODY()
MAKE_EVENTDELEGATE_PRESETPARAM(Bool, bool);
};

MAKE_EVENTDELEGATE_PRESETPARAM_DELEGATE(Float, float);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLEventDelegate_Float_DynamicDelegate, float, value);
USTRUCT(BlueprintType)
struct LEVENTDELEGATE_API FLEventDelegate_Float : public FLEventDelegate
{
	GENERATED_BODY()
		MAKE_EVENTDELEGATE_PRESETPARAM(Float, float);
};

MAKE_EVENTDELEGATE_PRESETPARAM_DELEGATE(Double, double);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLEventDelegate_Double_DynamicDelegate, double, value);
USTRUCT(BlueprintType)
struct LEVENTDELEGATE_API FLEventDelegate_Double : public FLEventDelegate
{
	GENERATED_BODY()
		MAKE_EVENTDELEGATE_PRESETPARAM(Double, double);
};

MAKE_EVENTDELEGATE_PRESETPARAM_DELEGATE(Int8, int8);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLEventDelegate_Int8_DynamicDelegate, int8, value);
USTRUCT(BlueprintType)
struct LEVENTDELEGATE_API FLEventDelegate_Int8 : public FLEventDelegate
{
	GENERATED_BODY()
		MAKE_EVENTDELEGATE_PRESETPARAM(Int8, int8);
};

MAKE_EVENTDELEGATE_PRESETPARAM_DELEGATE(UInt8, uint8);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLEventDelegate_UInt8_DynamicDelegate, uint8, value);
USTRUCT(BlueprintType)
struct LEVENTDELEGATE_API FLEventDelegate_UInt8 : public FLEventDelegate
{
	GENERATED_BODY()
		MAKE_EVENTDELEGATE_PRESETPARAM(UInt8, uint8);
};

MAKE_EVENTDELEGATE_PRESETPARAM_DELEGATE(Int16, int16);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLEventDelegate_Int16_DynamicDelegate, int16, value);
USTRUCT(BlueprintType)
struct LEVENTDELEGATE_API FLEventDelegate_Int16 : public FLEventDelegate
{
	GENERATED_BODY()
		MAKE_EVENTDELEGATE_PRESETPARAM(Int16, int16);
};

MAKE_EVENTDELEGATE_PRESETPARAM_DELEGATE(UInt16, uint16);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLEventDelegate_UInt16_DynamicDelegate, uint16, value);
USTRUCT(BlueprintType)
struct LEVENTDELEGATE_API FLEventDelegate_UInt16 : public FLEventDelegate
{
	GENERATED_BODY()
		MAKE_EVENTDELEGATE_PRESETPARAM(UInt16, uint16);
};

MAKE_EVENTDELEGATE_PRESETPARAM_DELEGATE(Int32, int32);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLEventDelegate_Int32_DynamicDelegate, int32, value);
USTRUCT(BlueprintType)
struct LEVENTDELEGATE_API FLEventDelegate_Int32 : public FLEventDelegate
{
	GENERATED_BODY()
		MAKE_EVENTDELEGATE_PRESETPARAM(Int32, int32);
};

MAKE_EVENTDELEGATE_PRESETPARAM_DELEGATE(UInt32, uint32);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLEventDelegate_UInt32_DynamicDelegate, uint32, value);
USTRUCT(BlueprintType)
struct LEVENTDELEGATE_API FLEventDelegate_UInt32 : public FLEventDelegate
{
	GENERATED_BODY()
		MAKE_EVENTDELEGATE_PRESETPARAM(UInt32, uint32);
};

MAKE_EVENTDELEGATE_PRESETPARAM_DELEGATE(Int64, int64);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLEventDelegate_Int64_DynamicDelegate, int64, value);
USTRUCT(BlueprintType)
struct LEVENTDELEGATE_API FLEventDelegate_Int64 : public FLEventDelegate
{
	GENERATED_BODY()
		MAKE_EVENTDELEGATE_PRESETPARAM(Int64, int64);
};

MAKE_EVENTDELEGATE_PRESETPARAM_DELEGATE(UInt64, uint64);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLEventDelegate_UInt64_DynamicDelegate, uint64, value);
USTRUCT(BlueprintType)
struct LEVENTDELEGATE_API FLEventDelegate_UInt64 : public FLEventDelegate
{
	GENERATED_BODY()
		MAKE_EVENTDELEGATE_PRESETPARAM(UInt64, uint64);
};

MAKE_EVENTDELEGATE_PRESETPARAM_DELEGATE(Vector2, FVector2D);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLEventDelegate_Vector2_DynamicDelegate, FVector2D, value);
USTRUCT(BlueprintType)
struct LEVENTDELEGATE_API FLEventDelegate_Vector2 : public FLEventDelegate
{
	GENERATED_BODY()
		MAKE_EVENTDELEGATE_PRESETPARAM(Vector2, FVector2D);
};

MAKE_EVENTDELEGATE_PRESETPARAM_DELEGATE(Vector3, FVector);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLEventDelegate_Vector3_DynamicDelegate, FVector, value);
USTRUCT(BlueprintType)
struct LEVENTDELEGATE_API FLEventDelegate_Vector3 : public FLEventDelegate
{
	GENERATED_BODY()
		MAKE_EVENTDELEGATE_PRESETPARAM(Vector3, FVector);
};

MAKE_EVENTDELEGATE_PRESETPARAM_DELEGATE(Vector4, FVector4);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLEventDelegate_Vector4_DynamicDelegate, FVector4, value);
USTRUCT(BlueprintType)
struct LEVENTDELEGATE_API FLEventDelegate_Vector4 : public FLEventDelegate
{
	GENERATED_BODY()
		MAKE_EVENTDELEGATE_PRESETPARAM(Vector4, FVector4);
};

MAKE_EVENTDELEGATE_PRESETPARAM_DELEGATE(Color, FColor);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLEventDelegate_Color_DynamicDelegate, FColor, value);
USTRUCT(BlueprintType)
struct LEVENTDELEGATE_API FLEventDelegate_Color : public FLEventDelegate
{
	GENERATED_BODY()
		MAKE_EVENTDELEGATE_PRESETPARAM(Color, FColor);
};

MAKE_EVENTDELEGATE_PRESETPARAM_DELEGATE(LinearColor, FLinearColor);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLEventDelegate_LinearColor_DynamicDelegate, FLinearColor, value);
USTRUCT(BlueprintType)
struct LEVENTDELEGATE_API FLEventDelegate_LinearColor : public FLEventDelegate
{
	GENERATED_BODY()
		MAKE_EVENTDELEGATE_PRESETPARAM(LinearColor, FLinearColor);
};

MAKE_EVENTDELEGATE_PRESETPARAM_DELEGATE(Quaternion, FQuat);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLEventDelegate_Quaternion_DynamicDelegate, FQuat, value);
USTRUCT(BlueprintType)
struct LEVENTDELEGATE_API FLEventDelegate_Quaternion : public FLEventDelegate
{
	GENERATED_BODY()
		MAKE_EVENTDELEGATE_PRESETPARAM(Quaternion, FQuat);
};

MAKE_EVENTDELEGATE_PRESETPARAM_DELEGATE(String, FString);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLEventDelegate_String_DynamicDelegate, FString, value);
USTRUCT(BlueprintType)
struct LEVENTDELEGATE_API FLEventDelegate_String : public FLEventDelegate
{
	GENERATED_BODY()
		MAKE_EVENTDELEGATE_PRESETPARAM(String, FString);
};

MAKE_EVENTDELEGATE_PRESETPARAM_DELEGATE(Object, UObject*);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLEventDelegate_Object_DynamicDelegate, UObject*, value);
USTRUCT(BlueprintType)
struct LEVENTDELEGATE_API FLEventDelegate_Object : public FLEventDelegate
{
	GENERATED_BODY()
		MAKE_EVENTDELEGATE_PRESETPARAM(Object, UObject*);
};

MAKE_EVENTDELEGATE_PRESETPARAM_DELEGATE(Actor, AActor*);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLEventDelegate_Actor_DynamicDelegate, AActor*, value);
USTRUCT(BlueprintType)
struct LEVENTDELEGATE_API FLEventDelegate_Actor : public FLEventDelegate
{
	GENERATED_BODY()
		MAKE_EVENTDELEGATE_PRESETPARAM(Actor, AActor*);
};

//MAKE_EVENTDELEGATE_PRESETPARAM_DELEGATE(PointerEvent, ULGUIPointerEventData*);
//DECLARE_DYNAMIC_DELEGATE_OneParam(FLEventDelegate_PointerEvent_DynamicDelegate, ULGUIPointerEventData*, value);
//USTRUCT(BlueprintType)
//struct LEVENTDELEGATE_API FLEventDelegate_PointerEvent : public FLEventDelegate
//{
//	GENERATED_BODY()
//		MAKE_EVENTDELEGATE_PRESETPARAM(PointerEvent, ULGUIPointerEventData*);
//};

MAKE_EVENTDELEGATE_PRESETPARAM_DELEGATE(Class, UClass*);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLEventDelegate_Class_DynamicDelegate, UClass*, value);
USTRUCT(BlueprintType)
struct LEVENTDELEGATE_API FLEventDelegate_Class : public FLEventDelegate
{
	GENERATED_BODY()
		MAKE_EVENTDELEGATE_PRESETPARAM(Class, UClass*);
};

MAKE_EVENTDELEGATE_PRESETPARAM_DELEGATE(Rotator, FRotator);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLEventDelegate_Rotator_DynamicDelegate, FRotator, value);
USTRUCT(BlueprintType)
struct LEVENTDELEGATE_API FLEventDelegate_Rotator : public FLEventDelegate
{
	GENERATED_BODY()
		MAKE_EVENTDELEGATE_PRESETPARAM(Rotator, FRotator);
};

MAKE_EVENTDELEGATE_PRESETPARAM_DELEGATE(Text, FText);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLEventDelegate_Text_DynamicDelegate, FText, value);
USTRUCT(BlueprintType)
struct LEVENTDELEGATE_API FLEventDelegate_Text : public FLEventDelegate
{
	GENERATED_BODY()
		MAKE_EVENTDELEGATE_PRESETPARAM(Text, FText);
};

MAKE_EVENTDELEGATE_PRESETPARAM_DELEGATE(Name, FName);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLEventDelegate_Name_DynamicDelegate, FName, value);
USTRUCT(BlueprintType)
struct LEVENTDELEGATE_API FLEventDelegate_Name : public FLEventDelegate
{
	GENERATED_BODY()
		MAKE_EVENTDELEGATE_PRESETPARAM(Name, FName);
};
