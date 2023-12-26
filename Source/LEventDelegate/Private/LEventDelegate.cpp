// Copyright 2019-Present LexLiu. All Rights Reserved.

#include "LEventDelegate.h"
#include "LEventDelegateModule.h"
#include "Serialization/MemoryReader.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"

#define LOCTEXT_NAMESPACE "LEventDelegate"

bool ULEventDelegateParameterHelper::IsFunctionCompatible(const UFunction* InFunction, ELEventDelegateParameterType& OutParameterType)
{
	if (InFunction->GetReturnProperty() != nullptr)return false;//not support return value for ProcessEvent
	TFieldIterator<FProperty> IteratorA(InFunction);
	TArray<ELEventDelegateParameterType> ParameterTypeArray;
	while (IteratorA && (IteratorA->PropertyFlags & CPF_Parm))
	{
		FProperty* PropA = *IteratorA;
		ELEventDelegateParameterType ParamType;
		if (IsPropertyCompatible(PropA, ParamType))
		{
			ParameterTypeArray.Add(ParamType);
		}
		else
		{
			// Type mismatch between an argument of A and B
			return false;
		}
		++IteratorA;
	}
	if (ParameterTypeArray.Num() == 1)
	{
		OutParameterType = ParameterTypeArray[0];
		return true;
	}
	if (ParameterTypeArray.Num() == 0)
	{
		OutParameterType = ELEventDelegateParameterType::Empty;
		return true;
	}
	return false;
}
bool ULEventDelegateParameterHelper::IsPropertyCompatible(const FProperty* InFunctionProperty, ELEventDelegateParameterType& OutParameterType)
{
	if (!InFunctionProperty)
	{
		return false;
	}

	auto PropertyID = InFunctionProperty->GetID();
	switch (*PropertyID.ToEName())
	{
	case NAME_BoolProperty:
	{
		OutParameterType = ELEventDelegateParameterType::Bool;
		return true;
	}
	case NAME_FloatProperty:
	{
		OutParameterType = ELEventDelegateParameterType::Float;
		return true;
	}
	case NAME_DoubleProperty:
	{
		OutParameterType = ELEventDelegateParameterType::Double;
		return true;
	}
	case NAME_Int8Property:
	{
		OutParameterType = ELEventDelegateParameterType::Int8;
		return true;
	}
	case NAME_ByteProperty:
	{
		OutParameterType = ELEventDelegateParameterType::UInt8;
		return true;
	}
	case NAME_Int16Property:
	{
		OutParameterType = ELEventDelegateParameterType::Int16;
		return true;
	}
	case NAME_UInt16Property:
	{
		OutParameterType = ELEventDelegateParameterType::UInt16;
		return true;
	}
	case NAME_IntProperty:
	{
		OutParameterType = ELEventDelegateParameterType::Int32;
		return true;
	}
	case NAME_UInt32Property:
	{
		OutParameterType = ELEventDelegateParameterType::UInt32;
		return true;
	}
	case NAME_Int64Property:
	{
		OutParameterType = ELEventDelegateParameterType::Int64;
		return true;
	}
	case NAME_UInt64Property:
	{
		OutParameterType = ELEventDelegateParameterType::UInt64;
		return true;
	}
	case NAME_EnumProperty:
	{
		OutParameterType = ELEventDelegateParameterType::UInt8;
		return true;
	}
	case NAME_StructProperty:
	{
		if (auto structProperty = CastField<FStructProperty>(InFunctionProperty))
		{
			if (structProperty->Struct != nullptr)
			{
				auto structName = structProperty->Struct->GetFName();
				if (structName == NAME_Vector2D)
				{
					OutParameterType = ELEventDelegateParameterType::Vector2; return true;
				}
				else if (structName == NAME_Vector)
				{
					OutParameterType = ELEventDelegateParameterType::Vector3; return true;
				}
				else if (structName == NAME_Vector4)
				{
					OutParameterType = ELEventDelegateParameterType::Vector4; return true;
				}
				else if (structName == NAME_Color)
				{
					OutParameterType = ELEventDelegateParameterType::Color; return true;
				}
				else if (structName == NAME_LinearColor)
				{
					OutParameterType = ELEventDelegateParameterType::LinearColor; return true;
				}
				else if (structName == NAME_Quat)
				{
					OutParameterType = ELEventDelegateParameterType::Quaternion; return true;
				}
				else if (structName == NAME_Rotator)
				{
					OutParameterType = ELEventDelegateParameterType::Rotator; return true;
				}
			}
		}
		return false;
	}

	case NAME_ObjectProperty:
	{
		if (auto classProperty = CastField<FClassProperty>(InFunctionProperty))
		{
			OutParameterType = ELEventDelegateParameterType::Class;
			return true;
		}
		else if (auto objectProperty = CastField<FObjectProperty>(InFunctionProperty))//if object property
		{
			if (objectProperty->PropertyClass->IsChildOf(AActor::StaticClass()))//if is Actor
			{
				OutParameterType = ELEventDelegateParameterType::Actor;
			}
			// else if (objectProperty->PropertyClass->IsChildOf(ULGUIPointerEventData::StaticClass()))
			// {
			// 	OutParameterType = ELEventDelegateParameterType::PointerEvent;
			// }
			else if (objectProperty->PropertyClass->IsChildOf(UActorComponent::StaticClass()))
			{
				return false;
			}
			else
			{
				OutParameterType = ELEventDelegateParameterType::Object;
			}
			return true;
		}
	}

	case NAME_StrProperty:
	{
		OutParameterType = ELEventDelegateParameterType::String;
		return true;
	}
	case NAME_NameProperty:
	{
		OutParameterType = ELEventDelegateParameterType::Name;
		return true;
	}
	case NAME_TextProperty:
	{
		OutParameterType = ELEventDelegateParameterType::Text;
		return true;
	}
	}

	return false;
}

