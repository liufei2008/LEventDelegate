// Copyright 2019-Present LexLiu. All Rights Reserved.

#include "DetailCustomization/LEventDelegateCustomization.h"
#include "LEventDelegateEditorModule.h"
#include "LEventDelegateEditorStyle.h"
#include "IDetailChildrenBuilder.h"
#include "IDetailPropertyRow.h"
#include "IDetailGroup.h"
#include "DetailLayoutBuilder.h"
#include "IPropertyUtilities.h"
#include "IPropertyTypeCustomization.h"
#include "PropertyCustomizationHelpers.h"
#include "Widget/LGUIVectorInputBox.h"
#include "Widgets/Input/SRotatorInputBox.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Colors/SColorPicker.h"
#include "Widgets/SWidget.h"
#include "Math/UnitConversion.h"
#include "STextPropertyEditableTextBox.h"
#include "SEnumCombo.h"
#include "Serialization/BufferArchive.h"
#include "LGUIEditableTextPropertyHandle.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"

#define LOCTEXT_NAMESPACE "LEventDelegateCustomization"

PRAGMA_DISABLE_OPTIMIZATION

#define LEventActorSelfName "(ActorSelf)"

TArray<FString> FLEventDelegateCustomization::CopySourceData;

TSharedPtr<IPropertyHandleArray> FLEventDelegateCustomization::GetEventListHandle(TSharedRef<IPropertyHandle> PropertyHandle)const
{
	return PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegate, eventList))->AsArray();
}

void FLEventDelegateCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	PropertyUtilites = CustomizationUtils.GetPropertyUtilities();

	//add parameter type property
	bool bIsInWorld = false;
	TArray<UObject*> NodeSet;
	PropertyHandle->GetOuterObjects(NodeSet);
	if (NodeSet.Num() > 1)
	{
		auto TipText = LOCTEXT("NotSupportMultipleEdit_Content", "(Not support multiple edit)");
		ChildBuilder.AddCustomRow(LOCTEXT("NotSupportMultipleEdit_Row", "NotSupportMultipleEdit"))
		.NameContent()
		[
			SNew(STextBlock)
			.Text(this, &FLEventDelegateCustomization::GetEventTitleName, PropertyHandle)
			.ToolTipText(PropertyHandle->GetToolTipText())
		]
		.ValueContent()
		[
			SNew(STextBlock)
			.Text(TipText)
			.ToolTipText(TipText)
			.Font(IDetailLayoutBuilder::GetDetailFont())
			.ColorAndOpacity(FLinearColor(FColor::Red))
			.AutoWrapText(true)
		]
		;
		return;
	}
	auto OutObject = NodeSet[0];
	bIsInWorld = OutObject->GetWorld() != nullptr;
	if (!bIsInWorld)
	{
		if (CanChangeParameterType)
		{
			AddNativeParameterTypeProperty(PropertyHandle, ChildBuilder);
		}
		return;
	}
	if (auto Component = Cast<UActorComponent>(OutObject))
	{
		if (Component->GetOwner() && Component->GetOwner()->BlueprintCreatedComponents.Contains(Component))
		{
			auto TipText = LOCTEXT("NotSupportActorBlueprintComponent_Content", "(Not support ActorBlueprint's component)");
			ChildBuilder.AddCustomRow(LOCTEXT("NotSupportActorBlueprintComponent_Row", "NotSupportActorBlueprintComponent"))
			.NameContent()
			[
				SNew(STextBlock)
				.Text(this, &FLEventDelegateCustomization::GetEventTitleName, PropertyHandle)
				.ToolTipText(PropertyHandle->GetToolTipText())
			]
			.ValueContent()
			[
				SNew(STextBlock)
				.Text(TipText)
				.ToolTipText(TipText)
				.Font(IDetailLayoutBuilder::GetDetailFont())
				.ColorAndOpacity(FLinearColor(FColor::Red))
				.AutoWrapText(true)
			]
			;
			return;
		}
	}

	// copy all EventDelegate I'm accessing right now
	TArray<void*> StructPtrs;
	PropertyHandle->AccessRawData(StructPtrs);
	check(StructPtrs.Num() != 0);

	EventDelegateInstances.AddZeroed(StructPtrs.Num());
	for (auto Iter = StructPtrs.CreateIterator(); Iter; ++Iter)
	{
		check(*Iter);
		auto Item = (FLEventDelegate*)(*Iter);
		EventDelegateInstances[Iter.GetIndex()] = Item;
		for (auto& listItem : Item->eventList)
		{
			listItem.CheckTargetObject();
		}
	}

	auto EventListHandle = GetEventListHandle(PropertyHandle);
	TWeakPtr<IPropertyUtilities> WeakUtilities = PropertyUtilites;
	OnEventArrayNumChangedDelegate = FSimpleDelegate::CreateLambda([WeakUtilities]() {
		if (WeakUtilities.IsValid())
		{
			WeakUtilities.Pin()->ForceRefresh();
		}
	});
	EventListHandle->SetOnNumElementsChanged(OnEventArrayNumChangedDelegate);
	UpdateEventsLayout(PropertyHandle);
	ChildBuilder.AddCustomRow(LOCTEXT("EventDelegate", "EventDelegate"))
		.WholeRowContent()
		[
			SNew(SBox)
			.Padding(FMargin(-10, 0, -2, 0))
			[
				SNew(SOverlay)
				+ SOverlay::Slot()
				[
					SNew(SBox)
					.HAlign(EHorizontalAlignment::HAlign_Center)
					.VAlign(EVerticalAlignment::VAlign_Center)
					[
						SNew(SBox)
						.WidthOverride(1000)
						.HeightOverride(this, &FLEventDelegateCustomization::GetEventTotalHeight)
						[
							SNew(SImage)
							.Image(FLEventDelegateEditorStyle::Get().GetBrush("LEventDelegateEditor.EventGroup"))
							.ColorAndOpacity(FLinearColor(FColor(255, 255, 255, 255)))
						]
					]
				]
				+ SOverlay::Slot()
				[
					EventsVerticalLayout.ToSharedRef()
				]
			]
		]
	;

}

FText FLEventDelegateCustomization::GetEventTitleName(TSharedRef<IPropertyHandle> PropertyHandle)const
{
	auto EventParameterType = GetNativeParameterType(PropertyHandle);
	auto NameStr = PropertyHandle->GetPropertyDisplayName().ToString();
	FString ParamTypeString = ULEventDelegateParameterHelper::ParameterTypeToName(EventParameterType, nullptr);
	NameStr = NameStr + "(" + ParamTypeString + ")";
	return FText::FromString(NameStr);
}

FText FLEventDelegateCustomization::GetEventItemFunctionName(TSharedRef<IPropertyHandle> EventItemPropertyHandle)const
{
	//function
	auto FunctionNameHandle = EventItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, functionName));
	FName FunctionFName;
	FunctionNameHandle->GetValue(FunctionFName);
	FString FunctionName = FunctionFName.ToString();
	//parameterType
	auto FunctionParameterType = GetEventDataParameterType(EventItemPropertyHandle);

	bool ComponentValid = false;//event target component valid?
	bool EventFunctionValid = false;//event target function valid?
	UFunction* EventFunction = nullptr;

	if (auto TargetObject = GetEventItemTargetObject(EventItemPropertyHandle))
	{
		EventFunction = TargetObject->FindFunction(FunctionFName);
		if (EventFunction)
		{
			if (ULEventDelegateParameterHelper::IsStillSupported(EventFunction, FunctionParameterType))
			{
				EventFunctionValid = true;
			}
		}
	}

	if (!EventFunctionValid)//function not valid, show tip
	{
		if (FunctionName != "None Function" && !FunctionName.IsEmpty())
		{
			FString Prefix = "(NotValid)";
			FunctionName = Prefix.Append(FunctionName);
		}
	}
	if (FunctionName.IsEmpty())FunctionName = "None Function";
	return FText::FromString(FunctionName);
}

UObject* FLEventDelegateCustomization::GetEventItemTargetObject(TSharedRef<IPropertyHandle> EventItemPropertyHandle)const
{
	//TargetObject
	auto TargetObjectHandle = EventItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, TargetObject));
	UObject* TargetObject = nullptr;
	TargetObjectHandle->GetValue(TargetObject);
	return TargetObject;
}

FText FLEventDelegateCustomization::GetComponentDisplayName(TSharedRef<IPropertyHandle> EventItemPropertyHandle)const
{
	FString ComponentDisplayName;
	auto TargetObject = GetEventItemTargetObject(EventItemPropertyHandle);
	auto HelperActor = GetEventItemHelperActor(EventItemPropertyHandle);
	if (TargetObject)
	{
		if (TargetObject == HelperActor)
		{
			ComponentDisplayName = LEventActorSelfName;
		}
		else
		{
			if (auto Comp = Cast<UActorComponent>(TargetObject))
			{
				ComponentDisplayName = TargetObject->GetName();
			}
			else
			{
				ComponentDisplayName = "(WrongType)";
			}
		}
	}
	else
	{
		ComponentDisplayName = "None";
	}
	return FText::FromString(ComponentDisplayName);
}

EVisibility FLEventDelegateCustomization::GetNativeParameterWidgetVisibility(TSharedRef<IPropertyHandle> EventItemPropertyHandle, TSharedRef<IPropertyHandle> PropertyHandle)const
{
	auto TargetObject = GetEventItemTargetObject(EventItemPropertyHandle);
	//function
	auto FunctionNameHandle = EventItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, functionName));
	FName FunctionFName;
	FunctionNameHandle->GetValue(FunctionFName);
	//parameterType
	auto FunctionParameterType = GetEventDataParameterType(EventItemPropertyHandle);
	UFunction* EventFunction = nullptr;
	auto EventParameterType = GetNativeParameterType(PropertyHandle);

	if (TargetObject)
	{
		EventFunction = TargetObject->FindFunction(FunctionFName);
	}

	if (IsValid(TargetObject) && IsValid(EventFunction))
	{
		bool bUseNativeParameter = false;
		auto UseNativeParameterHandle = EventItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, UseNativeParameter));
		UseNativeParameterHandle->GetValue(bUseNativeParameter);

		if ((EventParameterType == FunctionParameterType) && bUseNativeParameter)//support native parameter
		{
			return EVisibility::Visible;
		}
	}
	return EVisibility::Collapsed;
}

