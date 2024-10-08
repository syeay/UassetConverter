// Fill out your copyright notice in the Description page of Project Settings.


#include "ConvertWidget.h"
//Widget includes
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ComboBoxString.h"
#include "Components/HorizontalBox.h"
//File picker includes
#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
//Object Convert Func includes
#include "AssetRegistry/AssetRegistryModule.h"
#include "HAL/PlatformFilemanager.h"
#include "Exporters/Exporter.h"
#include "AssetExportTask.h"

void UConvertWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitComboBox();
	btn_OpenFile->OnClicked.AddDynamic(this, &UConvertWidget::AddConvertingFile);
	btn_Convert->OnClicked.AddDynamic(this, &UConvertWidget::Convert);
	btn_Setting->OnClicked.AddDynamic(this, &UConvertWidget::OnOffSettingPanel);
}

void UConvertWidget::InitComboBox()
{
	SetComboOptions(cb_Image, ImageExtension, EObjectType::Image);
	SetComboOptions(cb_Audio, AudioExtension, EObjectType::Audio);
	SetComboOptions(cb_SkeletalMesh, SkeletalMeshExtension, EObjectType::SkeletalMesh);
	SetComboOptions(cb_StaticMesh, StaticMeshExtension, EObjectType::StaticMesh);
}

void UConvertWidget::SetComboOptions(UComboBoxString* TargetCombo, const TArray<FString>& TargetExtensions, EObjectType Type)
{
	for (FString Extension : TargetExtensions) {
		TargetCombo->AddOption(*Extension);
	}

	TargetCombo->SetSelectedOption(*TargetExtensions[0]);
	FTypeExtension Temp(TargetCombo, Type);
	TypeExtensions.Add(Temp);
}

void UConvertWidget::EndOfSetting()
{
	for (auto Struct : TypeExtensions) {
		Struct.ExtensionIndex = Struct.Combo->GetSelectedIndex();
	}
}


// 00_Product/03_UI/00_Tex/air/guide
FString UConvertWidget::GetGamePath(const FString& FilePath)
{
	int32 Index = FilePath.Find(TEXT("Content/"));
	if (Index == INDEX_NONE) return "";

	FString RetValue = FilePath.RightChop(Index + FString("Content/").Len());
	RetValue = RetValue.Replace(TEXT(".uasset"), TEXT(""));

	UE_LOG(LogTemp, Warning, TEXT("%s"), *RetValue);

	return RetValue;
}

// guide
FString UConvertWidget::GetFileName(const FString& FilePath)
{
	int32 Index = FilePath.Find(TEXT(".uasset"));
	if (Index == INDEX_NONE) return "";

	int32 NameIndex = 0;
	for (int32 i = Index; i > 0; --i) {
		if (FilePath[i] == '/') {
			NameIndex = i;
			break;
		}
	}

	FString RetValue = FilePath.RightChop(NameIndex + 1);
	RetValue = RetValue.Replace(TEXT(".uasset"), TEXT(""));

	UE_LOG(LogTemp, Warning, TEXT("%s"), *RetValue);

	return RetValue;
}

TArray<UObject*> UConvertWidget::GetObjectsByPath()
{
	FAssetRegistryModule& ARM = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry"));
	IAssetRegistry& AR = ARM.Get();

	TArray<FAssetData> AssetDatas;
	FString FilePathName = TEXT("/Game/01_SourceFile");
	AR.GetAssetsByPath(*FilePathName, AssetDatas);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *FilePathName);

	if (AssetDatas.Num() > 0) {
		TArray<UObject*> RetValue;
		for (int32 i = 0; i < AssetDatas.Num(); i++) {
			RetValue.Add(AssetDatas[i].GetAsset());
		}
		return RetValue;
	}
	else
		return TArray<UObject*>();
}

FString UConvertWidget::GetObjectExtension(UObject* Target)
{
	if (Cast<UTexture>(Target)) {
		return ImageExtension[TypeExtensions[0].ExtensionIndex];
	}
	else if (Cast<USoundBase>(Target)) {
		return AudioExtension[TypeExtensions[1].ExtensionIndex];
	}
	else if (Cast<USkeletalMesh>(Target)) {
		return SkeletalMeshExtension[TypeExtensions[2].ExtensionIndex];
	}
	else if (Cast<UStaticMesh>(Target)) {
		return StaticMeshExtension[TypeExtensions[3].ExtensionIndex];
	}
	else
		return "t3d";
}

void UConvertWidget::OpenFileWindow(const FString& WindowTitle, const FString& DefaultPath, const FString& FileTypes, TArray<FString>& OutFileString)
{
	if (!GEngine || !GEngine->GameViewport) return;
	void* ParentWindowHandle = GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

	if (DesktopPlatform) {
		uint8 SelectionFlag = 0;
		DesktopPlatform->OpenFileDialog(ParentWindowHandle, WindowTitle, DefaultPath, FString(), FileTypes, SelectionFlag, OutFileString);
	}
}


void UConvertWidget::Convert()
{
	TArray<UObject*> ObjectArr = GetObjectsByPath();
	UE_LOG(LogTemp, Warning, TEXT("11111"));

	for (auto& Object : ObjectArr) {
		Export(Object, GetObjectExtension(Object), FPaths::ProjectContentDir() + OutputDirectory + Object->GetName() + "." + GetObjectExtension(Object));
		UE_LOG(LogTemp, Warning, TEXT("222222"));

		FString FilePath = FPaths::ProjectContentDir() + OutputDirectory + Object->GetName() + "." + GetObjectExtension(Object);

		UE_LOG(LogTemp, Warning, TEXT("%s"), *FilePath);
	}
}

void UConvertWidget::AddConvertingFile()
{
	TArray<FString> OutFileString;
	OpenFileWindow(FString("Open"), FString(""), FString("UAsset Files|*.uasset"), OutFileString);

	if (OutFileString.Num() > 0) {
		IPlatformFile& PF = FPlatformFileManager::Get().GetPlatformFile();
		FString SourcePath = OutFileString[0];
		FString FileName = GetFileName(SourcePath);
		FString CopiedPath = FPaths::ProjectContentDir() + SourceDirectory + FileName + ".uasset";

		UE_LOG(LogTemp, Warning, TEXT("%s"), *CopiedPath);

		if (PF.CopyFile(*CopiedPath, *SourcePath)) {
			txt_Log->SetText(FText::FromString("Copy Successed"));
		}
		else {
			txt_Log->SetText(FText::FromString("Copy Failed"));
		}
	}
}

void UConvertWidget::OnOffSettingPanel()
{
	if (hb_Box->GetVisibility() == ESlateVisibility::Collapsed)
		hb_Box->SetVisibility(ESlateVisibility::Visible);
	else
		hb_Box->SetVisibility(ESlateVisibility::Collapsed);
}

void UConvertWidget::Export(UObject* Target, const FString& Extension, const FString& ExportPath)
{
	if (!Target) return;

	UExporter* Exporter = UExporter::FindExporter(Target, *Extension);

	if (!Exporter) return;
	UAssetExportTask* ExportTask = NewObject<UAssetExportTask>();
	ExportTask->Exporter = Exporter;
	ExportTask->Object = Target;
	ExportTask->Filename = ExportPath;
	ExportTask->bWriteEmptyFiles = true;

	if (Exporter->RunAssetExportTask(ExportTask)) {
		UE_LOG(LogTemp, Warning, TEXT("Export Success!"));
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Export Failed!"));
}