UClass* ULEventDelegateParameterHelper::GetObjectParameterClass(const UFunction* InFunction)
{
	TFieldIterator<FProperty> paramsIterator(InFunction);
	FProperty* firstProperty = *paramsIterator;
	if (auto objProperty = CastField<FObjectProperty>(firstProperty))
	{
		return objProperty->PropertyClass;
	}
	return nullptr;
}

UEnum* ULEventDelegateParameterHelper::GetEnumParameter(const UFunction* InFunction)
{
	TFieldIterator<FProperty> paramsIterator(InFunction);
	FProperty* firstProperty = *paramsIterator;
	if (auto uint8Property = CastField<FByteProperty>(firstProperty))
	{
		if (uint8Property->IsEnum())
		{
			return uint8Property->Enum;
		}
	}
	if (auto enumProperty = CastField<FEnumProperty>(firstProperty))
	{
		return enumProperty->GetEnum();
	}
	return nullptr;
}
UClass* ULEventDelegateParameterHelper::GetClassParameterClass(const UFunction* InFunction)
{
	TFieldIterator<FProperty> paramsIterator(InFunction);
	FProperty* firstProperty = *paramsIterator;
	if (auto classProperty = CastField<FClassProperty>(firstProperty))
	{
		return classProperty->MetaClass;
	}
	return nullptr;
}

bool ULEventDelegateParameterHelper::IsSupportedFunction(UFunction* Target, ELEventDelegateParameterType& OutParamType)
{
	return IsFunctionCompatible(Target, OutParamType);
}

bool ULEventDelegateParameterHelper::IsStillSupported(UFunction* Target, ELEventDelegateParameterType InParamType)
{
	ELEventDelegateParameterType ParamType;
	if (IsSupportedFunction(Target, ParamType))
	{
		if (ParamType == InParamType)
		{
			return true;
		}
	}
	return false;
}