EVisibility FLEventDelegateCustomization::GetDrawFunctionParameterWidgetVisibility(TSharedRef<IPropertyHandle> EventItemPropertyHandle, TSharedRef<IPropertyHandle> PropertyHandle)const
{
	auto TargetObject = GetEventItemTargetObject(EventItemPropertyHandle);
	//function
	auto FunctionNameHandle = EventItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, functionName));
	FName FunctionFName;
	FunctionNameHandle->GetValue(FunctionFName);
	//parameterType
	auto FunctionParameterType = GetEventDataParameterType(EventItemPropertyHandle);
	UFunction* EventFunction = nullptr;
	auto EventParameterType = GetNativeParameterType(PropertyHandle);

	if (TargetObject)
	{
		EventFunction = TargetObject->FindFunction(FunctionFName);
	}

	if (IsValid(TargetObject) && IsValid(EventFunction))
	{
		bool bUseNativeParameter = false;
		auto UseNativeParameterHandle = EventItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, UseNativeParameter));
		UseNativeParameterHandle->GetValue(bUseNativeParameter);

		if ((EventParameterType == FunctionParameterType) && bUseNativeParameter)//support native parameter
		{

		}
		else
		{
			return EVisibility::Visible;
		}
	}
	return EVisibility::Collapsed;
}

EVisibility FLEventDelegateCustomization::GetNotValidParameterWidgetVisibility(TSharedRef<IPropertyHandle> EventItemPropertyHandle, TSharedRef<IPropertyHandle> PropertyHandle)const
{
	auto TargetObject = GetEventItemTargetObject(EventItemPropertyHandle);
	//function
	auto FunctionNameHandle = EventItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, functionName));
	FName FunctionFName;
	FunctionNameHandle->GetValue(FunctionFName);
	//parameterType
	auto FunctionParameterType = GetEventDataParameterType(EventItemPropertyHandle);
	UFunction* EventFunction = nullptr;
	auto EventParameterType = GetNativeParameterType(PropertyHandle);

	if (TargetObject)
	{
		EventFunction = TargetObject->FindFunction(FunctionFName);
	}

	if (IsValid(TargetObject) && IsValid(EventFunction))
	{
		return EVisibility::Collapsed;
	}
	else
	{
		return EVisibility::Visible;
	}
}

AActor* FLEventDelegateCustomization::GetEventItemHelperActor(TSharedRef<IPropertyHandle> EventItemPropertyHandle)const
{
	//HelperActor
	auto HelperActorHandle = EventItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, HelperActor));
	AActor* HelperActor = nullptr;
	UObject* HelperActorObject = nullptr;
	HelperActorHandle->GetValue(HelperActorObject);
	if (HelperActorObject != nullptr)
	{
		HelperActor = Cast<AActor>(HelperActorObject);
	}
	return HelperActor;
}

void FLEventDelegateCustomization::UpdateEventsLayout(TSharedRef<IPropertyHandle> PropertyHandle)
{
	auto EventParameterType = GetNativeParameterType(PropertyHandle);
	auto EventListHandle = GetEventListHandle(PropertyHandle);

	auto NativeParameterTypeHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegate, supportParameterType));
	NativeParameterTypeHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateLambda([=] { PropertyUtilites->ForceRefresh(); }));

	SAssignNew(EventsVerticalLayout, SVerticalBox)
	+ SVerticalBox::Slot()
	.AutoHeight()
	[
		SNew(SBox)
		.Padding(FMargin(8, 0))
		.HeightOverride(30)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Left)
			.VAlign(EVerticalAlignment::VAlign_Center)
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(this, &FLEventDelegateCustomization::GetEventTitleName, PropertyHandle)
				.ToolTipText(PropertyHandle->GetToolTipText())
				//.Font(IDetailLayoutBuilder::GetDetailFont())
			]
			+SHorizontalBox::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Right)
			[
				IsParameterTypeValid(EventParameterType) ?
				(
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Center)
					.Padding(2, 0)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						[
							PropertyCustomizationHelpers::MakeAddButton(FSimpleDelegate::CreateSP(this, &FLEventDelegateCustomization::OnClickListAdd, PropertyHandle))
						]
						+ SHorizontalBox::Slot()
						[
							PropertyCustomizationHelpers::MakeEmptyButton(FSimpleDelegate::CreateSP(this, &FLEventDelegateCustomization::OnClickListEmpty, PropertyHandle))
						]
					]
				)
				:
				(
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Center)
					.Padding(2, 0)
					[
						SNew(STextBlock)
						.AutoWrapText(true)
						.ColorAndOpacity(FSlateColor(FLinearColor::Red))
						.Text(LOCTEXT("ParameterTypeWrong", "Parameter type is wrong!"))
						.Font(IDetailLayoutBuilder::GetDetailFont())
					]
				)
			]
		]
	];

	EventParameterWidgetArray.Empty(); 
	uint32 arrayCount;
	EventListHandle->GetNumElements(arrayCount);
	for (int32 EventItemIndex = 0; EventItemIndex < (int32)arrayCount; EventItemIndex++)
	{
		auto ItemPropertyHandle = EventListHandle->GetElement(EventItemIndex);
		//HelperActor
		auto HelperActorHandle = ItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, HelperActor));
		UObject* HelperActorObject = nullptr;
		HelperActorHandle->GetValue(HelperActorObject);
		AActor* HelperActor = Cast<AActor>(HelperActorObject);

		//TargetObject
		auto TargetObjectHandle = ItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, TargetObject));
		UObject* TargetObject = nullptr;
		TargetObjectHandle->GetValue(TargetObject);

		UObject* ClassObject = nullptr;
		auto HelperClassHandle = ItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, HelperClass));
		HelperClassHandle->GetValue(ClassObject);
		if (ClassObject != nullptr)
		{
			UClass* ClassValue = Cast<UClass>(ClassObject);
			if (ClassValue == AActor::StaticClass())
			{
				TargetObjectHandle->SetValue(HelperActor);
			}
			else if (ClassValue->IsChildOf(UActorComponent::StaticClass()))
			{
				if (HelperActor != nullptr)
				{
					UActorComponent* FoundHelperComp = nullptr;
					TArray<UActorComponent*> CompArray;
					HelperActor->GetComponents(ClassValue, CompArray);
					if (CompArray.Num() == 1)
					{
						FoundHelperComp = CompArray[0];
					}
					else if (CompArray.Num() > 1)
					{
						FName HelperComponentName = NAME_None;
						auto HelperComponentNameHandle = ItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, HelperComponentName));
						HelperComponentNameHandle->GetValue(HelperComponentName);
						if (!HelperComponentName.IsNone())
						{
							for (auto& Comp : CompArray)
							{
								if (Comp->GetFName() == HelperComponentName)
								{
									FoundHelperComp = Comp;
									break;
								}
							}
						}
					}
					if (FoundHelperComp != TargetObject)
					{
						TargetObjectHandle->SetValue(FoundHelperComp);
					}
				}
				else
				{
					if (TargetObject != nullptr)
					{
						TargetObjectHandle->SetValue((UObject*)nullptr);
					}
				}
			}
		}
		else
		{
			if (TargetObject != nullptr)
			{
				TargetObjectHandle->SetValue((UObject*)nullptr);
			}
		}

		HelperActorHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FLEventDelegateCustomization::OnActorParameterChange, ItemPropertyHandle));
			
		//function
		auto FunctionNameHandle = ItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, functionName));
		FName FunctionFName;
		FunctionNameHandle->GetValue(FunctionFName);
		//parameterType
		auto paramTypeHandle = ItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, ParamType));
		paramTypeHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FLEventDelegateCustomization::OnParameterTypeChange, ItemPropertyHandle));
		auto FunctionParameterType = GetEventDataParameterType(ItemPropertyHandle);

		UFunction* EventFunction = nullptr;
		if (TargetObject)
		{
			EventFunction = TargetObject->FindFunction(FunctionFName);
		}

		TSharedRef<SWidget> ParameterWidget = SNew(SBox);
		if (IsValid(TargetObject) && IsValid(EventFunction))
		{
			bool bUseNativeParameter = false;
			auto UseNativeParameterHandle = ItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, UseNativeParameter));
			UseNativeParameterHandle->GetValue(bUseNativeParameter);
				
			if (EventParameterType != FunctionParameterType)//check "bUseNativeParameter" parameter
			{
				if (bUseNativeParameter)
				{
					bUseNativeParameter = false;
					UseNativeParameterHandle->SetValue(bUseNativeParameter);
				}
			}
			if ((EventParameterType == FunctionParameterType) && bUseNativeParameter)//support native parameter
			{
				//clear buffer and value
				ClearValueBuffer(ItemPropertyHandle);
				ClearReferenceValue(ItemPropertyHandle);
				//native parameter AnchorData
				ParameterWidget =
					SNew(SBox)
					.VAlign(EVerticalAlignment::VAlign_Center)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("(NativeParameter)", "(NativeParameter)"))
						.Font(IDetailLayoutBuilder::GetDetailFont())
					]
					;
			}
			else
			{
				ParameterWidget = DrawFunctionParameter(ItemPropertyHandle, FunctionParameterType, EventFunction);
			}
		}
		else
		{
			ParameterWidget = 
				SNew(SBox)
				.VAlign(EVerticalAlignment::VAlign_Center)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("(NotValid)", "(NotValid)"))
					.Font(IDetailLayoutBuilder::GetDetailFont())
				]
				;
		}
		ParameterWidget->SetToolTipText(LOCTEXT("Parameter", "Set parameter for the function of this event"));
		EventParameterWidgetArray.Add(ParameterWidget);

		//additional button
		int additionalButtonHeight = 20;
		auto additionalButtons = 
		SNew(SBox)
		[
			//copy, paste, add, delete
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SVerticalBox)
					+SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBox)
						.HeightOverride(additionalButtonHeight)
						[
							SNew(SButton)
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.Text(LOCTEXT("C", "C"))
							.OnClicked(this, &FLEventDelegateCustomization::OnClickCopyPaste, true, EventItemIndex, PropertyHandle)
							.ToolTipText(LOCTEXT("Copy", "Copy this function"))
						]
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBox)
						.HeightOverride(additionalButtonHeight)
						[
							SNew(SButton)
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.Text(LOCTEXT("P", "P"))
							.OnClicked(this, &FLEventDelegateCustomization::OnClickCopyPaste, false, EventItemIndex, PropertyHandle)
							.ToolTipText(LOCTEXT("Paste", "Paste copied function to this function"))
						]
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBox)
						.HeightOverride(additionalButtonHeight)
						[
							SNew(SButton)
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.Text(LOCTEXT("D", "D"))
							.OnClicked(this, &FLEventDelegateCustomization::OnClickDuplicate, EventItemIndex, PropertyHandle)
							.ToolTipText(LOCTEXT("Duplicate", "Duplicate this function"))
						]
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBox)
						.HeightOverride(additionalButtonHeight)
						[
							SNew(SButton)
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.Text(LOCTEXT("+", "+"))
							.OnClicked(this, &FLEventDelegateCustomization::OnClickAddRemove, true, EventItemIndex, (int32)arrayCount, PropertyHandle)
							.ToolTipText(LOCTEXT("Add", "Add new one"))
						]
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBox)
						.HeightOverride(additionalButtonHeight)
						[
							SNew(SButton)
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.Text(LOCTEXT("-", "-"))
							.OnClicked(this, &FLEventDelegateCustomization::OnClickAddRemove, false, EventItemIndex, (int32)arrayCount, PropertyHandle)
							.ToolTipText(LOCTEXT("Delete", "Delete this one"))
						]
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBox)
						.HeightOverride(additionalButtonHeight)
						[
							SNew(SButton)
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.Text(LOCTEXT("▲", "▲"))
							.OnClicked(this, &FLEventDelegateCustomization::OnClickMoveUpDown, true, EventItemIndex, PropertyHandle)
							.ToolTipText(LOCTEXT("MoveUp", "Move up"))
						]
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBox)
						.HeightOverride(additionalButtonHeight)
						[
							SNew(SButton)
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.Text(LOCTEXT("▼", "▼"))
							.OnClicked(this, &FLEventDelegateCustomization::OnClickMoveUpDown, false, EventItemIndex, PropertyHandle)
							.ToolTipText(LOCTEXT("MoveDown", "Move down"))
						]
					]
				]
			]
		];


		EventsVerticalLayout->AddSlot()
			.AutoHeight()
			[
				SNew(SBox)
				.Padding(FMargin(2, 0))
				[
					SNew(SOverlay)
					+ SOverlay::Slot()
					[
						SNew(SVerticalBox)
						+SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(EHorizontalAlignment::HAlign_Center)
						.VAlign(EVerticalAlignment::VAlign_Center)
						[
							SNew(SBox)
							.WidthOverride(1000)
							.HeightOverride(this, &FLEventDelegateCustomization::GetEventItemHeight, EventItemIndex)
							[
								SNew(SImage)
								.Image(FLEventDelegateEditorStyle::Get().GetBrush("LEventDelegateEditor.EventItem"))
								.ColorAndOpacity(FLinearColor(FColor(255, 255, 255, 255)))
							]
						]
					]
					+ SOverlay::Slot()
					[
						SNew(SBox)
						.Padding(FMargin(4, 4))
						[
							SNew(SVerticalBox)
							+SVerticalBox::Slot()
							.AutoHeight()
							[
								SNew(SBox)
								.Padding(FMargin(0, 0, 0, 2))
								[
									SNew(SHorizontalBox)
									+SHorizontalBox::Slot()
									[
										//HelperActor
										SNew(SBox)
										.Padding(FMargin(0, 0, 6, 0))
										[
											HelperActorHandle->CreatePropertyValueWidget()
										]
									]
									+SHorizontalBox::Slot()
									[
										SNew(SBox)
										.HeightOverride(26)
										[
											//Component
											SNew(SComboButton)
											.HasDownArrow(true)
											.IsEnabled(this, &FLEventDelegateCustomization::IsComponentSelectorMenuEnabled, ItemPropertyHandle)
											.ToolTipText(LOCTEXT("Component", "Pick component for this event"))
											.ButtonContent()
											[
												SNew(STextBlock)
												.Text(this, &FLEventDelegateCustomization::GetComponentDisplayName, ItemPropertyHandle)
												.Font(IDetailLayoutBuilder::GetDetailFont())
											]
											.MenuContent()
											[
												MakeComponentSelectorMenu(EventItemIndex, PropertyHandle)
											]
										]
									]
								]
							]
							+SVerticalBox::Slot()
							.AutoHeight()
							[
								SNew(SBox)
								.Padding(FMargin(0, 0, 0, 2))
								[
									SNew(SHorizontalBox)
									+SHorizontalBox::Slot()
									[
										SNew(SBox)
										.Padding(FMargin(0, 0, 6, 0))
										[
											SNew(SBox)
											.HeightOverride(26)
											[
												//function
												SNew(SComboButton)
												.HasDownArrow(true)
												.IsEnabled(this, &FLEventDelegateCustomization::IsFunctionSelectorMenuEnabled, ItemPropertyHandle)
												.ToolTipText(LOCTEXT("Function", "Pick a function to execute of this event"))
												.ButtonContent()
												[
													SNew(STextBlock)
													.Text(this, &FLEventDelegateCustomization::GetEventItemFunctionName, ItemPropertyHandle)
													.Font(IDetailLayoutBuilder::GetDetailFont())
												]
												.MenuContent()
												[
													MakeFunctionSelectorMenu(EventItemIndex, PropertyHandle)
												]
											]
										]
									]
									+SHorizontalBox::Slot()
									[
										//parameter
										ParameterWidget
									]
								]
							]
							+SVerticalBox::Slot()
							[
								additionalButtons
							]
						]
					]
				]
			]
		;
	}
}

