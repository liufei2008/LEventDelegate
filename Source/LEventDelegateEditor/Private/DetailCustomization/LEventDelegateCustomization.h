// Copyright 2019-Present LexLiu. All Rights Reserved.
#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"
#include "LEventDelegate.h"
#include "PropertyHandle.h"
#pragma once


/**
 * 
 */
class FLEventDelegateCustomization : public IPropertyTypeCustomization
{
protected:
	TSharedPtr<IPropertyUtilities> PropertyUtilites;
	static TArray<FString> CopySourceData;
	TSharedPtr<SWidget> ColorPickerParentWidget;
	TArray<TSharedRef<SWidget>> EventParameterWidgetArray;
	TSharedPtr<SVerticalBox> EventsVerticalLayout;
	TArray<FLEventDelegate*> EventDelegateInstances;
private:
	bool CanChangeParameterType = true;
	bool IsParameterTypeValid(ELEventDelegateParameterType InParamType)
	{
		return InParamType != ELEventDelegateParameterType::None;
	}
	FSimpleDelegate OnEventArrayNumChangedDelegate;
	FText GetEventTitleName(TSharedRef<IPropertyHandle> PropertyHandle)const;
	FText GetEventItemFunctionName(TSharedRef<IPropertyHandle> EventItemPropertyHandle)const;
	UObject* GetEventItemTargetObject(TSharedRef<IPropertyHandle> EventItemPropertyHandle)const;
	AActor* GetEventItemHelperActor(TSharedRef<IPropertyHandle> EventItemPropertyHandle)const;
	FText GetComponentDisplayName(TSharedRef<IPropertyHandle> EventItemPropertyHandle)const;
	EVisibility GetNativeParameterWidgetVisibility(TSharedRef<IPropertyHandle> EventItemPropertyHandle, TSharedRef<IPropertyHandle> PropertyHandle)const;
	EVisibility GetDrawFunctionParameterWidgetVisibility(TSharedRef<IPropertyHandle> EventItemPropertyHandle, TSharedRef<IPropertyHandle> PropertyHandle)const;
	EVisibility GetNotValidParameterWidgetVisibility(TSharedRef<IPropertyHandle> EventItemPropertyHandle, TSharedRef<IPropertyHandle> PropertyHandle)const;
public:
	FLEventDelegateCustomization(bool InCanChangeParameterType)
	{
		CanChangeParameterType = InCanChangeParameterType;
	}
	~FLEventDelegateCustomization()
	{
		
	}
	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShareable(new FLEventDelegateCustomization(true));
	}
	/** IDetailCustomization interface */
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override {};

	ELEventDelegateParameterType GetNativeParameterType(TSharedRef<IPropertyHandle> PropertyHandle)const;
	void AddNativeParameterTypeProperty(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder);
	ELEventDelegateParameterType GetEventDataParameterType(TSharedRef<IPropertyHandle> EventDataItemHandle)const;

	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)override;

	void SetEventDataParameterType(TSharedRef<IPropertyHandle> EventDataItemHandle, ELEventDelegateParameterType ParameterType);