FString ULEventDelegateParameterHelper::ParameterTypeToName(ELEventDelegateParameterType paramType, const UFunction* InFunction)
{
	FString ParamTypeString = "";
	switch (paramType)
	{
	case ELEventDelegateParameterType::Empty:
		break;
	case ELEventDelegateParameterType::Bool:
		ParamTypeString = "Bool";
		break;
	case ELEventDelegateParameterType::Float:
		ParamTypeString = "Float";
		break;
	case ELEventDelegateParameterType::Double:
		ParamTypeString = "Double";
		break;
	case ELEventDelegateParameterType::Int8:
		ParamTypeString = "Int8";
		break;
	case ELEventDelegateParameterType::UInt8:
	{
		if (auto enumValue = GetEnumParameter(InFunction))
		{
			ParamTypeString = enumValue->GetName() + "(Enum)";
		}
		else
		{
			ParamTypeString = "UInt8";
		}
	}
		break;
	case ELEventDelegateParameterType::Int16:
		ParamTypeString = "Int16";
		break;
	case ELEventDelegateParameterType::UInt16:
		ParamTypeString = "UInt16";
		break;
	case ELEventDelegateParameterType::Int32:
		ParamTypeString = "Int32";
		break;
	case ELEventDelegateParameterType::UInt32:
		ParamTypeString = "UInt32";
		break;
	case ELEventDelegateParameterType::Int64:
		ParamTypeString = "Int64";
		break;
	case ELEventDelegateParameterType::UInt64:
		ParamTypeString = "UInt64";
		break;
	case ELEventDelegateParameterType::Vector2:
		ParamTypeString = "Vector2";
		break;
	case ELEventDelegateParameterType::Vector3:
		ParamTypeString = "Vector3";
		break;
	case ELEventDelegateParameterType::Vector4:
		ParamTypeString = "Vector4";
		break;
	case ELEventDelegateParameterType::Quaternion:
		ParamTypeString = "Quaternion";
		break;
	case ELEventDelegateParameterType::Color:
		ParamTypeString = "Color";
		break;
	case ELEventDelegateParameterType::LinearColor:
		ParamTypeString = "LinearColor";
		break;
	case ELEventDelegateParameterType::String:
		ParamTypeString = "String";
		break;

	case ELEventDelegateParameterType::Object:
	{
		TFieldIterator<FProperty> ParamIterator(InFunction);
		if (auto firstProperty = CastField<FObjectProperty>(*ParamIterator))
		{
			if (firstProperty->PropertyClass != UObject::StaticClass())
			{
				ParamTypeString = firstProperty->PropertyClass->GetName() + "(Object)";
			}
			else
			{
				ParamTypeString = "Object";
			}
		}
		else
		{
			ParamTypeString = "Object";
		}
	}
		break;
	case ELEventDelegateParameterType::Actor:
	{
		TFieldIterator<FProperty> ParamIterator(InFunction);
		if (auto firstProperty = CastField<FObjectProperty>(*ParamIterator))
		{
			if (firstProperty->PropertyClass != AActor::StaticClass())
			{
				ParamTypeString = firstProperty->PropertyClass->GetName() + "(Actor)";
			}
			else
			{
				ParamTypeString = "Actor";
			}
		}
		else
		{
			ParamTypeString = "Actor";
		}
	}
		break;
	case ELEventDelegateParameterType::PointerEvent_DEPRECATED:
		ParamTypeString = "PointerEvent";
		break;
	case ELEventDelegateParameterType::Class:
		ParamTypeString = "Class";
		break;
	case ELEventDelegateParameterType::Rotator:
		ParamTypeString = "Rotator";
		break;
	case ELEventDelegateParameterType::Name:
		ParamTypeString = "Name";
		break;
	case ELEventDelegateParameterType::Text:
		ParamTypeString = "Text";
		break;
	default:
		break;
	}
	return ParamTypeString;
}