void FLEventDelegateCustomization::OnActorParameterChange(TSharedRef<IPropertyHandle> ItemPropertyHandle)
{
	UObject* HelperActorObject = nullptr;
	auto HelperActorHandle = ItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, HelperActor));
	HelperActorHandle->GetValue(HelperActorObject);
	AActor* HelperActor = Cast<AActor>(HelperActorObject);

	auto TargetObjectHandle = ItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, TargetObject));
	UObject* TargetObject = nullptr;
	TargetObjectHandle->GetValue(TargetObject);

	UObject* ClassObject = nullptr;
	auto HelperClassHandle = ItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, HelperClass));
	HelperClassHandle->GetValue(ClassObject);
	if (ClassObject != nullptr)
	{
		UClass* ClassValue = Cast<UClass>(ClassObject);
		if (ClassValue == AActor::StaticClass())
		{
			TargetObjectHandle->SetValue(HelperActor);
		}
		else if (ClassValue->IsChildOf(UActorComponent::StaticClass()))
		{
			if (HelperActor != nullptr)
			{
				UActorComponent* FoundHelperComp = nullptr;
				TArray<UActorComponent*> CompArray;
				HelperActor->GetComponents(ClassValue, CompArray);
				if (CompArray.Num() == 1)
				{
					FoundHelperComp = CompArray[0];
				}
				else if (CompArray.Num() > 1)
				{
					FName HelperComponentName = NAME_None;
					auto HelperComponentNameHandle = ItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, HelperComponentName));
					HelperComponentNameHandle->GetValue(HelperComponentName);
					if (!HelperComponentName.IsNone())
					{
						for (auto& Comp : CompArray)
						{
							if (Comp->GetFName() == HelperComponentName)
							{
								FoundHelperComp = Comp;
								break;
							}
						}
					}
				}
				if (FoundHelperComp != TargetObject)
				{
					TargetObjectHandle->SetValue(FoundHelperComp);
				}
			}
			else
			{
				if (TargetObject != nullptr)
				{
					TargetObjectHandle->SetValue((UObject*)nullptr);
				}
			}
		}
	}
	else
	{
		if (TargetObject != nullptr)
		{
			TargetObjectHandle->SetValue((UObject*)nullptr);
		}
	}

	PropertyUtilites->ForceRefresh();
}

void FLEventDelegateCustomization::OnSelectComponent(UActorComponent* Comp, TSharedRef<IPropertyHandle> ItemPropertyHandle)
{
	auto TargetObjectHandle = ItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, TargetObject));
	TargetObjectHandle->SetValue(Comp);

	auto HelperClassHandle = ItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, HelperClass));
	HelperClassHandle->SetValue(Comp->GetClass());

	auto HelperComponentNameHandle = ItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, HelperComponentName));
	HelperComponentNameHandle->SetValue(Comp->GetFName());

	PropertyUtilites->ForceRefresh();
}
void FLEventDelegateCustomization::OnSelectActorSelf(TSharedRef<IPropertyHandle> ItemPropertyHandle)
{
	auto HelperActorHandle = ItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, HelperActor));
	UObject* HelperActorObject = nullptr;
	HelperActorHandle->GetValue(HelperActorObject);

	auto TargetObjectHandle = ItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, TargetObject));
	TargetObjectHandle->SetValue(HelperActorObject);

	auto HelperClassHandle = ItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, HelperClass));
	HelperClassHandle->SetValue(AActor::StaticClass());

	auto HelperComponentNameHandle = ItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, HelperComponentName));
	HelperComponentNameHandle->SetValue(NAME_None);

	PropertyUtilites->ForceRefresh();
}
void FLEventDelegateCustomization::OnSelectFunction(FName FuncName, ELEventDelegateParameterType ParamType, bool UseNativeParameter, TSharedRef<IPropertyHandle> ItemPropertyHandle)
{
	auto nameHandle = ItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, functionName));
	nameHandle->SetValue(FuncName);
	SetEventDataParameterType(ItemPropertyHandle, ParamType);
	auto UseNativeParameterHandle = ItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, UseNativeParameter));
	UseNativeParameterHandle->SetValue(UseNativeParameter);
	PropertyUtilites->ForceRefresh();
}

