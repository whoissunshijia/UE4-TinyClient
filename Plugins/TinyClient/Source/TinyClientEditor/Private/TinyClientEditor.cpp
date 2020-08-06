// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "TinyClientEditor.h"
#include "EditorModeRegistry.h"
#include "ExportActorsMode.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "FTinyClientEditorModule"

void FTinyClientEditorModule::StartupModule()
{
	FEditorModeRegistry::Get().RegisterMode<FExportActorsMode>(FExportActorsMode::EM_ExportActorsEdModeId, LOCTEXT("ExportActorsModeName", "ExportActorsMode"), FSlateIcon(FEditorStyle::GetStyleSetName(), "DeviceDetails.Share"), true);

}

void FTinyClientEditorModule::ShutdownModule()
{
	FEditorModeRegistry::Get().UnregisterMode(FExportActorsMode::EM_ExportActorsEdModeId);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTinyClientEditorModule, TinyClientEditor)