#if WITH_EDITOR
#include "Editor.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Sound/SoundBase.h"
namespace LEventDelegateUtils
{
	//nodify some informations in editor
	void EditorNotification(FText NofityText, float ExpireDuration)
	{
		if (!IsValid(GEditor))return;
		FNotificationInfo Info(NofityText);
		Info.FadeInDuration = 0.1f;
		Info.FadeOutDuration = 0.5f;
		Info.ExpireDuration = ExpireDuration;
		Info.bUseSuccessFailIcons = false;
		Info.bUseLargeFont = false;
		Info.bFireAndForget = true;
		auto NotificationItem = FSlateNotificationManager::Get().AddNotification(Info);
		NotificationItem->SetCompletionState(SNotificationItem::CS_Success);
		NotificationItem->ExpireAndFadeout();

		auto CompileFailSound = LoadObject<USoundBase>(NULL, TEXT("/Engine/EditorSounds/Notifications/CompileFailed_Cue.CompileFailed_Cue"));
		GEditor->PlayEditorSound(CompileFailSound);
	}
}
#endif


void FLEventDelegateData::Execute()
{
	if (UseNativeParameter)
	{
		auto errMsg = LOCTEXT("NativeParameterError", "LEventDelegateData.Execute, If use NativeParameter, you must FireEvent with your own parameter!");
#if WITH_EDITOR
		LEventDelegateUtils::EditorNotification(errMsg, 10);
#endif
		UE_LOG(LGUI, Error, TEXT("[%s].%d %s"), ANSI_TO_TCHAR(__FUNCTION__), __LINE__, *errMsg.ToString());
		return;
	}
	if (ParamType == ELEventDelegateParameterType::None)
	{
		auto errMsg = LOCTEXT("NotValid", "LEventDelegateData.Execute, Not valid LEventDelegate.");
#if WITH_EDITOR
		LEventDelegateUtils::EditorNotification(errMsg, 10);
#endif
		UE_LOG(LGUI, Error, TEXT("[%s].%d %s"), ANSI_TO_TCHAR(__FUNCTION__), __LINE__, *errMsg.ToString());
		return;
	}
	
	FindAndExecute();
}
void FLEventDelegateData::Execute(void* InParam, ELEventDelegateParameterType InParameterType)
{
	if (ParamType == ELEventDelegateParameterType::None)
	{
		auto errMsg = LOCTEXT("NotValid", "LEventDelegateData.Execute, Not valid LEventDelegate.");
#if WITH_EDITOR
		LEventDelegateUtils::EditorNotification(errMsg, 10);
#endif
		UE_LOG(LGUI, Error, TEXT("[%s].%d %s"), ANSI_TO_TCHAR(__FUNCTION__), __LINE__, *errMsg.ToString());
		return;
	}

	if (UseNativeParameter)//should use native parameter (pass in param)
	{
		if (ParamType != InParameterType)//function's supported parameter is not equal to event's parameter
		{
			if (InParameterType == ELEventDelegateParameterType::Double && ParamType == ELEventDelegateParameterType::Float)
			{
				auto InValue = *((double*)InParam);
				auto ConvertValue = (float)InValue;
				InParam = &ConvertValue;
				auto errMsg = LOCTEXT("ParameterTypeNotEqual_DoubleToFloat", "LEventDelegateData.Execute, Parameter type not equal, LGUI will automatic convert it from double to float.");
#if WITH_EDITOR
				LEventDelegateUtils::EditorNotification(errMsg, 10);
#endif
				UE_LOG(LGUI, Error, TEXT("[%s].%d %s"), ANSI_TO_TCHAR(__FUNCTION__), __LINE__, *errMsg.ToString());
			}
			else if (InParameterType == ELEventDelegateParameterType::Float && ParamType == ELEventDelegateParameterType::Double)
			{
				auto InValue = *((float*)InParam);
				auto ConvertValue = (double)InValue;
				InParam = &ConvertValue;
				auto errMsg = LOCTEXT("ParameterTypeNotEqual_FloatToDouble", "LEventDelegateData.Execute, Parameter type not equal, LGUI will automatic convert it from float to double.");
#if WITH_EDITOR
				LEventDelegateUtils::EditorNotification(errMsg, 10);
#endif
				UE_LOG(LGUI, Error, TEXT("[%s].%d %s"), ANSI_TO_TCHAR(__FUNCTION__), __LINE__, *errMsg.ToString());
			}
			else
			{
				auto errMsg = LOCTEXT("ParameterTypeNotEqual", "LEventDelegateData.Execute, Parameter type not equal!");
#if WITH_EDITOR
				LEventDelegateUtils::EditorNotification(errMsg, 10);
#endif
				UE_LOG(LGUI, Error, TEXT("[%s].%d %s"), ANSI_TO_TCHAR(__FUNCTION__), __LINE__, *errMsg.ToString());
				return;
			}
		}
		FindAndExecute(InParam);
	}
	else
	{
		FindAndExecute();
	}
}

