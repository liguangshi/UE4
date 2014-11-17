// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "UMGEditorPrivatePCH.h"
#include "UMGSequencerObjectBindingManager.h"
#include "WidgetBlueprintEditor.h"
#include "ISequencer.h"
#include "MovieScene.h"

FUMGSequencerObjectBindingManager::FUMGSequencerObjectBindingManager( FWidgetBlueprintEditor& InWidgetBlueprintEditor, UMovieScene& InMovieScene )
	: WidgetBlueprintEditor( InWidgetBlueprintEditor )
	, MovieScene( &InMovieScene )
{
	WidgetBlueprintEditor.GetOnWidgetPreviewUpdated().AddRaw( this, &FUMGSequencerObjectBindingManager::OnWidgetPreviewUpdated );

}

FUMGSequencerObjectBindingManager::~FUMGSequencerObjectBindingManager()
{
}

FGuid FUMGSequencerObjectBindingManager::FindGuidForObject( const UMovieScene& MovieScene, UObject& Object ) const
{
	return PreviewObjectToGuidMap.FindRef( &Object );
}

bool FUMGSequencerObjectBindingManager::CanPossessObject( UObject& Object ) const
{
	UWidgetBlueprint* WidgetBlueprint = WidgetBlueprintEditor.GetWidgetBlueprintObj();

	// Only preview widgets in this blueprint can be possessed
	UUserWidget* PreviewWidget = WidgetBlueprintEditor.GetPreview();

	return Object.IsA<UVisual>() && Object.IsIn( PreviewWidget );
}

void FUMGSequencerObjectBindingManager::BindPossessableObject( const FGuid& PossessableGuid, UObject& PossessedObject )
{
	PreviewObjectToGuidMap.Add( &PossessedObject, PossessableGuid );

	UWidgetBlueprint* WidgetBlueprint = WidgetBlueprintEditor.GetWidgetBlueprintObj();
	FWidgetAnimation* WidgetAnimation = WidgetBlueprint->FindAnimationDataForMovieScene(*MovieScene);

	FWidgetAnimationBinding NewBinding;
	NewBinding.WidgetName = PossessedObject.GetFName();
	if( PossessedObject.IsA<UPanelSlot>() )
	{
		// Save the name of the widget containing the slots.  This is the object to look up that contaisn the slot itself(the thing we are animating)
		NewBinding.SlotWidgetName = PossessedObject.GetOuter()->GetFName();
	}

	NewBinding.AnimationGuid = PossessableGuid;

	WidgetAnimation->AnimationBindings.Add( NewBinding );
}

void FUMGSequencerObjectBindingManager::UnbindPossessableObjects( const FGuid& PossessableGuid )
{
	for( auto It = PreviewObjectToGuidMap.CreateIterator(); It; ++It )
	{
		if( It.Value() == PossessableGuid )
		{
			It.RemoveCurrent();
		}
	}

	UWidgetBlueprint* WidgetBlueprint = WidgetBlueprintEditor.GetWidgetBlueprintObj();
	FWidgetAnimation* WidgetAnimation = WidgetBlueprint->FindAnimationDataForMovieScene(*MovieScene);

	WidgetAnimation->AnimationBindings.RemoveAll( [&]( const FWidgetAnimationBinding& Binding ) { return Binding.AnimationGuid == PossessableGuid; } );

}

void FUMGSequencerObjectBindingManager::GetRuntimeObjects( const TSharedRef<FMovieSceneInstance>& MovieSceneInstance, const FGuid& ObjectGuid, TArray<UObject*>& OutRuntimeObjects ) const
{
	for( auto It = PreviewObjectToGuidMap.CreateConstIterator(); It; ++It )
	{
		UObject* Object = It.Key().Get();
		if( Object && It.Value() == ObjectGuid )
		{
			OutRuntimeObjects.Add( Object );
		}
	}
}

void FUMGSequencerObjectBindingManager::InitPreviewObjects()
{
	UWidgetBlueprint* WidgetBlueprint = WidgetBlueprintEditor.GetWidgetBlueprintObj();

	// Populate preview object to guid map
	UUserWidget* PreviewWidget = WidgetBlueprintEditor.GetPreview();

	UWidgetTree* WidgetTree = PreviewWidget->WidgetTree;

	const FWidgetAnimation* WidgetAnimation = WidgetBlueprint->FindAnimationDataForMovieScene( *MovieScene );
	check( WidgetAnimation );

	for( const FWidgetAnimationBinding& Binding : WidgetAnimation->AnimationBindings )
	{
		FName ObjectName = Binding.WidgetName;
		FName SlotWidgetName = Binding.SlotWidgetName;

		FName NameToSearchFor = SlotWidgetName != NAME_None ? SlotWidgetName : ObjectName;

		UObject* FoundObject = FindObject<UObject>(WidgetTree, *NameToSearchFor.ToString());
		if(FoundObject)
		{
			if( SlotWidgetName == NAME_None )
			{
				PreviewObjectToGuidMap.Add(FoundObject, Binding.AnimationGuid);
			}
			else
			{
				FoundObject = FindObject<UObject>( FoundObject, *ObjectName.ToString() );
				if( FoundObject )
				{
					PreviewObjectToGuidMap.Add(FoundObject, Binding.AnimationGuid);
				}
			}
		
		}
	}
}

void FUMGSequencerObjectBindingManager::OnWidgetPreviewUpdated()
{
	PreviewObjectToGuidMap.Empty();

	InitPreviewObjects();

	WidgetBlueprintEditor.GetSequencer()->UpdateRuntimeInstances();
}