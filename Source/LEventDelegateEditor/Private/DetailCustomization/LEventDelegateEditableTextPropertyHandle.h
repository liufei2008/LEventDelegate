﻿// Copyright 2019-Present LexLiu. All Rights Reserved.

#include "CoreMinimal.h"
#include "STextPropertyEditableTextBox.h"
#include "PropertyHandle.h"
#include "DetailWidgetRow.h"
#include "IPropertyUtilities.h"
#pragma once

/** Allows STextPropertyEditableTextBox to edit a property handle */
class FLEventDelegateEditableTextPropertyHandle : public IEditableTextProperty
{
public:
	FLEventDelegateEditableTextPropertyHandle(const TSharedRef<IPropertyHandle>& InPropertyHandle, const TSharedPtr<IPropertyUtilities>& InPropertyUtilities)
		: PropertyHandle(InPropertyHandle)
		, PropertyUtilities(InPropertyUtilities)
	{
	}

	virtual bool IsMultiLineText() const override
	{
		return PropertyHandle->IsValidHandle() && PropertyHandle->GetMetaDataProperty()->GetBoolMetaData("MultiLine");
	}

	virtual bool IsPassword() const override
	{
		return PropertyHandle->IsValidHandle() && PropertyHandle->GetMetaDataProperty()->GetBoolMetaData("PasswordField");
	}

	virtual bool IsReadOnly() const override
	{
		return !PropertyHandle->IsValidHandle() || PropertyHandle->IsEditConst();
	}

	virtual bool IsDefaultValue() const override
	{
		return PropertyHandle->IsValidHandle() && !PropertyHandle->DiffersFromDefault();
	}

	virtual FText GetToolTipText() const override
	{
		return (PropertyHandle->IsValidHandle())
			? PropertyHandle->GetToolTipText()
			: FText::GetEmpty();
	}

	virtual int32 GetNumTexts() const override
	{
		return (PropertyHandle->IsValidHandle())
			? PropertyHandle->GetNumPerObjectValues()
			: 0;
	}

	virtual FText GetText(const int32 InIndex) const override
	{
		if (PropertyHandle->IsValidHandle())
		{
			FString ObjectValue;
			if (PropertyHandle->GetPerObjectValue(InIndex, ObjectValue) == FPropertyAccess::Success)
			{
				FText TextValue;
				if (FTextStringHelper::ReadFromBuffer(*ObjectValue, TextValue))
				{
					return TextValue;
				}
			}
		}

		return FText::GetEmpty();
	}

	virtual void SetText(const int32 InIndex, const FText& InText) override
	{
		if (PropertyHandle->IsValidHandle())
		{
			FString ObjectValue;
			FTextStringHelper::WriteToBuffer(ObjectValue, InText);
			PropertyHandle->SetPerObjectValue(InIndex, ObjectValue);
		}
	}

	virtual bool IsValidText(const FText& InText, FText& OutErrorMsg) const override
	{
		return true;
	}

#if USE_STABLE_LOCALIZATION_KEYS
	virtual void GetStableTextId(const int32 InIndex, const ETextPropertyEditAction InEditAction, const FString& InTextSource, const FString& InProposedNamespace, const FString& InProposedKey, FString& OutStableNamespace, FString& OutStableKey) const override
	{
		if (PropertyHandle->IsValidHandle())
		{
			TArray<UPackage*> PropertyPackages;
			PropertyHandle->GetOuterPackages(PropertyPackages);

			check(PropertyPackages.IsValidIndex(InIndex));

			StaticStableTextId(PropertyPackages[InIndex], InEditAction, InTextSource, InProposedNamespace, InProposedKey, OutStableNamespace, OutStableKey);
		}
	}
#endif // USE_STABLE_LOCALIZATION_KEYS

private:
	TSharedRef<IPropertyHandle> PropertyHandle;
	TSharedPtr<IPropertyUtilities> PropertyUtilities;
};