#if WITH_EDITOR
bool FLEventDelegateData::CheckFunctionParameter()const
{
	if (ParamType == ELEventDelegateParameterType::None)
	{
		return false;
	}

	auto TargetFunction = TargetObject->FindFunction(functionName);
	if (!TargetFunction)
	{
		return false;
	}
	if (!ULEventDelegateParameterHelper::IsStillSupported(TargetFunction, ParamType))
	{
		return false;
	}

	return true;
}
#endif

bool FLEventDelegateData::CheckTargetObject()
{
	if (IsValid(TargetObject))
	{
		return true;
	}
	else
	{
		if (IsValid(HelperActor))
		{
			if (IsValid(HelperClass))
			{
				if (HelperClass == AActor::StaticClass())
				{
					TargetObject = HelperActor;
				}
				else
				{
					TArray<UActorComponent*> Components;
					HelperActor->GetComponents(HelperClass, Components);
					if (Components.Num() == 1)
					{
						TargetObject = Components[0];
					}
					else if (Components.Num() > 1)
					{
						if (!HelperComponentName.IsNone())
						{
							for (auto& Comp : Components)
							{
								if (Comp->GetFName() == HelperComponentName)
								{
									TargetObject = Comp;
									return true;
								}
							}
							FString ActorName =
#if WITH_EDITOR
								HelperActor->GetActorLabel();
#else
								HelperActor->GetPathName();
#endif
							UE_LOG(LGUI, Error, TEXT("[%s].%d Can't find component of name '%s' on actor '%s'"), ANSI_TO_TCHAR(__FUNCTION__), __LINE__, *HelperComponentName.ToString(), *ActorName);
						}
					}
				}
			}
		}

		return IsValid(TargetObject);
	}
}
void FLEventDelegateData::FindAndExecute(void* ParamData)
{
	if (!CheckTargetObject())
	{
		auto errMsg = LOCTEXT("TargetObjectNotValid", "LEventDelegateData.FindAndExecute, Target object not valid!");
#if WITH_EDITOR
		LEventDelegateUtils::EditorNotification(errMsg, 10);
#endif
		UE_LOG(LGUI, Error, TEXT("[%s].%d %s"), ANSI_TO_TCHAR(__FUNCTION__), __LINE__, *errMsg.ToString());
		return;
	}
	if (CacheFunction == nullptr)
	{
		CacheFunction = TargetObject->FindFunction(functionName);
	}
	if (CacheFunction)
	{
		if (!ULEventDelegateParameterHelper::IsStillSupported(CacheFunction, ParamType))
		{
			auto errMsg = FText::Format(LOCTEXT("FunctionNotSupport", "LEventDelegateData.FindAndExecute, Target function: {0} not supported!"), FText::FromName(functionName));
#if WITH_EDITOR
			LEventDelegateUtils::EditorNotification(errMsg, 10);
#endif
			UE_LOG(LGUI, Error, TEXT("[%s].%d %s"), ANSI_TO_TCHAR(__FUNCTION__), __LINE__, *errMsg.ToString());
			CacheFunction = nullptr;
		}
		else
		{
			if (ParamData == nullptr)
			{
				switch (ParamType)
				{
				case ELEventDelegateParameterType::String:
				{
					FString TempString;
					auto FromBinary = FMemoryReader(ParamBuffer, false);
					FromBinary << TempString;
					TargetObject->ProcessEvent(CacheFunction, &TempString);
				}
				break;
				case ELEventDelegateParameterType::Name:
				{
					FName TempName;
					auto FromBinary = FMemoryReader(ParamBuffer, false);
					FromBinary << TempName;
					TargetObject->ProcessEvent(CacheFunction, &TempName);
				}
				break;
				case ELEventDelegateParameterType::Text:
				{
					FText TempText;
					auto FromBinary = FMemoryReader(ParamBuffer, false);
					FromBinary << TempText;
					TargetObject->ProcessEvent(CacheFunction, &TempText);
				}
				break;
				case ELEventDelegateParameterType::Object:
				case ELEventDelegateParameterType::Actor:
				case ELEventDelegateParameterType::Class:
				{
					TargetObject->ProcessEvent(CacheFunction, &ReferenceObject);
				}
				break;
				default:
				{
					TargetObject->ProcessEvent(CacheFunction, ParamBuffer.GetData());
				}
				break;
				}
			}
			else
			{
				//execute function with passin parameter
				TargetObject->ProcessEvent(CacheFunction, ParamData);
			}
		}
	}
	else
	{
		auto errMsg = FText::Format(LOCTEXT("FunctionNotExist", "LEventDelegateData.FindAndExecute, Target function: {0} not exist!"), FText::FromName(functionName));
#if WITH_EDITOR
		LEventDelegateUtils::EditorNotification(errMsg, 10);
#endif
		UE_LOG(LGUI, Error, TEXT("[%s].%d %s"), ANSI_TO_TCHAR(__FUNCTION__), __LINE__, *errMsg.ToString());
	}
}

