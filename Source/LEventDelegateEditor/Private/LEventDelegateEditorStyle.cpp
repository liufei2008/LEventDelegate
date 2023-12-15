// Copyright 2019-Present LexLiu. All Rights Reserved.

#include "LEventDelegateEditorStyle.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

TSharedPtr< FSlateStyleSet > FLEventDelegateEditorStyle::StyleInstance = NULL;

void FLEventDelegateEditorStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FLEventDelegateEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FLEventDelegateEditorStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("LEventDelegateEditorStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef< FSlateStyleSet > FLEventDelegateEditorStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("LEventDelegateEditorStyle"));
	Style->SetContentRoot((IPluginManager::Get().FindPlugin("LGUI") != nullptr ? IPluginManager::Get().FindPlugin("LGUI") : IPluginManager::Get().FindPlugin("LEventDelegate"))->GetBaseDir() / TEXT("Resources/Icons"));;

	Style->Set("LEventDelegateEditor.EventGroup", new BOX_BRUSH(TEXT("EventGroup"), FMargin(15.0f / 30.0f, 34.0f / 40.0f, 15.0f / 30.0f, 6.0f / 40.0f)));
	Style->Set("LEventDelegateEditor.EventItem", new BOX_BRUSH(TEXT("EventItem"), FVector2D(26, 26), 12.0f / 26.0f));

	FButtonStyle AnchorButton = FButtonStyle()
		.SetNormal(BOX_BRUSH(TEXT("AnchorData_Button_Normal"), FVector2D(16, 16), 4.0f / 16.0f))
		.SetDisabled(BOX_BRUSH(TEXT("AnchorData_Button_Normal"), FVector2D(16, 16), 4.0f / 16.0f))
		.SetHovered(BOX_BRUSH(TEXT("WhiteFrameHover_1x"), FVector2D(16, 16), 4.0f / 16.0f))
		.SetPressed(BOX_BRUSH(TEXT("WhiteFramePress_1x"), FVector2D(16, 16), 4.0f / 16.0f));
	Style->Set("AnchorButton", AnchorButton);

	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void FLEventDelegateEditorStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FLEventDelegateEditorStyle::Get()
{
	return *StyleInstance;
}
