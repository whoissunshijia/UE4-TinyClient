// Fill out your copyright notice in the Description page of Project Settings.


#include "ExportActors.h"
#include "ReleaseObjectVersion.h"
#include "RenderingObjectVersion.h"

TArray<AActor*> UExportActors::GlobalCollertActors;

FArchive& FArchiveExport::operator<<(UObject*& Obj)
{
	if (Obj)
	{
		bool bIsNative = Obj->IsNative();
		if (!Obj->HasAnyFlags(RF_Transient) || bIsNative)
		{
			const bool bIsTopLevelPackage = Obj->GetOuter() == nullptr && dynamic_cast<UPackage*>(Obj);
			UObject* Outer = Obj->GetOuter();
			while (!bIsNative && Outer)
			{
				if (dynamic_cast<UClass*>(Outer) && Outer->IsNative())
				{
					bIsNative = true;
				}
				Outer = Outer->GetOuter();
			}

			if (!bIsTopLevelPackage)
			{
				if (bIsNative)
					return *this;
				else
				{
					if (Obj->GetOuter() == (UObject*)CurrentLevel)
					{
						UObjectProperty* Property = (UObjectProperty*)this->GetSerializedProperty();
						uint8* DataPtr = Property->ContainerPtrToValuePtr<uint8>(CurrentSerializeUObject, 0);
						FString Name = Obj->GetName();
						UObject** Value = (UObject**)(AllExportActors->Find(Name));
						if (Value)
						{
							UObjectProperty* l = (UObjectProperty*)((uint8*)CurrentSerializeUObject + Property->GetOffset_ForInternal());
							UObjectProperty::SetPropertyValue(l, *Value);
						}
						else
							//ref no tag(export)
							check(false);
					}
				}
			}
		}
	}
	return *this;
}

FArchive& FArchiveSpawn::operator<<(UObject*& Obj)
{
	if (Obj)
	{
		bool bIsNative = Obj->IsNative();
		if (!Obj->HasAnyFlags(RF_Transient) || bIsNative)
		{
			const bool bIsTopLevelPackage = Obj->GetOuter() == nullptr && dynamic_cast<UPackage*>(Obj);
			UObject* Outer = Obj->GetOuter();
			while (!bIsNative && Outer)
			{
				if (dynamic_cast<UClass*>(Outer) && Outer->IsNative())
				{
					bIsNative = true;
				}
				Outer = Outer->GetOuter();
			}

			if (!bIsTopLevelPackage)
			{
				if (bIsNative)
					return *this;
				else
				{
					if (Obj->GetOuter() != CurrentLevel)
					{
						FString ObjName = Obj->GetName();
						UObject** CheckActorInLevel = (UObject**)InLevelExportActors->Find(ObjName);
						if (CheckActorInLevel)
						{							
							UObjectProperty* Property = (UObjectProperty*)this->GetSerializedProperty();
							uint8* DataPtr = Property->ContainerPtrToValuePtr<uint8>(CurrentSerializeUObject[0], 0);
							UObjectProperty* l = (UObjectProperty*)((uint8*)CurrentSerializeUObject[0] + Property->GetOffset_ForInternal());
							UObjectProperty::SetPropertyValue(l, *CheckActorInLevel);

							CurrentSerializeUObject.Insert(*CheckActorInLevel, 0);
							(*CheckActorInLevel)->Serialize(*this);
							CurrentSerializeUObject.Pop();
						}
					}				
				}
			}
		}
	}
	return *this;
}

FArchive& FArchiveTempCopy::operator<<(UObject*& Obj)
{
	if (Obj)
	{
		bool bIsNative = Obj->IsNative();
		if (!Obj->HasAnyFlags(RF_Transient) || bIsNative)
		{
			const bool bIsTopLevelPackage = Obj->GetOuter() == nullptr && dynamic_cast<UPackage*>(Obj);
			UObject* Outer = Obj->GetOuter();
			while (!bIsNative && Outer)
			{
				if (dynamic_cast<UClass*>(Outer) && Outer->IsNative())
				{
					bIsNative = true;
				}
				Outer = Outer->GetOuter();
			}

			if (!bIsTopLevelPackage)
			{
				if (bIsNative)
					return *this;
				else
				{
					if (Obj->GetOuter() == ExportOuter)
					{
						UObjectProperty* Property = (UObjectProperty*)this->GetSerializedProperty();
						uint8* DataPtr = Property->ContainerPtrToValuePtr<uint8>(CurrentSerializeUObject[0], 0);
						UObject* IsExist = FindObject<UObject>(TempOuter, *Obj->GetName());
						if(IsExist)
						{
							UObjectProperty* l = (UObjectProperty*)((uint8*)CurrentSerializeUObject[0] + Property->GetOffset_ForInternal());
							UObjectProperty::SetPropertyValue(l, IsExist);
							return *this;
						}
						else
						{
							UObject* TempObject = (UObject*)(StaticDuplicateObject(Obj, TempOuter));
							UObjectProperty* l = (UObjectProperty*)((uint8*)CurrentSerializeUObject[0] + Property->GetOffset_ForInternal());
							UObjectProperty::SetPropertyValue(l, TempObject);

							CurrentSerializeUObject.Insert(TempObject, 0);
							TempObject->Serialize(*this);
							CurrentSerializeUObject.Pop();
							
						}				
					}
				}
			}
		}
	}
	return *this;
}

UExportActors::UExportActors()
{

}

UExportActors::~UExportActors()
{
}

void UExportActors::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	Ar << CollertActors;
	GlobalCollertActors= CollertActors;
}



UTempActor::UTempActor()
{

}

UTempActor::~UTempActor()
{

}

void UTempActor::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	Ar << TempPtr;
}
