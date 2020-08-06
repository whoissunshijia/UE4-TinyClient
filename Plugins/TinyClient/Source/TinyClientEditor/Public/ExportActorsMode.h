// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "UObject/Object.h"
#include "UnrealEd.h"
#include "Editor.h"

/*
 * The class handling the editor mode of MapSync
 * Also contains most of the logic behind, there was no point in putting it inside another file
 * The structure of the sent data is [COMMAND][NAMESIZE][NAME][DATASIZE][DATA], and all modifications are concatenated. The command is 1 byte, the sizes are 4 bytes long
 * At the message's beginning, there is the level name, and the same of the string just before it: [LEVELNAMESIZE][LEVELNAME]
 */
class FExportActorsMode : public FEdMode
{
public:
	static const FEditorModeID EM_ExportActorsEdModeId; // Use for EdMode
	static bool bIsExportActorsSerialization;

public:
	// ctor, dtor
	FExportActorsMode();
	virtual ~FExportActorsMode();

	// Used for EdMode
	virtual void Enter() override; // Called when the editor enters this EdMode
	virtual void Exit() override; // Called when the editor exits this EdMode
	bool UsesToolkits() const override; // Requiered to use a toolkit, i.e. a GUI in the EdMode panel

	void ShowTookit(int32 iMode);
};