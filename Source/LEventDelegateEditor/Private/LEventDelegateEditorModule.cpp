// Copyright 2019-Present LexLiu. All Rights Reserved.

#include "LEventDelegateEditorModule.h"
#include "Modules/ModuleManager.h"
#include "LEventDelegateHeaders.h"
#include "LEventDelegateEditorStyle.h"
#include "DetailCustomization/LEventDelegateCustomization.h"
#include "DetailCustomization/LEventDelegatePresetParamCustomization.h"

#define LOCTEXT_NAMESPACE "FLEventDelegateEditorModule"
DEFINE_LOG_CATEGORY(LEventDelegateEditor);

void FLEventDelegateEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FLEventDelegateEditorStyle::Initialize();
	FLEventDelegateEditorStyle::ReloadTextures();

	//register custom editor
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		
		PropertyModule.RegisterCustomPropertyTypeLayout(FLEventDelegate::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FLEventDelegateCustomization::MakeInstance));
		//PropertyModule.RegisterCustomPropertyTypeLayout(FLEventDelegateTwoParam::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FLEventDelegateTwoParamCustomization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(FLEventDelegate_Empty::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&LEventDelegatePresetParamCustomization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(FLEventDelegate_Bool::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&LEventDelegatePresetParamCustomization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(FLEventDelegate_Float::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&LEventDelegatePresetParamCustomization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(FLEventDelegate_Double::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&LEventDelegatePresetParamCustomization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(FLEventDelegate_Int8::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&LEventDelegatePresetParamCustomization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(FLEventDelegate_UInt8::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&LEventDelegatePresetParamCustomization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(FLEventDelegate_Int16::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&LEventDelegatePresetParamCustomization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(FLEventDelegate_UInt16::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&LEventDelegatePresetParamCustomization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(FLEventDelegate_Int32::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&LEventDelegatePresetParamCustomization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(FLEventDelegate_UInt32::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&LEventDelegatePresetParamCustomization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(FLEventDelegate_Int64::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&LEventDelegatePresetParamCustomization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(FLEventDelegate_UInt64::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&LEventDelegatePresetParamCustomization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(FLEventDelegate_Vector2::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&LEventDelegatePresetParamCustomization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(FLEventDelegate_Vector3::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&LEventDelegatePresetParamCustomization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(FLEventDelegate_Vector4::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&LEventDelegatePresetParamCustomization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(FLEventDelegate_Color::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&LEventDelegatePresetParamCustomization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(FLEventDelegate_LinearColor::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&LEventDelegatePresetParamCustomization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(FLEventDelegate_Quaternion::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&LEventDelegatePresetParamCustomization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(FLEventDelegate_String::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&LEventDelegatePresetParamCustomization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(FLEventDelegate_Object::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&LEventDelegatePresetParamCustomization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(FLEventDelegate_Actor::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&LEventDelegatePresetParamCustomization::MakeInstance));
		// PropertyModule.RegisterCustomPropertyTypeLayout(FLEventDelegate_PointerEvent::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&LEventDelegatePresetParamCustomization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(FLEventDelegate_Class::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&LEventDelegatePresetParamCustomization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(FLEventDelegate_Rotator::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&LEventDelegatePresetParamCustomization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(FLEventDelegate_Text::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&LEventDelegatePresetParamCustomization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(FLEventDelegate_Name::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&LEventDelegatePresetParamCustomization::MakeInstance));
	}
}

void FLEventDelegateEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FLEventDelegateEditorStyle::Shutdown();

	//unregister custom editor
	if (UObjectInitialized() && FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

		PropertyModule.UnregisterCustomPropertyTypeLayout(FLEventDelegate::StaticStruct()->GetFName());
		//PropertyModule.UnregisterCustomPropertyTypeLayout(FLEventDelegateTwoParam::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomPropertyTypeLayout(FLEventDelegate_Empty::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomPropertyTypeLayout(FLEventDelegate_Bool::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomPropertyTypeLayout(FLEventDelegate_Float::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomPropertyTypeLayout(FLEventDelegate_Double::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomPropertyTypeLayout(FLEventDelegate_Int8::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomPropertyTypeLayout(FLEventDelegate_UInt8::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomPropertyTypeLayout(FLEventDelegate_Int16::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomPropertyTypeLayout(FLEventDelegate_UInt16::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomPropertyTypeLayout(FLEventDelegate_Int32::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomPropertyTypeLayout(FLEventDelegate_UInt32::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomPropertyTypeLayout(FLEventDelegate_Int64::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomPropertyTypeLayout(FLEventDelegate_UInt64::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomPropertyTypeLayout(FLEventDelegate_Vector2::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomPropertyTypeLayout(FLEventDelegate_Vector3::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomPropertyTypeLayout(FLEventDelegate_Vector4::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomPropertyTypeLayout(FLEventDelegate_Color::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomPropertyTypeLayout(FLEventDelegate_LinearColor::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomPropertyTypeLayout(FLEventDelegate_Quaternion::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomPropertyTypeLayout(FLEventDelegate_String::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomPropertyTypeLayout(FLEventDelegate_Object::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomPropertyTypeLayout(FLEventDelegate_Actor::StaticStruct()->GetFName());
		// PropertyModule.UnregisterCustomPropertyTypeLayout(FLEventDelegate_PointerEvent::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomPropertyTypeLayout(FLEventDelegate_Class::StaticStruct()->GetFName());
		PropertyModule.UnregisterCustomPropertyTypeLayout(FLEventDelegate_Rotator::StaticStruct()->GetFName());
	}
}

IMPLEMENT_MODULE(FLEventDelegateEditorModule, LEventDelegateEditor)

#undef LOCTEXT_NAMESPACE