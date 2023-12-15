// Copyright 2019-Present LexLiu. All Rights Reserved.

#pragma once

#include "Modules/ModuleInterface.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"

DECLARE_LOG_CATEGORY_EXTERN(LEventDelegateEditor, Log, All);

class FLEventDelegateEditorModule : public IModuleInterface
{
public:


	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

};