private:

	void UpdateEventsLayout(TSharedRef<IPropertyHandle> PropertyHandle);

	TSharedPtr<IPropertyHandleArray> GetEventListHandle(TSharedRef<IPropertyHandle> PropertyHandle)const;
	FOptionalSize GetEventItemHeight(int itemIndex)const
	{
		return EventParameterWidgetArray[itemIndex]->GetCachedGeometry().Size.Y + 60;//60 is other's size
	}
	FOptionalSize GetEventTotalHeight()const
	{
		float result = EventParameterWidgetArray.Num() > 0 ? 32 : 40;//32 & 40 is the header and tail size
		for (int i = 0; i < EventParameterWidgetArray.Num(); i++)
		{
			result += GetEventItemHeight(i).Get();
		}
		return result;
	}
	TSharedRef<SWidget> MakeComponentSelectorMenu(int32 itemIndex, TSharedRef<IPropertyHandle> PropertyHandle);
	TSharedRef<SWidget> MakeFunctionSelectorMenu(int32 itemIndex, TSharedRef<IPropertyHandle> PropertyHandle);
	void OnActorParameterChange(TSharedRef<IPropertyHandle> ItemPropertyHandle);
	void OnSelectComponent(UActorComponent* Comp, TSharedRef<IPropertyHandle> ItemPropertyHandle);
	void OnSelectActorSelf(TSharedRef<IPropertyHandle> ItemPropertyHandle);
	void OnSelectFunction(FName FuncName, ELEventDelegateParameterType ParamType, bool UseNativeParameter, TSharedRef<IPropertyHandle> ItemPropertyHandle);
	bool IsComponentSelectorMenuEnabled(TSharedRef<IPropertyHandle> ItemPropertyHandle)const;
	bool IsFunctionSelectorMenuEnabled(TSharedRef<IPropertyHandle> ItemPropertyHandle)const;
	void OnClickListAdd(TSharedRef<IPropertyHandle> PropertyHandle);
	void OnClickListEmpty(TSharedRef<IPropertyHandle> PropertyHandle);
	FReply OnClickAddRemove(bool AddOrRemove, int32 Index, int32 Count, TSharedRef<IPropertyHandle> PropertyHandle);
	FReply OnClickCopyPaste(bool CopyOrPaste, int32 Index, TSharedRef<IPropertyHandle> PropertyHandle);
	FReply OnClickDuplicate(int32 Index, TSharedRef<IPropertyHandle> PropertyHandle);
	FReply OnClickMoveUpDown(bool UpOrDown, int32 Index, TSharedRef<IPropertyHandle> PropertyHandle);

	TSharedRef<SWidget> DrawFunctionParameter(TSharedRef<IPropertyHandle> InDataContainerHandle, ELEventDelegateParameterType InFunctionParameterType, UFunction* InFunction);
	//function's parameter editor
	TSharedRef<SWidget> DrawFunctionReferenceParameter(TSharedRef<IPropertyHandle> InDataContainerHandle, ELEventDelegateParameterType FunctionParameterType, UFunction* InFunction);

	void ObjectValueChange(const FAssetData& InObj, TSharedPtr<IPropertyHandle> BufferHandle, TSharedPtr<IPropertyHandle> ObjectReferenceHandle, bool ObjectOrActor);
	const UClass* GetClassValue(TSharedPtr<IPropertyHandle> ClassReferenceHandle)const;
	void ClassValueChange(const UClass* InClass, TSharedPtr<IPropertyHandle> ClassReferenceHandle);
	void EnumValueChange(int32 InValue, ESelectInfo::Type SelectionType, TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle);
	void BoolValueChange(TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle);
	void FloatValueChange(TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle);
	void DoubleValueChange(TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle);
	void Int8ValueChange(TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle);
	void UInt8ValueChange(TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle);
	void Int16ValueChange(TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle);
	void UInt16ValueChange(TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle);
	void Int32ValueChange(TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle);
	void UInt32ValueChange(TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle);
	void Int64ValueChange(TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle);
	void UInt64ValueChange(TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle);
	void StringValueChange(TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle);
	void NameValueChange(TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle);
	void TextValueChange(TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle);
	void Vector2ItemValueChange(double NewValue, ETextCommit::Type CommitInfo, int AxisType, TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle);
	TOptional<double> Vector2GetItemValue(int AxisType, TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)const;
	void Vector3ItemValueChange(double NewValue, ETextCommit::Type CommitInfo, int AxisType, TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle);
	TOptional<double> Vector3GetItemValue(int AxisType, TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)const;
	void Vector4ItemValueChange(double NewValue, ETextCommit::Type CommitInfo, int AxisType, TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle);
	TOptional<double> Vector4GetItemValue(int AxisType, TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)const;
	FLinearColor LinearColorGetValue(bool bIsLinearColor, TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)const;
	void LinearColorValueChange(FLinearColor NewValue, bool bIsLinearColor, TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle);
	FReply OnMouseButtonDownColorBlock(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, bool bIsLinearColor, TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle);
	TOptional<double> RotatorGetItemValue(int AxisType, TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)const;
	void RotatorValueChange(double NewValue, ETextCommit::Type CommitInfo, int AxisType, TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle);
	void SetBufferValue(TSharedPtr<IPropertyHandle> BufferHandle, const TArray<uint8>& BufferArray);
	void SetBufferLength(TSharedPtr<IPropertyHandle> BufferHandle, int32 Count);
	TArray<uint8> GetBuffer(TSharedPtr<IPropertyHandle> BufferHandle);
	TArray<uint8> GetPropertyBuffer(TSharedPtr<IPropertyHandle> BufferHandle) const;
	int32 GetEnumValue(TSharedPtr<IPropertyHandle> ValueHandle)const;
	FText GetTextValue(TSharedPtr<IPropertyHandle> ValueHandle)const;
	void SetTextValue(const FText& InText, ETextCommit::Type InCommitType, TSharedPtr<IPropertyHandle> ValueHandle);
	void ClearValueBuffer(TSharedPtr<IPropertyHandle> PropertyHandle);
	void ClearReferenceValue(TSharedPtr<IPropertyHandle> PropertyHandle);
	void ClearObjectValue(TSharedPtr<IPropertyHandle> PropertyHandle);
	void OnParameterTypeChange(TSharedRef<IPropertyHandle> InDataContainerHandle);
	void CreateColorPicker(bool bIsLinearColor, TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle);
};