void FLEventDelegateCustomization::SetEventDataParameterType(TSharedRef<IPropertyHandle> EventDataItemHandle, ELEventDelegateParameterType ParameterType)
{
	auto ParamTypeHandle = EventDataItemHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, ParamType));
	ParamTypeHandle->SetValue((uint8)ParameterType);
}
ELEventDelegateParameterType FLEventDelegateCustomization::GetNativeParameterType(TSharedRef<IPropertyHandle> PropertyHandle)const
{
	auto NativeParameterTypeHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegate, supportParameterType));
	uint8 supportParameterTypeUint8;
	NativeParameterTypeHandle->GetValue(supportParameterTypeUint8);
	ELEventDelegateParameterType eventParameterType = (ELEventDelegateParameterType)supportParameterTypeUint8;
	return eventParameterType;
}
void FLEventDelegateCustomization::AddNativeParameterTypeProperty(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder)
{
	auto NativeParameterTypeHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegate, supportParameterType));
	ChildBuilder.AddProperty(NativeParameterTypeHandle.ToSharedRef());
}
ELEventDelegateParameterType FLEventDelegateCustomization::GetEventDataParameterType(TSharedRef<IPropertyHandle> EventDataItemHandle)const
{
	auto paramTypeHandle = EventDataItemHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, ParamType));
	uint8 functionParameterTypeUint8;
	paramTypeHandle->GetValue(functionParameterTypeUint8);
	ELEventDelegateParameterType functionParameterType = (ELEventDelegateParameterType)functionParameterTypeUint8;
	return functionParameterType;
}

TSharedRef<SWidget> FLEventDelegateCustomization::MakeComponentSelectorMenu(int32 itemIndex, TSharedRef<IPropertyHandle> PropertyHandle)
{
	auto EventListHandle = GetEventListHandle(PropertyHandle);
	auto ItemPropertyHandle = EventListHandle->GetElement(itemIndex);
	auto HelperActorHandle = ItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, HelperActor));
	UObject* HelperActorObject = nullptr;
	HelperActorHandle->GetValue(HelperActorObject);
	if (HelperActorObject == nullptr)
	{
		return SNew(SBox);
	}

	FMenuBuilder MenuBuilder(true, MakeShareable(new FUICommandList));

	auto HelperActor = (AActor*)HelperActorObject;
	MenuBuilder.AddMenuEntry(
		FUIAction(FExecuteAction::CreateRaw(this, &FLEventDelegateCustomization::OnSelectActorSelf, ItemPropertyHandle)),
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		[
			SNew(STextBlock)
			.Text(FText::FromString(LEventActorSelfName))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		]
		//+SHorizontalBox::Slot()
		//.HAlign(EHorizontalAlignment::HAlign_Right)
		//[
		//	SNew(STextBlock)
		//	.Text(FText::FromString(HelperActor->GetClass()->GetName()))
		//	.Font(IDetailLayoutBuilder::GetDetailFont())
		//]
	);
	auto Components = HelperActor->GetComponents();
	for (auto Comp : Components)
	{
		if(Comp->HasAnyFlags(EObjectFlags::RF_Transient))continue;
		auto CompName = Comp->GetFName();
		auto CompTypeName = Comp->GetClass()->GetName();
		MenuBuilder.AddMenuEntry(
			FUIAction(FExecuteAction::CreateRaw(this, &FLEventDelegateCustomization::OnSelectComponent, Comp, ItemPropertyHandle)),
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Left)
			[
				SNew(STextBlock)
				.Text(FText::FromString(CompName.ToString()))
				.Font(IDetailLayoutBuilder::GetDetailFont())
			]
			//+ SHorizontalBox::Slot()
			//.HAlign(EHorizontalAlignment::HAlign_Right)
			//[
			//	SNew(STextBlock)
			//	.Text(FText::FromString(CompTypeName))
			//	.Font(IDetailLayoutBuilder::GetDetailFont())
			//]
		);
	}
	return MenuBuilder.MakeWidget();
}
TSharedRef<SWidget> FLEventDelegateCustomization::MakeFunctionSelectorMenu(int32 itemIndex, TSharedRef<IPropertyHandle> PropertyHandle)
{
	auto EventListHandle = GetEventListHandle(PropertyHandle);
	auto EventParameterType = GetNativeParameterType(PropertyHandle);
	auto ItemPropertyHandle = EventListHandle->GetElement(itemIndex);
	auto TargetObjectHandle = ItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, TargetObject));
	UObject* TargetObject = nullptr;
	TargetObjectHandle->GetValue(TargetObject);
	if (TargetObject == nullptr)
	{
		return SNew(SBox);
	}

	FMenuBuilder MenuBuilder(true, MakeShareable(new FUICommandList));

	auto FunctionField = TFieldRange<UFunction>(TargetObject->GetClass());
	for (auto Func : FunctionField)
	{
		ELEventDelegateParameterType ParamType;
		if (ULEventDelegateParameterHelper::IsSupportedFunction(Func, ParamType))//show only supported type
		{
			FString ParamTypeString = ULEventDelegateParameterHelper::ParameterTypeToName(ParamType, Func);
			auto FunctionSelectorName = FString::Printf(TEXT("%s(%s)"), *Func->GetName(), *ParamTypeString);
			MenuBuilder.AddMenuEntry(
				FUIAction(FExecuteAction::CreateRaw(this, &FLEventDelegateCustomization::OnSelectFunction, Func->GetFName(), ParamType, false, ItemPropertyHandle)),
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(EHorizontalAlignment::HAlign_Left)
				[
					SNew(STextBlock)
					.Text(FText::FromString(FunctionSelectorName))
					.Font(IDetailLayoutBuilder::GetDetailFont())
				]
			);
			if (ParamType == EventParameterType && EventParameterType != ELEventDelegateParameterType::Empty)//if function support native parameter, then draw another button, and show as native parameter
			{
				FunctionSelectorName = FString::Printf(TEXT("%s(NativeParameter)"), *Func->GetName());
				MenuBuilder.AddMenuEntry(
					FUIAction(FExecuteAction::CreateRaw(this, &FLEventDelegateCustomization::OnSelectFunction, Func->GetFName(), ParamType, true, ItemPropertyHandle)),
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.HAlign(EHorizontalAlignment::HAlign_Left)
					[
						SNew(STextBlock)
						.Text(FText::FromString(FunctionSelectorName))
						.Font(IDetailLayoutBuilder::GetDetailFont())
					]
				);
			}					
		}
	}
	return MenuBuilder.MakeWidget();
}

bool FLEventDelegateCustomization::IsComponentSelectorMenuEnabled(TSharedRef<IPropertyHandle> ItemPropertyHandle)const
{
	auto HelperActorHandle = ItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, HelperActor));
	UObject* HelperActorObject = nullptr;
	HelperActorHandle->GetValue(HelperActorObject);
	return IsValid(HelperActorObject);
}
bool FLEventDelegateCustomization::IsFunctionSelectorMenuEnabled(TSharedRef<IPropertyHandle> ItemPropertyHandle)const
{
	auto HelperActorHandle = ItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, HelperActor));
	UObject* HelperActorObject = nullptr;
	HelperActorHandle->GetValue(HelperActorObject);

	auto TargetObjectHandle = ItemPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, TargetObject));
	UObject* TargetObject = nullptr;
	TargetObjectHandle->GetValue(TargetObject);

	return IsValid(HelperActorObject) && IsValid(TargetObject);
}
void FLEventDelegateCustomization::OnClickListAdd(TSharedRef<IPropertyHandle> PropertyHandle)
{
	auto EventListHandle = GetEventListHandle(PropertyHandle);
	EventListHandle->AddItem();
	PropertyUtilites->ForceRefresh();
}
void FLEventDelegateCustomization::OnClickListEmpty(TSharedRef<IPropertyHandle> PropertyHandle)
{
	auto EventListHandle = GetEventListHandle(PropertyHandle);
	EventListHandle->EmptyArray();
	PropertyUtilites->ForceRefresh();
}
FReply FLEventDelegateCustomization::OnClickAddRemove(bool AddOrRemove, int32 Index, int32 Count, TSharedRef<IPropertyHandle> PropertyHandle)
{
	auto EventListHandle = GetEventListHandle(PropertyHandle);
	if (AddOrRemove)
	{
		if (Count == 0)
		{
			EventListHandle->AddItem();
		}
		else
		{
			if (Index == Count - 1)//current is last, add to last
				EventListHandle->AddItem();
			else
				EventListHandle->Insert(Index + 1);
		}
	}
	else
	{
		if (Count != 0)
			EventListHandle->DeleteItem(Index);
	}
	PropertyUtilites->ForceRefresh();
	return FReply::Handled();
}
FReply FLEventDelegateCustomization::OnClickCopyPaste(bool CopyOrPaste, int32 Index, TSharedRef<IPropertyHandle> PropertyHandle)
{
	auto EventListHandle = GetEventListHandle(PropertyHandle);
	auto EventDataHandle = EventListHandle->GetElement(Index);
	if (CopyOrPaste)
	{
		CopySourceData.Reset();
		EventDataHandle->GetPerObjectValues(CopySourceData);
	}
	else
	{
		EventDataHandle->SetPerObjectValues(CopySourceData);
		PropertyUtilites->ForceRefresh();
	}
	return FReply::Handled();
}

FReply FLEventDelegateCustomization::OnClickDuplicate(int32 Index, TSharedRef<IPropertyHandle> PropertyHandle)
{
	auto EventListHandle = GetEventListHandle(PropertyHandle);
	auto EventDataHandle = EventListHandle->GetElement(Index);
	EventListHandle->DuplicateItem(Index);
	return FReply::Handled();
}
FReply FLEventDelegateCustomization::OnClickMoveUpDown(bool UpOrDown, int32 Index, TSharedRef<IPropertyHandle> PropertyHandle)
{
	auto EventListHandle = GetEventListHandle(PropertyHandle);
	if (UpOrDown)
	{
		if (Index <= 0)
			return FReply::Handled();

		EventListHandle->SwapItems(Index, Index - 1);
	}
	else
	{
		uint32 arrayCount;
		EventListHandle->GetNumElements(arrayCount);
		if (Index + 1 >= (int32)arrayCount)
			return FReply::Handled();

		EventListHandle->SwapItems(Index, Index + 1);
	}
	return FReply::Handled();
}


