// Copyright 2019-Present LexLiu. All Rights Reserved.
#include "LEventDelegateCustomization.h"

#pragma once

/**
 * 
 */
class LEventDelegatePresetParamCustomization : public FLEventDelegateCustomization
{
private:
	LEventDelegatePresetParamCustomization() :FLEventDelegateCustomization(false) {}
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShareable(new LEventDelegatePresetParamCustomization());
	}
};