FLEventDelegate::FLEventDelegate()
{
}
FLEventDelegate::FLEventDelegate(ELEventDelegateParameterType InParameterType)
{
	supportParameterType = InParameterType;
}

bool FLEventDelegate::IsBound()const
{
	return eventList.Num() != 0;
}
void FLEventDelegate::FireEvent()const
{
	if (eventList.Num() == 0)return;
	if (supportParameterType == ELEventDelegateParameterType::Empty)
	{
		for (auto& item : eventList)
		{
			item.Execute();
		}
	}
	else
		LogParameterError(ELEventDelegateParameterType::Empty);
}
void FLEventDelegate::LogParameterError(ELEventDelegateParameterType WrongParamType)const
{
	auto enumObject = FindObject<UEnum>(nullptr, TEXT("/Script/LGUI.ELEventDelegateParameterType"), true);
	auto errMsg = FText::Format(LOCTEXT("ParameterTypeMismatch", "LEventDelegate parameter type must be the same as your declaration. support parameter type: {0}, execute parameter type: {1}")
		, enumObject->GetDisplayNameTextByValue((int64)supportParameterType)
		, enumObject->GetDisplayNameTextByValue((int64)WrongParamType)
	);
#if WITH_EDITOR
	LEventDelegateUtils::EditorNotification(errMsg, 10);
#endif
	UE_LOG(LGUI, Error, TEXT("[%s].%d %s"), ANSI_TO_TCHAR(__FUNCTION__), __LINE__, *errMsg.ToString());
}
void FLEventDelegate::FireEvent(void* InParam)const
{
	for (auto& item : eventList)
	{
		item.Execute(InParam, supportParameterType);
	}
}