#define SET_VALUE_ON_BUFFER(type)\
auto ParamBuffer = GetBuffer(ParamBufferHandle);\
FMemoryReader Reader(ParamBuffer);\
type Value;\
Reader << Value;\
ValueHandle->SetValue(Value);

TSharedRef<SWidget> FLEventDelegateCustomization::DrawFunctionParameter(TSharedRef<IPropertyHandle> InDataContainerHandle, ELEventDelegateParameterType InFunctionParameterType, UFunction* InFunction)
{
	auto ParamBufferHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, ParamBuffer));
	if (InFunctionParameterType != ELEventDelegateParameterType::None)//None means not select function yet
	{
		switch (InFunctionParameterType)
		{
		default:
		case ELEventDelegateParameterType::Empty:
		{
			ClearValueBuffer(InDataContainerHandle);
			ClearReferenceValue(InDataContainerHandle);
			return
				SNew(SBox)
				.MinDesiredWidth(500)
				.VAlign(EVerticalAlignment::VAlign_Center)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("(No parameter)", "(No parameter)"))
					.Font(IDetailLayoutBuilder::GetDetailFont())
				]
			;
		}
		break;
		case ELEventDelegateParameterType::Bool:
		{
			ClearReferenceValue(InDataContainerHandle);
			SetBufferLength(ParamBufferHandle, 1);
			auto ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, BoolValue));
			auto ParamBuffer = GetBuffer(ParamBufferHandle);
			bool Value = ParamBuffer[0] == 1;
			ValueHandle->SetValue(Value);
			ValueHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FLEventDelegateCustomization::BoolValueChange, ValueHandle, ParamBufferHandle));
			return ValueHandle->CreatePropertyValueWidget();
		}
		break;
		case ELEventDelegateParameterType::Float:
		{
			ClearReferenceValue(InDataContainerHandle);
			SetBufferLength(ParamBufferHandle, 4);
			auto ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, FloatValue));
			SET_VALUE_ON_BUFFER(float);
			ValueHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FLEventDelegateCustomization::FloatValueChange, ValueHandle, ParamBufferHandle));
			return ValueHandle->CreatePropertyValueWidget();
		}
		break;
		case ELEventDelegateParameterType::Double:
		{
			ClearReferenceValue(InDataContainerHandle);
			SetBufferLength(ParamBufferHandle, 8);
			auto ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, DoubleValue));
			SET_VALUE_ON_BUFFER(double);
			ValueHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FLEventDelegateCustomization::DoubleValueChange, ValueHandle, ParamBufferHandle));
			return ValueHandle->CreatePropertyValueWidget();
		}
		break;
		case ELEventDelegateParameterType::Int8:
		{
			ClearReferenceValue(InDataContainerHandle);
			SetBufferLength(ParamBufferHandle, 1);
			auto ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, Int8Value));
			SET_VALUE_ON_BUFFER(int8);
			ValueHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FLEventDelegateCustomization::Int8ValueChange, ValueHandle, ParamBufferHandle));
			return ValueHandle->CreatePropertyValueWidget();
		}
		break;
		case ELEventDelegateParameterType::UInt8:
		{
			ClearReferenceValue(InDataContainerHandle);
			SetBufferLength(ParamBufferHandle, 1);
			auto ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, UInt8Value));
			SET_VALUE_ON_BUFFER(uint8);
			if (auto enumValue = ULEventDelegateParameterHelper::GetEnumParameter(InFunction))
			{
				return
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.VAlign(VAlign_Center)
					.FillWidth(1.0f)
					.Padding(0.0f, 2.0f)
					[
						SNew(SBox)
						.MinDesiredWidth(500)
						[
							SNew(SEnumComboBox, enumValue)
							.CurrentValue(this, &FLEventDelegateCustomization::GetEnumValue, ValueHandle)
							.OnEnumSelectionChanged(this, &FLEventDelegateCustomization::EnumValueChange, ValueHandle, ParamBufferHandle)
						]
					]
				;
			}
			else
			{
				ValueHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FLEventDelegateCustomization::UInt8ValueChange, ValueHandle, ParamBufferHandle));
				return ValueHandle->CreatePropertyValueWidget();
			}
		}
		break;
		case ELEventDelegateParameterType::Int16:
		{
			ClearReferenceValue(InDataContainerHandle);
			SetBufferLength(ParamBufferHandle, 2);
			auto ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, Int16Value));
			SET_VALUE_ON_BUFFER(int16);
			ValueHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FLEventDelegateCustomization::Int16ValueChange, ValueHandle, ParamBufferHandle));
			return ValueHandle->CreatePropertyValueWidget();
		}
		break;
		case ELEventDelegateParameterType::UInt16:
		{
			ClearReferenceValue(InDataContainerHandle);
			SetBufferLength(ParamBufferHandle, 2);
			auto ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, UInt16Value));
			SET_VALUE_ON_BUFFER(uint16);
			ValueHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FLEventDelegateCustomization::UInt16ValueChange, ValueHandle, ParamBufferHandle));
			return ValueHandle->CreatePropertyValueWidget();
		}
		break;
		case ELEventDelegateParameterType::Int32:
		{
			ClearReferenceValue(InDataContainerHandle);
			SetBufferLength(ParamBufferHandle, 4);
			auto ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, Int32Value));
			SET_VALUE_ON_BUFFER(int32);
			ValueHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FLEventDelegateCustomization::Int32ValueChange, ValueHandle, ParamBufferHandle));
			return ValueHandle->CreatePropertyValueWidget();
		}
		break;
		case ELEventDelegateParameterType::UInt32:
		{
			ClearReferenceValue(InDataContainerHandle);
			SetBufferLength(ParamBufferHandle, 4);
			auto ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, UInt32Value));
			SET_VALUE_ON_BUFFER(uint32);
			ValueHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FLEventDelegateCustomization::UInt32ValueChange, ValueHandle, ParamBufferHandle));
			return ValueHandle->CreatePropertyValueWidget();
		}
		break;
		case ELEventDelegateParameterType::Int64:
		{
			ClearReferenceValue(InDataContainerHandle);
			SetBufferLength(ParamBufferHandle, 8);
			auto ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, Int64Value));
			SET_VALUE_ON_BUFFER(int64);
			ValueHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FLEventDelegateCustomization::Int64ValueChange, ValueHandle, ParamBufferHandle));
			return ValueHandle->CreatePropertyValueWidget();
		}
		break;
		case ELEventDelegateParameterType::UInt64:
		{
			ClearReferenceValue(InDataContainerHandle);
			SetBufferLength(ParamBufferHandle, 8);
			auto ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, UInt64Value));
			SET_VALUE_ON_BUFFER(uint64);
			ValueHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FLEventDelegateCustomization::UInt64ValueChange, ValueHandle, ParamBufferHandle));
			return ValueHandle->CreatePropertyValueWidget();
		}
		break;
		case ELEventDelegateParameterType::Vector2:
		{
			ClearReferenceValue(InDataContainerHandle);
			SetBufferLength(ParamBufferHandle, 8);
			auto ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, Vector2Value));
			SET_VALUE_ON_BUFFER(FVector2D);
			return SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.FillWidth(1.0f)
				.Padding(0.0f, 2.0f)
				[
					SNew(SLGUIVectorInputBox)
					.AllowSpin(false)
					.bColorAxisLabels(true)
					.EnableX(true)
					.EnableY(true)
					.ShowX(true)
					.ShowY(true)
					.X(this, &FLEventDelegateCustomization::Vector2GetItemValue, 0, ValueHandle, ParamBufferHandle)
					.Y(this, &FLEventDelegateCustomization::Vector2GetItemValue, 1, ValueHandle, ParamBufferHandle)
					.OnXCommitted(this, &FLEventDelegateCustomization::Vector2ItemValueChange, 0, ValueHandle, ParamBufferHandle)
					.OnYCommitted(this, &FLEventDelegateCustomization::Vector2ItemValueChange, 1, ValueHandle, ParamBufferHandle)
				]
			;
		}
		break;
		case ELEventDelegateParameterType::Vector3:
		{
			ClearReferenceValue(InDataContainerHandle);
			SetBufferLength(ParamBufferHandle, 12);
			auto ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, Vector3Value));
			SET_VALUE_ON_BUFFER(FVector);
			return SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.FillWidth(1.0f)
				.Padding(0.0f, 2.0f)
				[
					SNew(SLGUIVectorInputBox)
					.AllowSpin(false)
					.bColorAxisLabels(true)
					.EnableX(true)
					.EnableY(true)
					.EnableZ(true)
					.ShowX(true)
					.ShowY(true)
					.ShowZ(true)
					.X(this, &FLEventDelegateCustomization::Vector3GetItemValue, 0, ValueHandle, ParamBufferHandle)
					.Y(this, &FLEventDelegateCustomization::Vector3GetItemValue, 1, ValueHandle, ParamBufferHandle)
					.Z(this, &FLEventDelegateCustomization::Vector3GetItemValue, 2, ValueHandle, ParamBufferHandle)
					.OnXCommitted(this, &FLEventDelegateCustomization::Vector3ItemValueChange, 0, ValueHandle, ParamBufferHandle)
					.OnYCommitted(this, &FLEventDelegateCustomization::Vector3ItemValueChange, 1, ValueHandle, ParamBufferHandle)
					.OnZCommitted(this, &FLEventDelegateCustomization::Vector3ItemValueChange, 2, ValueHandle, ParamBufferHandle)
				]
			;
		}
		break;
		case ELEventDelegateParameterType::Vector4:
		{
			ClearReferenceValue(InDataContainerHandle);
			SetBufferLength(ParamBufferHandle, 16);
			auto ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, Vector4Value));
			SET_VALUE_ON_BUFFER(FVector4);
			return SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.FillWidth(1.0f)
				.Padding(0.0f, 2.0f)
				[
					SNew(SLGUIVectorInputBox)
					.AllowSpin(false)
					.bColorAxisLabels(true)
					.EnableX(true)
					.EnableY(true)
					.EnableZ(true)
					.EnableW(true)
					.ShowX(true)
					.ShowY(true)
					.ShowZ(true)
					.ShowW(true)
					.X(this, &FLEventDelegateCustomization::Vector4GetItemValue, 0, ValueHandle, ParamBufferHandle)
					.Y(this, &FLEventDelegateCustomization::Vector4GetItemValue, 1, ValueHandle, ParamBufferHandle)
					.Z(this, &FLEventDelegateCustomization::Vector4GetItemValue, 2, ValueHandle, ParamBufferHandle)
					.W(this, &FLEventDelegateCustomization::Vector4GetItemValue, 3, ValueHandle, ParamBufferHandle)
					.OnXCommitted(this, &FLEventDelegateCustomization::Vector4ItemValueChange, 0, ValueHandle, ParamBufferHandle)
					.OnYCommitted(this, &FLEventDelegateCustomization::Vector4ItemValueChange, 1, ValueHandle, ParamBufferHandle)
					.OnZCommitted(this, &FLEventDelegateCustomization::Vector4ItemValueChange, 2, ValueHandle, ParamBufferHandle)
					.OnWCommitted(this, &FLEventDelegateCustomization::Vector4ItemValueChange, 3, ValueHandle, ParamBufferHandle)
				]
			;
		}
		break;
		case ELEventDelegateParameterType::Color:
		{
			ClearReferenceValue(InDataContainerHandle);
			SetBufferLength(ParamBufferHandle, 4);
			auto ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, ColorValue));
			auto ParamBuffer = GetBuffer(ParamBufferHandle);
			FMemoryReader Reader(ParamBuffer);
			FColor Value;
			Reader << Value;
			ValueHandle->SetValueFromFormattedString(Value.ToString());
			return SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.Padding(0.0f, 2.0f)
				[
					// Displays the color with alpha unless it is ignored
					SAssignNew(ColorPickerParentWidget, SColorBlock)
					.Color(this, &FLEventDelegateCustomization::LinearColorGetValue, false, ValueHandle, ParamBufferHandle)
					.ShowBackgroundForAlpha(true)
					.AlphaDisplayMode(EColorBlockAlphaDisplayMode::Separate)
					.OnMouseButtonDown(this, &FLEventDelegateCustomization::OnMouseButtonDownColorBlock, false, ValueHandle, ParamBufferHandle)
					.Size(FVector2D(35.0f, 12.0f))
				]
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.Padding(0.0f, 2.0f)
				[
					// Displays the color without alpha
					SNew(SColorBlock)
					.Color(this, &FLEventDelegateCustomization::LinearColorGetValue, false, ValueHandle, ParamBufferHandle)
					.ShowBackgroundForAlpha(false)
					.AlphaDisplayMode(EColorBlockAlphaDisplayMode::Ignore)
					.OnMouseButtonDown(this, &FLEventDelegateCustomization::OnMouseButtonDownColorBlock, false, ValueHandle, ParamBufferHandle)
					.Size(FVector2D(35.0f, 12.0f))
				];
			;
		}
		break;
		case ELEventDelegateParameterType::LinearColor:
		{
			ClearReferenceValue(InDataContainerHandle);
			SetBufferLength(ParamBufferHandle, 16);
			auto ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, LinearColorValue));
			SET_VALUE_ON_BUFFER(FLinearColor);
			return SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.Padding(0.0f, 2.0f)
				[
					// Displays the color with alpha unless it is ignored
					SAssignNew(ColorPickerParentWidget, SColorBlock)
					.Color(this, &FLEventDelegateCustomization::LinearColorGetValue, true, ValueHandle, ParamBufferHandle)
					.ShowBackgroundForAlpha(true)
					.AlphaDisplayMode(EColorBlockAlphaDisplayMode::Separate)
					.OnMouseButtonDown(this, &FLEventDelegateCustomization::OnMouseButtonDownColorBlock, true, ValueHandle, ParamBufferHandle)
					.Size(FVector2D(35.0f, 12.0f))
				]
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.Padding(0.0f, 2.0f)
				[
					// Displays the color without alpha
					SNew(SColorBlock)
					.Color(this, &FLEventDelegateCustomization::LinearColorGetValue, true, ValueHandle, ParamBufferHandle)
					.ShowBackgroundForAlpha(false)
					.AlphaDisplayMode(EColorBlockAlphaDisplayMode::Ignore)
					.OnMouseButtonDown(this, &FLEventDelegateCustomization::OnMouseButtonDownColorBlock, true, ValueHandle, ParamBufferHandle)
					.Size(FVector2D(35.0f, 12.0f))
				];
			;
		}
		break;
		case ELEventDelegateParameterType::Quaternion:
		{
			ClearReferenceValue(InDataContainerHandle);
			SetBufferLength(ParamBufferHandle, 16);
			auto ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, QuatValue));
			SET_VALUE_ON_BUFFER(FQuat);
			return SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.FillWidth(1.0f)
				.Padding(0.0f, 2.0f)
				[
					SNew(SLGUIVectorInputBox)
					.AllowSpin(false)
					.bColorAxisLabels(true)
					.EnableX(true)
					.EnableY(true)
					.EnableZ(true)
					.EnableW(true)
					.ShowX(true)
					.ShowY(true)
					.ShowZ(true)
					.ShowW(true)
					.X(this, &FLEventDelegateCustomization::Vector4GetItemValue, 0, ValueHandle, ParamBufferHandle)
					.Y(this, &FLEventDelegateCustomization::Vector4GetItemValue, 1, ValueHandle, ParamBufferHandle)
					.Z(this, &FLEventDelegateCustomization::Vector4GetItemValue, 2, ValueHandle, ParamBufferHandle)
					.W(this, &FLEventDelegateCustomization::Vector4GetItemValue, 3, ValueHandle, ParamBufferHandle)
					.OnXCommitted(this, &FLEventDelegateCustomization::Vector4ItemValueChange, 0, ValueHandle, ParamBufferHandle)
					.OnYCommitted(this, &FLEventDelegateCustomization::Vector4ItemValueChange, 1, ValueHandle, ParamBufferHandle)
					.OnZCommitted(this, &FLEventDelegateCustomization::Vector4ItemValueChange, 2, ValueHandle, ParamBufferHandle)
					.OnWCommitted(this, &FLEventDelegateCustomization::Vector4ItemValueChange, 3, ValueHandle, ParamBufferHandle)
				]
			;
		}
		break;
		case ELEventDelegateParameterType::String:
		{
			ClearReferenceValue(InDataContainerHandle);
			auto ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, StringValue));
			SET_VALUE_ON_BUFFER(FString);
			ValueHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FLEventDelegateCustomization::StringValueChange, ValueHandle, ParamBufferHandle));
			return ValueHandle->CreatePropertyValueWidget();
		}
		break;
		case ELEventDelegateParameterType::Name:
		{
			ClearReferenceValue(InDataContainerHandle);
			auto ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, NameValue));
			SET_VALUE_ON_BUFFER(FName);
			ValueHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FLEventDelegateCustomization::NameValueChange, ValueHandle, ParamBufferHandle));
			return ValueHandle->CreatePropertyValueWidget();
		}
		case ELEventDelegateParameterType::Text:
		{
			ClearReferenceValue(InDataContainerHandle);
			auto ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, TextValue));
			SET_VALUE_ON_BUFFER(FText);
			ValueHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FLEventDelegateCustomization::TextValueChange, ValueHandle, ParamBufferHandle));
			TSharedRef<IEditableTextProperty> EditableTextProperty = MakeShareable(new FLGUIEditableTextPropertyHandle(ValueHandle.ToSharedRef(), PropertyUtilites));
			const bool bIsMultiLine = EditableTextProperty->IsMultiLineText();
			return 
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.FillWidth(1.0f)
				.Padding(0.0f, 2.0f)
				[
					SNew(SBox)
					.MinDesiredWidth(bIsMultiLine ? 250.f : 125.f)
					.MaxDesiredWidth(600)
					[
						SNew(STextPropertyEditableTextBox, EditableTextProperty)
						.Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"))
						.AutoWrapText(true)
					]
				]
				;
		}
		case ELEventDelegateParameterType::PointerEvent_DEPRECATED:
		{
			ClearValueBuffer(InDataContainerHandle);
			ClearReferenceValue(InDataContainerHandle);
			return
				SNew(SBox)
				.MinDesiredWidth(500)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("PointerEventDataNotEditableError", "(PointerEventData not editable! You can only pass native parameter!)"))
					.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
					.AutoWrapText(true)
					.ColorAndOpacity(FLinearColor(FColor(255, 0, 0, 255)))
					.Font(IDetailLayoutBuilder::GetDetailFont())
				];
		}
		break;
		case ELEventDelegateParameterType::Object:
		case ELEventDelegateParameterType::Actor:
		case ELEventDelegateParameterType::Class:
		{
			return
				SNew(SBox)
				.MinDesiredWidth(500)
				[
					DrawFunctionReferenceParameter(InDataContainerHandle, InFunctionParameterType, InFunction)
				];
		}
		break;
		case ELEventDelegateParameterType::Rotator:
		{
			ClearReferenceValue(InDataContainerHandle);
			SetBufferLength(ParamBufferHandle, 12);
			auto ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, RotatorValue));
			SET_VALUE_ON_BUFFER(FRotator);
			TSharedPtr<INumericTypeInterface<float>> TypeInterface;
			if (FUnitConversion::Settings().ShouldDisplayUnits())
			{
				TypeInterface = MakeShareable(new TNumericUnitTypeInterface<float>(EUnit::Degrees));
			}
			return SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.FillWidth(1.0f)
				.Padding(0.0f, 2.0f)
				[
					SNew(SRotatorInputBox)
					.AllowSpin(false)
					.bColorAxisLabels(true)
					.Roll(this, &FLEventDelegateCustomization::RotatorGetItemValue, 0, ValueHandle, ParamBufferHandle)
					.Pitch(this, &FLEventDelegateCustomization::RotatorGetItemValue, 1, ValueHandle, ParamBufferHandle)
					.Yaw(this, &FLEventDelegateCustomization::RotatorGetItemValue, 2, ValueHandle, ParamBufferHandle)
					.OnRollCommitted(this, &FLEventDelegateCustomization::RotatorValueChange, 0, ValueHandle, ParamBufferHandle)
					.OnPitchCommitted(this, &FLEventDelegateCustomization::RotatorValueChange, 1, ValueHandle, ParamBufferHandle)
					.OnYawCommitted(this, &FLEventDelegateCustomization::RotatorValueChange, 2, ValueHandle, ParamBufferHandle)
					.TypeInterface(TypeInterface)
				]
			;
		}
		break;
		}
	}
	else
	{
		ClearValueBuffer(InDataContainerHandle);
		ClearReferenceValue(InDataContainerHandle);
		return
			SNew(STextBlock)
			.Font(IDetailLayoutBuilder::GetDetailFont())
			.Text(LOCTEXT("(Not handled)", "(Not handled)"));
	}
}
//function's parameter editor
TSharedRef<SWidget> FLEventDelegateCustomization::DrawFunctionReferenceParameter(TSharedRef<IPropertyHandle> InDataContainerHandle, ELEventDelegateParameterType FunctionParameterType, UFunction* InFunction)
{
	auto ParamBufferHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, ParamBuffer));

	TSharedPtr<SWidget> ParameterContent;
	switch (FunctionParameterType)
	{
	case ELEventDelegateParameterType::Object:
	{
		ClearValueBuffer(InDataContainerHandle);
		return SNew(SObjectPropertyEntryBox)
			.IsEnabled(true)
			.AllowedClass(ULEventDelegateParameterHelper::GetObjectParameterClass(InFunction))
			.PropertyHandle(InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, ReferenceObject)))
			.AllowClear(true)
			.ToolTipText(LOCTEXT("UObjectTips", "UObject only referece asset, dont use for HelperActor"))
			.OnObjectChanged(this, &FLEventDelegateCustomization::ObjectValueChange, ParamBufferHandle, InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, ReferenceObject)), true);
	}
	break;
	case ELEventDelegateParameterType::Actor:
	{
		ClearValueBuffer(InDataContainerHandle);
		return SNew(SObjectPropertyEntryBox)
			.IsEnabled(true)
			.AllowedClass(ULEventDelegateParameterHelper::GetObjectParameterClass(InFunction))
			.PropertyHandle(InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, ReferenceObject)))
			.AllowClear(true)
			.OnObjectChanged(this, &FLEventDelegateCustomization::ObjectValueChange, ParamBufferHandle, InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, ReferenceObject)), false);
	}
	break;
	case ELEventDelegateParameterType::Class:
	{
		auto MetaClass = ULEventDelegateParameterHelper::GetClassParameterClass(InFunction);
		auto ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, ReferenceObject));
		ClearValueBuffer(InDataContainerHandle);
		return SNew(SClassPropertyEntryBox)
			.IsEnabled(true)
			.AllowAbstract(true)
			.AllowNone(true)
			.MetaClass(MetaClass)
			.SelectedClass(this, &FLEventDelegateCustomization::GetClassValue, ValueHandle)
			.OnSetClass(this, &FLEventDelegateCustomization::ClassValueChange, ValueHandle);
	}
	break;
	default:
		break;
	}
	return 
		SNew(STextBlock)
		.Font(IDetailLayoutBuilder::GetDetailFont())
		.Text(LOCTEXT("(Not handled)", "(Not handled)"));
}

