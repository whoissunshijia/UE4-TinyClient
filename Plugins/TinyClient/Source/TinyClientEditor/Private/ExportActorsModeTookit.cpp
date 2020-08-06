#include "ExportActorsModeTookit.h"
#include "ExportActorsMode.h"
#include "ExportActors.h"
#include "AssetRegistryModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "SEditableText.h"
#include "Private/AssetRegistry.h"
#include "AssetDeleteModel.h"

//all export actors in current level
static TMap<FString, AActor*> AllExportActors;

//information(dependencies) of export acto
static TMap<FString, TArray<FName>> ExportActorsInfo;


FExportActorsModeTookit::FExportActorsModeTookit(int32 iMode)
{
	TMap<int32, TFunction<void(FExportActorsModeTookit*)>> PanelMap;
	PanelMap.Add(0, &FExportActorsModeTookit::ShowExportActorsPanel);
	if (PanelMap.Contains(iMode))
	{
		(*PanelMap.Find(iMode))(this);
	}
}

FName FExportActorsModeTookit::GetToolkitFName() const
{
	return FName(TEXT("ExportActorsMode"));
}

FText FExportActorsModeTookit::GetBaseToolkitName() const
{
	return NSLOCTEXT("ExportActorsModeTookit", "DisplayName", "ExportActorsMode Tool");
}

class FEdMode* FExportActorsModeTookit::GetEditorMode() const
{
	return GLevelEditorModeTools().GetActiveMode(FExportActorsMode::EM_ExportActorsEdModeId);
}

namespace
{
	//FString PackagePath = FString("/Game/Dev/TestExprot");

	static void GetAllExportActor(ULevel* CurrentLevel, UExportActors* ExportActor, UPackage* ExportPackage)
	{
		//collert all export actors
		for (int i = 0; i < CurrentLevel->Actors.Num(); ++i)
		{
			AActor* a = (CurrentLevel->Actors[i]);
			if (a)
			{
				if (a->ActorHasTag(TEXT("Export")))
				{
					AActor* ExActor = (AActor*)(StaticDuplicateObject(a, ExportActor));
					AllExportActors.Add(ExActor->GetName(), ExActor);
					ExportActor->CollertActors.Add(ExActor);
				}
			}
		}

		//if export actor has other ref(actor), replace the ref
		for (auto acotr : AllExportActors)
		{
			FArchiveExport FAS;
			FAS.SetIsSaving(true);
			FAS.SetIsPersistent(true);
			FAS.ArIsObjectReferenceCollector = true;
			FAS.ArShouldSkipBulkData = true;
			FAS.CurrentSerializeUObject = acotr.Value;
			FAS.CurrentLevel = CurrentLevel;
			FAS.AllExportActors = &AllExportActors;
			acotr.Value->Serialize(FAS);
		}

		//create temp asset for each
		for (auto acotr : AllExportActors)
		{		
			FString TempPath("/Game/TempAsset");
			FString TempAssetName = FPaths::GetBaseFilename(TempPath);
			UPackage *TempPackage = CreatePackage(nullptr, *TempPath);
			if (TempPackage)
			{
				UTempActor* TempExportAsset = NewObject<UTempActor>(TempPackage, UTempActor::StaticClass(), *TempAssetName, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone);
				AActor* TempActor = (AActor*)(StaticDuplicateObject(acotr.Value, TempExportAsset));

				FArchiveTempCopy FAS;
				FAS.SetIsSaving(true);
				FAS.SetIsPersistent(true);
				FAS.ArIsObjectReferenceCollector = true;
				FAS.ArShouldSkipBulkData = true;
				FAS.CurrentSerializeUObject.Push(TempActor);
				FAS.TempOuter = TempExportAsset;
				FAS.ExportOuter = ExportActor;
				TempActor->Serialize(FAS);

				TempExportAsset->TempPtr = TempActor;
				TempPackage->FullyLoad();
				//uasset path
				FString AssetDiskPath = FPackageName::LongPackageNameToFilename(TempPath, *FPackageName::GetAssetPackageExtension());
				bool bSuccess = UPackage::SavePackage(
					TempPackage,
					TempExportAsset,
					EObjectFlags::RF_Public | EObjectFlags::RF_Standalone,
					*AssetDiskPath
				);

				//create temp asset
				TArray<FString> UAssetFile;
				UAssetFile.Add(AssetDiskPath);

				static FName DirectoryWatcherName("AssetRegistry");
				FAssetRegistryModule& DirectoryWatcherModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(DirectoryWatcherName);
				DirectoryWatcherModule.Get().ScanModifiedAssetFiles(UAssetFile);
				

				//get all dependencies
				FAssetRegistryModule& AssetRegistryModule = FModuleManager::GetModuleChecked<FAssetRegistryModule>("AssetRegistry");
				TArray<FName> dependencies;
				AssetRegistryModule.Get().GetDependencies(*TempPath, dependencies);
				ExportActorsInfo.Add(TempActor->GetName(), dependencies);

				//deltet temp asset
				TArray<UObject*> DeletTempActor;
				DeletTempActor.Add(TempExportAsset);
				FAssetDeleteModel deleTemp(DeletTempActor);
				deleTemp.DoDelete();
			}
		}
	}

