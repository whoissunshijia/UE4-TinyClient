// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine\EngineBaseTypes.h"
#include "ExportActors.generated.h"

/**
 * 
 */
class TINYCLIENT_API FArchiveExport : public FArchiveUObject
{
public:
	virtual FArchive& operator<<(UObject*& Obj) override;
	UObject* CurrentSerializeUObject;
	ULevel* CurrentLevel;
	TMap<FString, AActor*>* AllExportActors;
};

class TINYCLIENT_API FArchiveSpawn : public FArchiveUObject
{
public:
	virtual FArchive& operator<<(UObject*& Obj) override;
	TArray<UObject*> CurrentSerializeUObject;
	UObject* CurrentLevel;
	TMap<FString, AActor*>* InLevelExportActors;
};

class TINYCLIENT_API FArchiveTempCopy : public FArchiveUObject
{
public:
	virtual FArchive& operator<<(UObject*& Obj) override;
	UObject* TempOuter;
	TArray<UObject*> CurrentSerializeUObject;
	UObject* ExportOuter;
};


UCLASS()
class TINYCLIENT_API UTempActor :public UObject
{
	GENERATED_BODY()
public:

	UTempActor();
	~UTempActor();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "TextAsset")
	UObject* TempPtr;
	virtual void Serialize(FArchive& Ar) override;
};

UCLASS()
class TINYCLIENT_API UExportActors:public UObject
{
	 GENERATED_BODY()
public:

	UExportActors();
	~UExportActors();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "TextAsset")
	TArray<AActor*> CollertActors;
	virtual void Serialize(FArchive& Ar) override;
	static TArray<AActor*> GlobalCollertActors;
};

