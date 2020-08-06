// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Toolkits/BaseToolkit.h"

class FExportActorsMode;
/**
 * 
 */
class FExportActorsModeTookit : public FModeToolkit
{
public:

	FExportActorsModeTookit(int32 iMode);

	/** IToolkit interface */
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual class FEdMode* GetEditorMode() const override;
	virtual TSharedPtr<SWidget> GetInlineContent() const override { return ToolkitWidget; }

	void ShowExportActorsPanel();
	FExportActorsMode* OwnerEdMode;

private:

	TSharedPtr<SWidget> ToolkitWidget;
	TSharedPtr<SEditableText> ExportActorPathWidet;
};