	static void DoExportActors(TSharedPtr<SEditableText>& ExportPath)
	{
		UWorld* CurrentPIEWorld = GEditor->GetEditorWorldContext().World();
		if (CurrentPIEWorld)
		{
			ULevel* CurrentLevel = CurrentPIEWorld->GetCurrentLevel();
			if (CurrentLevel)
			{
				TArray<AActor*> CollertActors = (CurrentLevel->Actors);
				FText ExportPackagePath = ExportPath->GetText();
				//if Package exist , delete
				UObject* p = (UObject*)FindPackage(nullptr, *ExportPackagePath.ToString());
				if (p)
				{
					TArray<UObject*> DeletTempActor;
					DeletTempActor.Add(p);
					FAssetDeleteModel deleTemp(DeletTempActor);
					deleTemp.DoDelete();
				}

				FString AssetName = FPaths::GetBaseFilename(ExportPackagePath.ToString());
				UPackage *Package = CreatePackage(nullptr, *ExportPackagePath.ToString());
				if (Package)
				{
					UExportActors* ExportAsset = NewObject<UExportActors>(Package, UExportActors::StaticClass(), *AssetName, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone);
					GetAllExportActor(CurrentLevel, ExportAsset, Package);
					Package->FullyLoad();
					//uasset path
					FString AssetDiskPath = FPackageName::LongPackageNameToFilename(ExportPackagePath.ToString(), *FPackageName::GetAssetPackageExtension());
					bool bSuccess = UPackage::SavePackage(
						Package,
						ExportAsset,
						EObjectFlags::RF_Public | EObjectFlags::RF_Standalone,
						*AssetDiskPath
					);

					UE_LOG(LogTemp, Log, TEXT("Saved Package: %s"), bSuccess ? TEXT("True") : TEXT("False"));
					AllExportActors.Empty();
					ExportActorsInfo.Empty();
					return;
				}
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("Create faile because unkonw error :("));
	}
}

void FExportActorsModeTookit::ShowExportActorsPanel()
{
	SAssignNew(ToolkitWidget, SBorder)
		.VAlign(VAlign_Top)
		.Padding(FMargin(10.f, 0.f))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Left)
		.AutoHeight()
		.Padding(FMargin(0.f, 10.f))
		[
			SNew(SBox)
			.WidthOverride(400.f)
		.HeightOverride(30.f)
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			SNew(SBorder)
			[
				SAssignNew(ExportActorPathWidet, SEditableText)
				.Font(FSlateFontInfo("../../../Engine/Content/Slate/Fonts/DroidSansFallback.ttf", 13))
		.Text(FText::FromString("Export"))
			]
		]
		]
	+ SVerticalBox::Slot()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		.AutoHeight()
		.Padding(FMargin(0.f, 10.f))
		[
			SNew(SButton)
			.Text(FText::FromString("ExportActors"))
		.OnClicked_Lambda([&]()
	{
		DoExportActors(ExportActorPathWidet);
		return FReply::Handled();
	})
		]
		];
}
