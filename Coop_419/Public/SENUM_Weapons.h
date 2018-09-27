#pragma once

UENUM(BlueprintType)

enum class EWeaponTypes : uint8
{

	none UMETA(DisplayName = "none"),
	Rifle UMETA(DisplayName = "Rifle"),
	AIRifle UMETA(DisplayName = "AIRifle"),
	Launcher UMETA(DisplayName = "Grenade Launcher")

};

template<typename T>
static FString EnumAsString(const FString& enumName, const T value)
{
	UEnum* pEnum = FindObject<UEnum>(ANY_PACKAGE, *enumName);
	return *pEnum ? pEnum->GetNameStringByIndex(static_cast<uint8>(value)) : "null";
}