void FLEventDelegateCustomization::ObjectValueChange(const FAssetData& InObj, TSharedPtr<IPropertyHandle> BufferHandle, TSharedPtr<IPropertyHandle> ObjectReferenceHandle, bool ObjectOrActor)
{
	if (ObjectOrActor)
	{
		//ObjectReference is not for HelperActor reference
		if (InObj.IsValid() && InObj.GetClass()->IsChildOf(AActor::StaticClass()))
		{
			UE_LOG(LEventDelegateEditor, Error, TEXT("Please use Actor type for referece Actor, UObject is for asset object referece"));
			AActor* NullActor = nullptr;
			ObjectReferenceHandle->SetValue(NullActor);
		}
		else
		{
			ObjectReferenceHandle->SetValue(InObj);
		}
	}
	else
	{
		ObjectReferenceHandle->SetValue(InObj);
	}
}
const UClass* FLEventDelegateCustomization::GetClassValue(TSharedPtr<IPropertyHandle> ClassReferenceHandle)const
{
	UObject* referenceClassObject = nullptr;
	ClassReferenceHandle->GetValue(referenceClassObject);
	return (UClass*)referenceClassObject;
}
void FLEventDelegateCustomization::ClassValueChange(const UClass* InClass, TSharedPtr<IPropertyHandle> ClassReferenceHandle)
{
	ClassReferenceHandle->SetValue(InClass);
}
void FLEventDelegateCustomization::EnumValueChange(int32 InValue, ESelectInfo::Type SelectionType, TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)
{
	uint8 Value = (uint8)InValue;
	ValueHandle->SetValue(Value);
	UInt8ValueChange(ValueHandle, BufferHandle);
}

