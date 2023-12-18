// Copyright 2019-Present LexLiu. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LEventDelegate.h"
#include "LEventDelegate_PresetParameter.h"
#include "LEventDelegateBPLibrary.generated.h"


UCLASS()
class LEVENTDELEGATE_API ULEventDelegateBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = LEventDelegate)static void LEventDelegateExecuteEmpty(const FLEventDelegate& InEvent) { InEvent.FireEvent(); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate)static void LEventDelegateExecuteBool(const FLEventDelegate& InEvent, const bool& InParameter) { InEvent.FireEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate)static void LEventDelegateExecuteFloat(const FLEventDelegate& InEvent, const float& InParameter) { InEvent.FireEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate)static void LEventDelegateExecuteDouble(const FLEventDelegate& InEvent, const double& InParameter) { InEvent.FireEvent(InParameter); }
	//UFUNCTION(BlueprintCallable, Category = LEventDelegate)static void LEventDelegateExecuteInt8(const FLEventDelegate& InEvent, const int8& InParameter) { InEvent.FireEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate)static void LEventDelegateExecuteUInt8(const FLEventDelegate& InEvent, const uint8& InParameter) { InEvent.FireEvent(InParameter); }
	//UFUNCTION(BlueprintCallable, Category = LEventDelegate)static void LEventDelegateExecuteInt16(const FLEventDelegate& InEvent, const int16& InParameter) { InEvent.FireEvent(InParameter); }
	//UFUNCTION(BlueprintCallable, Category = LEventDelegate)static void LEventDelegateExecuteUInt16(const FLEventDelegate& InEvent, const uint16& InParameter) { InEvent.FireEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate)static void LEventDelegateExecuteInt32(const FLEventDelegate& InEvent, const int32& InParameter) { InEvent.FireEvent(InParameter); }
	//UFUNCTION(BlueprintCallable, Category = LEventDelegate)static void LEventDelegateExecuteUInt32(const FLEventDelegate& InEvent, const uint32& InParameter) { InEvent.FireEvent(InParameter); }
	//UFUNCTION(BlueprintCallable, Category = LEventDelegate)static void LEventDelegateExecuteInt64(const FLEventDelegate& InEvent, const int64& InParameter) { InEvent.FireEvent(InParameter); }
	//UFUNCTION(BlueprintCallable, Category = LEventDelegate)static void LEventDelegateExecuteUInt64(const FLEventDelegate& InEvent, const uint64& InParameter) { InEvent.FireEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate)static void LEventDelegateExecuteVector2(const FLEventDelegate& InEvent, const FVector2D& InParameter) { InEvent.FireEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate)static void LEventDelegateExecuteVector3(const FLEventDelegate& InEvent, const FVector& InParameter) { InEvent.FireEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate)static void LEventDelegateExecuteVector4(const FLEventDelegate& InEvent, const FVector4& InParameter) { InEvent.FireEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate)static void LEventDelegateExecuteColor(const FLEventDelegate& InEvent, const FColor& InParameter) { InEvent.FireEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate)static void LEventDelegateExecuteLinearColor(const FLEventDelegate& InEvent, const FLinearColor& InParameter) { InEvent.FireEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate)static void LEventDelegateExecuteQuaternion(const FLEventDelegate& InEvent, const FQuat& InParameter) { InEvent.FireEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate)static void LEventDelegateExecuteString(const FLEventDelegate& InEvent, const FString& InParameter) { InEvent.FireEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate)static void LEventDelegateExecuteObject(const FLEventDelegate& InEvent, UObject* InParameter) { InEvent.FireEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate)static void LEventDelegateExecuteActor(const FLEventDelegate& InEvent, AActor* InParameter) { InEvent.FireEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate)static void LEventDelegateExecuteClass(const FLEventDelegate& InEvent, UClass* InParameter) { InEvent.FireEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate)static void LEventDelegateExecuteRotator(const FLEventDelegate& InEvent, const FRotator& InParameter) { InEvent.FireEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate)static void LEventDelegateExecuteText(const FLEventDelegate& InEvent, const FText& InParameter) { InEvent.FireEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate)static void LEventDelegateExecuteName(const FLEventDelegate& InEvent, const FName& InParameter) { InEvent.FireEvent(InParameter); }


	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Execute"))
		static void LEventDelegate_Empty_Execute(const FLEventDelegate_Empty& InEvent) { InEvent(); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Register"))
		static FLEventDelegateHandleWrapper LEventDelegate_Empty_Register(const FLEventDelegate_Empty& InEvent, FLEventDelegate_Empty_DynamicDelegate InDelegate);
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Unregister"))
		static void LEventDelegate_Empty_Unregister(const FLEventDelegate_Empty& InEvent, const FLEventDelegateHandleWrapper& InDelegateHandle);

	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Execute"))
		static void LEventDelegate_Bool_Execute(const FLEventDelegate_Bool& InEvent, bool InParameter) { InEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Register"))
		static FLEventDelegateHandleWrapper LEventDelegate_Bool_Register(const FLEventDelegate_Bool& InEvent, FLEventDelegate_Bool_DynamicDelegate InDelegate);
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Unregister"))
		static void LEventDelegate_Bool_Unregister(const FLEventDelegate_Bool& InEvent, const FLEventDelegateHandleWrapper& InDelegateHandle);

	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Execute"))
		static void LEventDelegate_Float_Execute(const FLEventDelegate_Float& InEvent, float InParameter) { InEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Register"))
		static FLEventDelegateHandleWrapper LEventDelegate_Float_Register(const FLEventDelegate_Float& InEvent, FLEventDelegate_Float_DynamicDelegate InDelegate);
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Unregister"))
		static void LEventDelegate_Float_Unregister(const FLEventDelegate_Float& InEvent, const FLEventDelegateHandleWrapper& InDelegateHandle);

	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Execute"))
		static void LEventDelegate_Double_Execute(const FLEventDelegate_Double& InEvent, double InParameter) { InEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Register"))
		static FLEventDelegateHandleWrapper LEventDelegate_Double_Register(const FLEventDelegate_Double& InEvent, FLEventDelegate_Double_DynamicDelegate InDelegate);
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Unregister"))
		static void LEventDelegate_Double_Unregister(const FLEventDelegate_Double& InEvent, const FLEventDelegateHandleWrapper& InDelegateHandle);

	//UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Execute"))
	//	static void LEventDelegate_Int8_Execute(const FLEventDelegate_Int8& InEvent, int8 InParameter) { InEvent(InParameter); }
	//UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Register"))
	//	static FLEventDelegateHandleWrapper LEventDelegate_Int8_Register(const FLEventDelegate_Int8& InEvent, FLEventDelegate_Int8_DynamicDelegate InDelegate);
	//UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Unregister"))
	//	static void LEventDelegate_Int8_Unregister(const FLEventDelegate_Int8& InEvent, const FLEventDelegateHandleWrapper& InDelegateHandle);

	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Execute"))
		static void LEventDelegate_UInt8_Execute(const FLEventDelegate_UInt8& InEvent, uint8 InParameter) { InEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Register"))
		static FLEventDelegateHandleWrapper LEventDelegate_UInt8_Register(const FLEventDelegate_UInt8& InEvent, FLEventDelegate_UInt8_DynamicDelegate InDelegate);
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Unregister"))
		static void LEventDelegate_UInt8_Unregister(const FLEventDelegate_UInt8& InEvent, const FLEventDelegateHandleWrapper& InDelegateHandle);

	//UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Execute"))
	//	static void LEventDelegate_Int16_Execute(const FLEventDelegate_Int16& InEvent, int16 InParameter) { InEvent(InParameter); }
	//UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Register"))
	//	static FLEventDelegateHandleWrapper LEventDelegate_Int16_Register(const FLEventDelegate_Int16& InEvent, FLEventDelegate_Int16_DynamicDelegate InDelegate);
	//UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Unregister"))
	//	static void LEventDelegate_Int16_Unregister(const FLEventDelegate_Int16& InEvent, const FLEventDelegateHandleWrapper& InDelegateHandle);

	//UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Execute"))
	//	static void LEventDelegate_UInt16_Execute(const FLEventDelegate_UInt16& InEvent, uint16 InParameter) { InEvent(InParameter); }
	//UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Register"))
	//	static FLEventDelegateHandleWrapper LEventDelegate_UInt16_Register(const FLEventDelegate_UInt16& InEvent, FLEventDelegate_UInt16_DynamicDelegate InDelegate);
	//UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Unregister"))
	//	static void LEventDelegate_UInt16_Unregister(const FLEventDelegate_UInt16& InEvent, const FLEventDelegateHandleWrapper& InDelegateHandle);

	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Execute"))
		static void LEventDelegate_Int32_Execute(const FLEventDelegate_Int32& InEvent, int32 InParameter) { InEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Register"))
		static FLEventDelegateHandleWrapper LEventDelegate_Int32_Register(const FLEventDelegate_Int32& InEvent, FLEventDelegate_Int32_DynamicDelegate InDelegate);
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Unregister"))
		static void LEventDelegate_Int32_Unregister(const FLEventDelegate_Int32& InEvent, const FLEventDelegateHandleWrapper& InDelegateHandle);

	//UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Execute"))
	//	static void LEventDelegate_UInt32_Execute(const FLEventDelegate_UInt32& InEvent, uint32 InParameter) { InEvent(InParameter); }
	//UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Register"))
	//	static FLEventDelegateHandleWrapper LEventDelegate_UInt32_Register(const FLEventDelegate_UInt32& InEvent, FLEventDelegate_UInt32_DynamicDelegate InDelegate);
	//UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Unregister"))
	//	static void LEventDelegate_UInt32_Unregister(const FLEventDelegate_UInt32& InEvent, const FLEventDelegateHandleWrapper& InDelegateHandle);

	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Execute"))
		static void LEventDelegate_Int64_Execute(const FLEventDelegate_Int64& InEvent, int64 InParameter) { InEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Register"))
		static FLEventDelegateHandleWrapper LEventDelegate_Int64_Register(const FLEventDelegate_Int64& InEvent, FLEventDelegate_Int64_DynamicDelegate InDelegate);
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Unregister"))
		static void LEventDelegate_Int64_Unregister(const FLEventDelegate_Int64& InEvent, const FLEventDelegateHandleWrapper& InDelegateHandle);

	//UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Execute"))
	//	static void LEventDelegate_UInt64_Execute(const FLEventDelegate_UInt64& InEvent, uint64 InParameter) { InEvent(InParameter); }
	//UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Register"))
	//	static FLEventDelegateHandleWrapper LEventDelegate_UInt64_Register(const FLEventDelegate_UInt64& InEvent, FLEventDelegate_UInt64_DynamicDelegate InDelegate);
	//UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Unregister"))
	//	static void LEventDelegate_UInt64_Unregister(const FLEventDelegate_UInt64& InEvent, const FLEventDelegateHandleWrapper& InDelegateHandle);

	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Execute"))
		static void LEventDelegate_Vector2_Execute(const FLEventDelegate_Vector2& InEvent, FVector2D InParameter) { InEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Register"))
		static FLEventDelegateHandleWrapper LEventDelegate_Vector2_Register(const FLEventDelegate_Vector2& InEvent, FLEventDelegate_Vector2_DynamicDelegate InDelegate);
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Unregister"))
		static void LEventDelegate_Vector2_Unregister(const FLEventDelegate_Vector2& InEvent, const FLEventDelegateHandleWrapper& InDelegateHandle);

	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Execute"))
		static void LEventDelegate_Vector3_Execute(const FLEventDelegate_Vector3& InEvent, FVector InParameter) { InEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Register"))
		static FLEventDelegateHandleWrapper LEventDelegate_Vector3_Register(const FLEventDelegate_Vector3& InEvent, FLEventDelegate_Vector3_DynamicDelegate InDelegate);
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Unregister"))
		static void LEventDelegate_Vector3_Unregister(const FLEventDelegate_Vector3& InEvent, const FLEventDelegateHandleWrapper& InDelegateHandle);

	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Execute"))
		static void LEventDelegate_Vector4_Execute(const FLEventDelegate_Vector4& InEvent, FVector4 InParameter) { InEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Register"))
		static FLEventDelegateHandleWrapper LEventDelegate_Vector4_Register(const FLEventDelegate_Vector4& InEvent, FLEventDelegate_Vector4_DynamicDelegate InDelegate);
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Unregister"))
		static void LEventDelegate_Vector4_Unregister(const FLEventDelegate_Vector4& InEvent, const FLEventDelegateHandleWrapper& InDelegateHandle);

	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Execute"))
		static void LEventDelegate_Color_Execute(const FLEventDelegate_Color& InEvent, FColor InParameter) { InEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Register"))
		static FLEventDelegateHandleWrapper LEventDelegate_Color_Register(const FLEventDelegate_Color& InEvent, FLEventDelegate_Color_DynamicDelegate InDelegate);
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Unregister"))
		static void LEventDelegate_Color_Unregister(const FLEventDelegate_Color& InEvent, const FLEventDelegateHandleWrapper& InDelegateHandle);

	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Execute"))
		static void LEventDelegate_LinearColor_Execute(const FLEventDelegate_LinearColor& InEvent, FLinearColor InParameter) { InEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Register"))
		static FLEventDelegateHandleWrapper LEventDelegate_LinearColor_Register(const FLEventDelegate_LinearColor& InEvent, FLEventDelegate_LinearColor_DynamicDelegate InDelegate);
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Unregister"))
		static void LEventDelegate_LinearColor_Unregister(const FLEventDelegate_LinearColor& InEvent, const FLEventDelegateHandleWrapper& InDelegateHandle);

	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Execute"))
		static void LEventDelegate_Quaternion_Execute(const FLEventDelegate_Quaternion& InEvent, FQuat InParameter) { InEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Register"))
		static FLEventDelegateHandleWrapper LEventDelegate_Quaternion_Register(const FLEventDelegate_Quaternion& InEvent, FLEventDelegate_Quaternion_DynamicDelegate InDelegate);
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Unregister"))
		static void LEventDelegate_Quaternion_Unregister(const FLEventDelegate_Quaternion& InEvent, const FLEventDelegateHandleWrapper& InDelegateHandle);

	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Execute"))
		static void LEventDelegate_String_Execute(const FLEventDelegate_String& InEvent, FString InParameter) { InEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Register"))
		static FLEventDelegateHandleWrapper LEventDelegate_String_Register(const FLEventDelegate_String& InEvent, FLEventDelegate_String_DynamicDelegate InDelegate);
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Unregister"))
		static void LEventDelegate_String_Unregister(const FLEventDelegate_String& InEvent, const FLEventDelegateHandleWrapper& InDelegateHandle);

	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Execute"))
		static void LEventDelegate_Object_Execute(const FLEventDelegate_Object& InEvent, UObject* InParameter) { InEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Register"))
		static FLEventDelegateHandleWrapper LEventDelegate_Object_Register(const FLEventDelegate_Object& InEvent, FLEventDelegate_Object_DynamicDelegate InDelegate);
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Unregister"))
		static void LEventDelegate_Object_Unregister(const FLEventDelegate_Object& InEvent, const FLEventDelegateHandleWrapper& InDelegateHandle);

	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Execute"))
		static void LEventDelegate_Actor_Execute(const FLEventDelegate_Actor& InEvent, AActor* InParameter) { InEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Register"))
		static FLEventDelegateHandleWrapper LEventDelegate_Actor_Register(const FLEventDelegate_Actor& InEvent, FLEventDelegate_Actor_DynamicDelegate InDelegate);
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Unregister"))
		static void LEventDelegate_Actor_Unregister(const FLEventDelegate_Actor& InEvent, const FLEventDelegateHandleWrapper& InDelegateHandle);

	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Execute"))
		static void LEventDelegate_Class_Execute(const FLEventDelegate_Class& InEvent, UClass* InParameter) { InEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Register"))
		static FLEventDelegateHandleWrapper LEventDelegate_Class_Register(const FLEventDelegate_Class& InEvent, FLEventDelegate_Class_DynamicDelegate InDelegate);
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Unregister"))
		static void LEventDelegate_Class_Unregister(const FLEventDelegate_Class& InEvent, const FLEventDelegateHandleWrapper& InDelegateHandle);

	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Execute"))
		static void LEventDelegate_Rotator_Execute(const FLEventDelegate_Rotator& InEvent, FRotator InParameter) { InEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Register"))
		static FLEventDelegateHandleWrapper LEventDelegate_Rotator_Register(const FLEventDelegate_Rotator& InEvent, FLEventDelegate_Rotator_DynamicDelegate InDelegate);
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Unregister"))
		static void LEventDelegate_Rotator_Unregister(const FLEventDelegate_Rotator& InEvent, const FLEventDelegateHandleWrapper& InDelegateHandle);

	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Execute"))
		static void LEventDelegate_Text_Execute(const FLEventDelegate_Text& InEvent, FText InParameter) { InEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Register"))
		static FLEventDelegateHandleWrapper LEventDelegate_Text_Register(const FLEventDelegate_Text& InEvent, FLEventDelegate_Text_DynamicDelegate InDelegate);
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Unregister"))
		static void LEventDelegate_Text_Unregister(const FLEventDelegate_Text& InEvent, const FLEventDelegateHandleWrapper& InDelegateHandle);

	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Execute"))
		static void LEventDelegate_Name_Execute(const FLEventDelegate_Name& InEvent, FName InParameter) { InEvent(InParameter); }
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Register"))
		static FLEventDelegateHandleWrapper LEventDelegate_Name_Register(const FLEventDelegate_Name& InEvent, FLEventDelegate_Name_DynamicDelegate InDelegate);
	UFUNCTION(BlueprintCallable, Category = LEventDelegate, meta = (DisplayName = "Unregister"))
		static void LEventDelegate_Name_Unregister(const FLEventDelegate_Name& InEvent, const FLEventDelegateHandleWrapper& InDelegateHandle);

};