void FLEventDelegate::FireEvent(bool InParam)const
{
	if (eventList.Num() == 0)return;
	if (supportParameterType == ELEventDelegateParameterType::Bool)
	{
		FireEvent(&InParam);
	}
	else LogParameterError(ELEventDelegateParameterType::Bool);
}
void FLEventDelegate::FireEvent(float InParam)const
{
	if (eventList.Num() == 0)return;
	if (supportParameterType == ELEventDelegateParameterType::Float)
	{
		FireEvent(&InParam);
	}
	else LogParameterError(ELEventDelegateParameterType::Float);
}
void FLEventDelegate::FireEvent(double InParam)const
{
	if (eventList.Num() == 0)return;
	if (supportParameterType == ELEventDelegateParameterType::Double)
	{
		FireEvent(&InParam);
	}
	else LogParameterError(ELEventDelegateParameterType::Double);
}
void FLEventDelegate::FireEvent(int8 InParam)const
{
	if (eventList.Num() == 0)return;
	if (supportParameterType == ELEventDelegateParameterType::Int8)
	{
		FireEvent(&InParam);
	}
	else LogParameterError(ELEventDelegateParameterType::Int8);
}
void FLEventDelegate::FireEvent(uint8 InParam)const
{
	if (eventList.Num() == 0)return;
	if (supportParameterType == ELEventDelegateParameterType::UInt8)
	{
		FireEvent(&InParam);
	}
	else LogParameterError(ELEventDelegateParameterType::UInt8);
}
void FLEventDelegate::FireEvent(int16 InParam)const
{
	if (eventList.Num() == 0)return;
	if (supportParameterType == ELEventDelegateParameterType::Int16)
	{
		FireEvent(&InParam);
	}
	else LogParameterError(ELEventDelegateParameterType::Int16);
}
void FLEventDelegate::FireEvent(uint16 InParam)const
{
	if (eventList.Num() == 0)return;
	if (supportParameterType == ELEventDelegateParameterType::UInt16)
	{
		FireEvent(&InParam);
	}
	else LogParameterError(ELEventDelegateParameterType::UInt16);
}
void FLEventDelegate::FireEvent(int32 InParam)const
{
	if (eventList.Num() == 0)return;
	if (supportParameterType == ELEventDelegateParameterType::Int32)
	{
		FireEvent(&InParam);
	}
	else LogParameterError(ELEventDelegateParameterType::Int32);
}
void FLEventDelegate::FireEvent(uint32 InParam)const
{
	if (eventList.Num() == 0)return;
	if (supportParameterType == ELEventDelegateParameterType::UInt32)
	{
		FireEvent(&InParam);
	}
	else LogParameterError(ELEventDelegateParameterType::UInt32);
}
void FLEventDelegate::FireEvent(int64 InParam)const
{
	if (eventList.Num() == 0)return;
	if (supportParameterType == ELEventDelegateParameterType::Int64)
	{
		FireEvent(&InParam);
	}
	else LogParameterError(ELEventDelegateParameterType::Int64);
}
void FLEventDelegate::FireEvent(uint64 InParam)const
{
	if (eventList.Num() == 0)return;
	if (supportParameterType == ELEventDelegateParameterType::UInt64)
	{
		FireEvent(&InParam);
	}
	else LogParameterError(ELEventDelegateParameterType::UInt64);
}
void FLEventDelegate::FireEvent(FVector2D InParam)const
{
	if (eventList.Num() == 0)return;
	if (supportParameterType == ELEventDelegateParameterType::Vector2)
	{
		FireEvent(&InParam);
	}
	else LogParameterError(ELEventDelegateParameterType::Vector2);
}
void FLEventDelegate::FireEvent(FVector InParam)const
{
	if (eventList.Num() == 0)return;
	if (supportParameterType == ELEventDelegateParameterType::Vector3)
	{
		FireEvent(&InParam);
	}
	else LogParameterError(ELEventDelegateParameterType::Vector3);
}
void FLEventDelegate::FireEvent(FVector4 InParam)const
{
	if (eventList.Num() == 0)return;
	if (supportParameterType == ELEventDelegateParameterType::Vector4)
	{
		FireEvent(&InParam);
	}
	else LogParameterError(ELEventDelegateParameterType::Vector4);
}
void FLEventDelegate::FireEvent(FColor InParam)const
{
	if (eventList.Num() == 0)return;
	if (supportParameterType == ELEventDelegateParameterType::Color)
	{
		FireEvent(&InParam);
	}
	else LogParameterError(ELEventDelegateParameterType::Color);
}
void FLEventDelegate::FireEvent(FLinearColor InParam)const
{
	if (eventList.Num() == 0)return;
	if (supportParameterType == ELEventDelegateParameterType::LinearColor)
	{
		FireEvent(&InParam);
	}
	else LogParameterError(ELEventDelegateParameterType::LinearColor);
}
void FLEventDelegate::FireEvent(FQuat InParam)const
{
	if (eventList.Num() == 0)return;
	if (supportParameterType == ELEventDelegateParameterType::Quaternion)
	{
		FireEvent(&InParam);
	}
	else LogParameterError(ELEventDelegateParameterType::Quaternion);
}
void FLEventDelegate::FireEvent(const FString& InParam)const
{
	if (eventList.Num() == 0)return;
	if (supportParameterType == ELEventDelegateParameterType::String)
	{
		FireEvent((void*)&InParam);
	}
	else LogParameterError(ELEventDelegateParameterType::String);
}
void FLEventDelegate::FireEvent(UObject* InParam)const
{
	if (eventList.Num() == 0)return;
	if (supportParameterType == ELEventDelegateParameterType::Object)
	{
		FireEvent(&InParam);
	}
	else LogParameterError(ELEventDelegateParameterType::Object);
}
void FLEventDelegate::FireEvent(AActor* InParam)const
{
	if (eventList.Num() == 0)return;
	if (supportParameterType == ELEventDelegateParameterType::Actor)
	{
		FireEvent(&InParam);
	}
	else LogParameterError(ELEventDelegateParameterType::Actor);
}
// void FLEventDelegate::FireEvent(ULGUIPointerEventData* InParam)const
// {
// 	if (eventList.Num() == 0)return;
// 	if (supportParameterType == ELEventDelegateParameterType::PointerEvent)
// 	{
// 		FireEvent(&InParam);
// 	}
// 	else LogParameterError(ELEventDelegateParameterType::PointerEvent);
// }
void FLEventDelegate::FireEvent(UClass* InParam)const
{
	if (eventList.Num() == 0)return;
	if (supportParameterType == ELEventDelegateParameterType::Class)
	{
		FireEvent(&InParam);
	}
	else LogParameterError(ELEventDelegateParameterType::Class);
}
void FLEventDelegate::FireEvent(FRotator InParam)const
{
	if (eventList.Num() == 0)return;
	if (supportParameterType == ELEventDelegateParameterType::Rotator)
	{
		FireEvent(&InParam);
	}
	else LogParameterError(ELEventDelegateParameterType::Rotator);
}
void FLEventDelegate::FireEvent(const FName& InParam)const
{
	if (eventList.Num() == 0)return;
	if (supportParameterType == ELEventDelegateParameterType::Name)
	{
		FireEvent((void*)&InParam);
	}
	else LogParameterError(ELEventDelegateParameterType::Name);
}
void FLEventDelegate::FireEvent(const FText& InParam)const
{
	if (eventList.Num() == 0)return;
	if (supportParameterType == ELEventDelegateParameterType::Text)
	{
		FireEvent((void*)&InParam);
	}
	else LogParameterError(ELEventDelegateParameterType::Text);
}

#if WITH_EDITOR
bool FLEventDelegate::CheckFunctionParameter()const
{
	for (auto& item : eventList)
	{
		if (!item.CheckFunctionParameter())
		{
			return false;
		}
	}
	return true;
}
#endif

#undef LOCTEXT_NAMESPACE

