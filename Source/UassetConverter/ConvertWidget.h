// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ConvertWidget.generated.h"

class UButton;
class UTextBlock;
class UComboBoxString;
class UHorizontalBox;

UENUM()
enum class EObjectType : uint8
{
	Image UMETA(DisplayName = "Image"),
	Audio UMETA(DisplayName = "Audio"),
	SkeletalMesh UMETA(DisplayName = "SkeletalMesh"),
	StaticMesh UMETA(DisplayName = "StaticMesh")
};

USTRUCT()
struct FTypeExtension
{
	GENERATED_BODY()

public:
	FTypeExtension() {}
	FTypeExtension(UComboBoxString* _Combo, EObjectType _Type) : Combo(_Combo), Type(_Type) {}

	UPROPERTY()
	UComboBoxString* Combo;
	UPROPERTY()
	EObjectType Type;
	UPROPERTY()
	uint8 ExtensionIndex = 0;
};

UCLASS()
class UASSETCONVERTER_API UConvertWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* btn_OpenFile;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* btn_Convert;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* btn_Setting;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UHorizontalBox* hb_Box;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* txt_Log;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UComboBoxString* cb_Image;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UComboBoxString* cb_Audio;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UComboBoxString* cb_SkeletalMesh;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UComboBoxString* cb_StaticMesh;

private:
	const FString SourceDirectory = "01_SourceFile/";
	const FString OutputDirectory = "02_ConvertedFile/";

	const TArray<FString> ImageExtension = { "png", "tga", "pcx", "bmp" };
	const TArray<FString> AudioExtension = { "wav", "ogg" };
	const TArray<FString> SkeletalMeshExtension = { "fbx", "glb", "gltf" };
	const TArray<FString> StaticMeshExtension = { "fbx", "obj", "glb", "gltf" };

	const TArray<UComboBoxString*> ComboBoxArr = { cb_Image, cb_Audio, cb_SkeletalMesh, cb_StaticMesh };
	const TArray<TArray<FString>> TypeExtensionArr = { ImageExtension, AudioExtension, SkeletalMeshExtension, StaticMeshExtension };

	TArray<FTypeExtension> TypeExtensions;

	void InitComboBox();
	void SetComboOptions(UComboBoxString* TargetCombo, const TArray<FString>& TargetExtensions, EObjectType Type);
	void EndOfSetting();

	FString GetGamePath(const FString& FilePath);
	FString GetFileName(const FString& FilePath);
	TArray<UObject*> GetObjectsByPath();
	FString GetObjectExtension(UObject* Target);

	void OpenFileWindow(const FString& WindowTitle, const FString& DefaultPath, const FString& FileTypes, TArray<FString>& OutFileString);

	UFUNCTION(BlueprintCallable)
	void Convert();
	UFUNCTION(BlueprintCallable)
	void AddConvertingFile();
	UFUNCTION(BlueprintCallable)
	void OnOffSettingPanel();
	UFUNCTION(BlueprintCallable)
	void Export(UObject* Target, const FString& Extension, const FString& ExportPath);
};