#define SET_BUFFER_ON_VALUE(type)\
type Value;\
ValueHandle->GetValue(Value);\
FBufferArchive ToBinary;\
ToBinary << Value;\
SetBufferValue(BufferHandle, ToBinary);

void FLEventDelegateCustomization::BoolValueChange(TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)
{
	bool Value; 
	ValueHandle->GetValue(Value); 
	TArray<uint8> Buffer;
	Buffer.Add(Value ? 1 : 0);
	SetBufferValue(BufferHandle, Buffer);
}
void FLEventDelegateCustomization::FloatValueChange(TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)
{
	SET_BUFFER_ON_VALUE(float);
}
void FLEventDelegateCustomization::DoubleValueChange(TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)
{
	SET_BUFFER_ON_VALUE(double);
}
void FLEventDelegateCustomization::Int8ValueChange(TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)
{
	SET_BUFFER_ON_VALUE(int8);
}
void FLEventDelegateCustomization::UInt8ValueChange(TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)
{
	SET_BUFFER_ON_VALUE(uint8);
}
void FLEventDelegateCustomization::Int16ValueChange(TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)
{
	SET_BUFFER_ON_VALUE(int16);
}
void FLEventDelegateCustomization::UInt16ValueChange(TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)
{
	SET_BUFFER_ON_VALUE(uint16);
}
void FLEventDelegateCustomization::Int32ValueChange(TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)
{
	SET_BUFFER_ON_VALUE(int32);
}
void FLEventDelegateCustomization::UInt32ValueChange(TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)
{
	SET_BUFFER_ON_VALUE(uint32);
}
void FLEventDelegateCustomization::Int64ValueChange(TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)
{
	SET_BUFFER_ON_VALUE(int64);
}
void FLEventDelegateCustomization::UInt64ValueChange(TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)
{
	SET_BUFFER_ON_VALUE(uint64);
}
void FLEventDelegateCustomization::StringValueChange(TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)
{
	SET_BUFFER_ON_VALUE(FString);
}
void FLEventDelegateCustomization::NameValueChange(TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)
{
	SET_BUFFER_ON_VALUE(FName);
}
void FLEventDelegateCustomization::TextValueChange(TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)
{
	SET_BUFFER_ON_VALUE(FText);
}
void FLEventDelegateCustomization::Vector2ItemValueChange(float NewValue, ETextCommit::Type CommitInfo, int AxisType, TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)
{
	FVector2D Value;
	ValueHandle->GetValue(Value);
	switch (AxisType)
	{
	case 0:	Value.X = NewValue; break;
	case 1:	Value.Y = NewValue; break;
	}
	ValueHandle->SetValue(Value);
	FBufferArchive ToBinary;
	ToBinary << Value;
	SetBufferValue(BufferHandle, ToBinary);
}
TOptional<float> FLEventDelegateCustomization::Vector2GetItemValue(int AxisType, TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)const
{
	FVector2D Value;
	ValueHandle->GetValue(Value);
	switch (AxisType)
	{
	default:
	case 0: return	Value.X;
	case 1: return	Value.Y;
	}
}
void FLEventDelegateCustomization::Vector3ItemValueChange(float NewValue, ETextCommit::Type CommitInfo, int AxisType, TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)
{
	FVector Value;
	ValueHandle->GetValue(Value);
	switch (AxisType)
	{
	case 0:	Value.X = NewValue; break;
	case 1:	Value.Y = NewValue; break;
	case 2:	Value.Z = NewValue; break;
	}
	ValueHandle->SetValue(Value);
	FBufferArchive ToBinary;
	ToBinary << Value;
	SetBufferValue(BufferHandle, ToBinary);
}
TOptional<float> FLEventDelegateCustomization::Vector3GetItemValue(int AxisType, TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)const
{
	FVector Value;
	ValueHandle->GetValue(Value);
	switch (AxisType)
	{
	default:
	case 0: return	Value.X;
	case 1: return	Value.Y;
	case 2: return	Value.Z;
	}
}
void FLEventDelegateCustomization::Vector4ItemValueChange(float NewValue, ETextCommit::Type CommitInfo, int AxisType, TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)
{
	FVector4 Value;
	ValueHandle->GetValue(Value);
	switch (AxisType)
	{
	case 0:	Value.X = NewValue; break;
	case 1:	Value.Y = NewValue; break;
	case 2:	Value.Z = NewValue; break;
	case 3:	Value.W = NewValue; break;
	}
	ValueHandle->SetValue(Value);
	FBufferArchive ToBinary;
	ToBinary << Value;
	SetBufferValue(BufferHandle, ToBinary);
}
TOptional<float> FLEventDelegateCustomization::Vector4GetItemValue(int AxisType, TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)const
{
	FVector4 Value;
	ValueHandle->GetValue(Value);
	switch (AxisType)
	{
	default:
	case 0: return	Value.X;
	case 1: return	Value.Y;
	case 2: return	Value.Z;
	case 3: return	Value.W;
	}
}
FLinearColor FLEventDelegateCustomization::LinearColorGetValue(bool bIsLinearColor, TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)const
{
	if (bIsLinearColor)
	{
		FLinearColor Value;
		FString FormatedString;
		ValueHandle->GetValueAsFormattedString(FormatedString);
		Value.InitFromString(FormatedString);
		return Value;
	}
	else
	{
		FColor Value;
		FString FormatedString;
		ValueHandle->GetValueAsFormattedString(FormatedString);
		Value.InitFromString(FormatedString);
		return FLinearColor(Value.R / 255.0f, Value.G / 255.0f, Value.B / 255.0f, Value.A / 255.0f);
	}
}
void FLEventDelegateCustomization::LinearColorValueChange(FLinearColor NewValue, bool bIsLinearColor, TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)
{
	if (bIsLinearColor)
	{
		FString FormatedString = NewValue.ToString();
		ValueHandle->SetValueFromFormattedString(FormatedString);
		FBufferArchive ToBinary;
		ToBinary << NewValue;
		SetBufferValue(BufferHandle, ToBinary);
	}
	else
	{
		FColor ColorValue = NewValue.ToFColor(false);
		FString FormatedString = ColorValue.ToString();
		ValueHandle->SetValueFromFormattedString(FormatedString);
		FBufferArchive ToBinary;
		ToBinary << ColorValue;
		SetBufferValue(BufferHandle, ToBinary);
	}
}
FReply FLEventDelegateCustomization::OnMouseButtonDownColorBlock(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, bool bIsLinearColor, TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)
{
	if (MouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
	{
		return FReply::Unhandled();
	}

	CreateColorPicker(bIsLinearColor, ValueHandle, BufferHandle);

	return FReply::Handled();
}
TOptional<float> FLEventDelegateCustomization::RotatorGetItemValue(int AxisType, TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)const
{
	FRotator Value;
	ValueHandle->GetValue(Value);
	switch (AxisType)
	{
	default:
	case 0: return	Value.Roll;
	case 1: return	Value.Pitch;
	case 2: return	Value.Yaw;
	}
}
void FLEventDelegateCustomization::RotatorValueChange(float NewValue, ETextCommit::Type CommitInfo, int AxisType, TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)
{
	FRotator Value;
	ValueHandle->GetValue(Value);
	switch (AxisType)
	{
	case 0:	Value.Roll = NewValue; break;
	case 1:	Value.Pitch = NewValue; break;
	case 2:	Value.Yaw = NewValue; break;
	}
	ValueHandle->SetValue(Value);
	FBufferArchive ToBinary;
	ToBinary << Value;
	SetBufferValue(BufferHandle, ToBinary);
}
void FLEventDelegateCustomization::SetBufferValue(TSharedPtr<IPropertyHandle> BufferHandle, const TArray<uint8>& BufferArray)
{
	auto BufferArrayHandle = BufferHandle->AsArray();
	auto bufferCount = BufferArray.Num();
	uint32 bufferHandleCount;
	BufferArrayHandle->GetNumElements(bufferHandleCount);
	if (bufferCount != (int32)bufferHandleCount)
	{
		BufferArrayHandle->EmptyArray();
		for (int i = 0; i < bufferCount; i++)
		{
			BufferArrayHandle->AddItem();

			auto bufferHandle = BufferArrayHandle->GetElement(i);
			auto buffer = BufferArray[i];
			bufferHandle->SetValue(buffer);
		}
	}
	else
	{
		for (int i = 0; i < bufferCount; i++)
		{
			auto bufferHandle = BufferArrayHandle->GetElement(i);
			auto buffer = BufferArray[i];
			bufferHandle->SetValue(buffer);
		}
	}
}

void FLEventDelegateCustomization::SetBufferLength(TSharedPtr<IPropertyHandle> BufferHandle, int32 Count)
{
	auto BufferArrayHandle = BufferHandle->AsArray();
	uint32 bufferHandleCount;
	BufferArrayHandle->GetNumElements(bufferHandleCount);
	if (Count != (int32)bufferHandleCount)
	{
		BufferArrayHandle->EmptyArray();
		for (int i = 0; i < Count; i++)
		{
			BufferArrayHandle->AddItem();
		}
	}
}

TArray<uint8> FLEventDelegateCustomization::GetBuffer(TSharedPtr<IPropertyHandle> BufferHandle)
{
	auto BufferArrayHandle = BufferHandle->AsArray();
	uint32 bufferHandleCount;
	BufferArrayHandle->GetNumElements(bufferHandleCount);
	TArray<uint8> resultBuffer;
	resultBuffer.Reserve(bufferHandleCount);
	for (uint32 i = 0; i < bufferHandleCount; i++)
	{
		auto elementHandle = BufferArrayHandle->GetElement(i);
		uint8 value;
		elementHandle->GetValue(value);
		resultBuffer.Add(value);
	}
	return resultBuffer;
}

TArray<uint8> FLEventDelegateCustomization::GetPropertyBuffer(TSharedPtr<IPropertyHandle> BufferHandle) const
{
	auto paramBufferArrayHandle = BufferHandle->AsArray();
	uint32 bufferCount;
	paramBufferArrayHandle->GetNumElements(bufferCount);
	TArray<uint8> paramBuffer;
	for (uint32 i = 0; i < bufferCount; i++)
	{
		auto bufferHandle = paramBufferArrayHandle->GetElement(i);
		uint8 buffer;
		bufferHandle->GetValue(buffer);
		paramBuffer.Add(buffer);
	}
	return paramBuffer;
}
int32 FLEventDelegateCustomization::GetEnumValue(TSharedPtr<IPropertyHandle> ValueHandle)const
{
	uint8 Value = 0;
	ValueHandle->GetValue(Value);
	return Value;
}
FText FLEventDelegateCustomization::GetTextValue(TSharedPtr<IPropertyHandle> ValueHandle)const
{
	FText Value;
	ValueHandle->GetValue(Value);
	return Value;
}
void FLEventDelegateCustomization::SetTextValue(const FText& InText, ETextCommit::Type InCommitType, TSharedPtr<IPropertyHandle> ValueHandle)
{
	ValueHandle->SetValue(InText);
}

void FLEventDelegateCustomization::ClearValueBuffer(TSharedPtr<IPropertyHandle> PropertyHandle)
{
	auto handle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, ParamBuffer))->AsArray();
	uint32 NumElements = 0;
	if (handle->GetNumElements(NumElements) == FPropertyAccess::Result::Success && NumElements > 0)
	{
		handle->EmptyArray();
	}
}
void FLEventDelegateCustomization::ClearReferenceValue(TSharedPtr<IPropertyHandle> PropertyHandle)
{
	ClearObjectValue(PropertyHandle);
}
void FLEventDelegateCustomization::ClearObjectValue(TSharedPtr<IPropertyHandle> PropertyHandle)
{
	auto handle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, ReferenceObject));
	UObject* Obj = nullptr;
	if (handle->GetValue(Obj) == FPropertyAccess::Result::Success && Obj != nullptr)
	{
		handle->ResetToDefault();
	}
}

