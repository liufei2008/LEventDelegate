﻿// Copyright 2019-Present LexLiu. All Rights Reserved.

#pragma once
#include "Stats/Stats.h"
#include "Modules/ModuleInterface.h"

LEVENTDELEGATE_API DECLARE_LOG_CATEGORY_EXTERN(LGUI, Log, All);
DECLARE_STATS_GROUP(TEXT("LGUI"), STATGROUP_LGUI, STATCAT_Advanced);

//prevent compile optimization for easier code debug
#define LGUI_CAN_DISABLE_OPTIMIZATION 0

class FLEventDelegateModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
