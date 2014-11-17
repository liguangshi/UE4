// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Throbber.generated.h"

class USlateBrushAsset;

/** A Throbber widget that shows several zooming circles in a row. */
UCLASS(meta=( Category="Misc" ), ClassGroup=UserInterface)
class UMG_API UThrobber : public UWidget
{
	GENERATED_UCLASS_BODY()

	/** How many pieces there are */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance, meta=(ClampMin = "1", UIMin = "1", UIMax = "8"))
	int32 NumberOfPieces;

	/** Should the pieces animate horizontally? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	bool bAnimateHorizontally;

	/** Should the pieces animate vertically? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	bool bAnimateVertically;

	/** Should the pieces animate their opacity? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance)
	bool bAnimateOpacity;

	/** Image to use for each segment of the throbber */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Appearance, meta=( DisplayThumbnail = "true" ))
	USlateBrushAsset* PieceImage;

protected:
	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	// End of UWidget interface
};