void FLEventDelegateCustomization::OnParameterTypeChange(TSharedRef<IPropertyHandle> InDataContainerHandle)
{
	auto ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, BoolValue)); ValueHandle->ResetToDefault();
	ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, FloatValue)); ValueHandle->ResetToDefault();
	ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, DoubleValue)); ValueHandle->ResetToDefault();
	ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, Int8Value)); ValueHandle->ResetToDefault();
	ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, UInt8Value)); ValueHandle->ResetToDefault();
	ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, Int16Value)); ValueHandle->ResetToDefault();
	ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, UInt16Value)); ValueHandle->ResetToDefault();
	ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, Int32Value)); ValueHandle->ResetToDefault();
	ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, UInt32Value)); ValueHandle->ResetToDefault();
	ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, Int64Value)); ValueHandle->ResetToDefault();
	ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, UInt64Value)); ValueHandle->ResetToDefault();
	ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, Vector2Value)); ValueHandle->ResetToDefault();
	ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, Vector3Value)); ValueHandle->ResetToDefault();
	ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, Vector4Value)); ValueHandle->ResetToDefault();
	ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, QuatValue)); ValueHandle->ResetToDefault();
	ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, ColorValue)); ValueHandle->ResetToDefault();
	ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, LinearColorValue)); ValueHandle->ResetToDefault();
	ValueHandle = InDataContainerHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLEventDelegateData, RotatorValue)); ValueHandle->ResetToDefault();
}



void FLEventDelegateCustomization::CreateColorPicker(bool bIsLinearColor, TSharedPtr<IPropertyHandle> ValueHandle, TSharedPtr<IPropertyHandle> BufferHandle)
{
	FLinearColor InitialColor = LinearColorGetValue(bIsLinearColor, ValueHandle, BufferHandle);

	FColorPickerArgs PickerArgs;
	{
		PickerArgs.bUseAlpha = true;
		PickerArgs.bOnlyRefreshOnMouseUp = false;
		PickerArgs.bOnlyRefreshOnOk = false;
		PickerArgs.sRGBOverride = bIsLinearColor;
		PickerArgs.DisplayGamma = TAttribute<float>::Create(TAttribute<float>::FGetter::CreateUObject(GEngine, &UEngine::GetDisplayGamma));
		PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateSP(this, &FLEventDelegateCustomization::LinearColorValueChange, bIsLinearColor, ValueHandle, BufferHandle);
		//PickerArgs.OnColorPickerCancelled = FOnColorPickerCancelled::CreateSP(this, &FColorStructCustomization::OnColorPickerCancelled);
		//PickerArgs.OnInteractivePickBegin = FSimpleDelegate::CreateSP(this, &FColorStructCustomization::OnColorPickerInteractiveBegin);
		//PickerArgs.OnInteractivePickEnd = FSimpleDelegate::CreateSP(this, &FColorStructCustomization::OnColorPickerInteractiveEnd);
		PickerArgs.InitialColorOverride = InitialColor;
		PickerArgs.ParentWidget = ColorPickerParentWidget;
		PickerArgs.OptionalOwningDetailsView = ColorPickerParentWidget;
		FWidgetPath ParentWidgetPath;
		if (FSlateApplication::Get().FindPathToWidget(ColorPickerParentWidget.ToSharedRef(), ParentWidgetPath))
		{
			PickerArgs.bOpenAsMenu = FSlateApplication::Get().FindMenuInWidgetPath(ParentWidgetPath).IsValid();
		}
	}

	OpenColorPicker(PickerArgs);
}

PRAGMA_ENABLE_OPTIMIZATION
#undef LOCTEXT_NAMESPACE
