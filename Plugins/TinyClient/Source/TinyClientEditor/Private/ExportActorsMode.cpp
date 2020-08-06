#include "ExportActorsMode.h"
#include "ExportActorsModeTookit.h"
#include "Toolkits/ToolkitManager.h"

const FEditorModeID FExportActorsMode::EM_ExportActorsEdModeId = TEXT("EM_ExportActors");

bool FExportActorsMode::bIsExportActorsSerialization = false;

FExportActorsMode::FExportActorsMode()
{

}

FExportActorsMode::~FExportActorsMode()
{

}

void FExportActorsMode::Enter()
{
	FEdMode::Enter();

	if (!Toolkit.IsValid() && UsesToolkits())
	{
		ShowTookit(0);
	}
}

void FExportActorsMode::Exit()
{
	if (Toolkit.IsValid())
	{
		FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
		Toolkit.Reset();
	}

	// Call base Exit method to ensure proper cleanup
	FEdMode::Exit();
}

bool FExportActorsMode::UsesToolkits() const
{
	return true;
}

void FExportActorsMode::ShowTookit(int32 iMode)
{
	Toolkit = MakeShareable(new FExportActorsModeTookit(iMode));
	Toolkit->Init(Owner->GetToolkitHost());
	static_cast<FExportActorsModeTookit*>(Toolkit.Get())->OwnerEdMode = this;
}
