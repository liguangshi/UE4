// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "PerforceSourceControlPrivatePCH.h"
#include "PerforceSourceControlModule.h"
#include "ModuleManager.h"
#include "ISourceControlModule.h"
#include "PerforceSourceControlOperations.h"
#include "PerforceSourceControlState.h"
#include "PerforceSourceControlSettings.h"
#include "Runtime/Core/Public/Features/IModularFeatures.h"

#define LOCTEXT_NAMESPACE "PerforceSourceControl"

template<typename Type>
static TSharedRef<IPerforceSourceControlWorker, ESPMode::ThreadSafe> CreateWorker()
{
	return MakeShareable( new Type() );
}

void FPerforceSourceControlModule::StartupModule()
{
	// Register our operations
	PerforceSourceControlProvider.RegisterWorker( "Connect", FGetPerforceSourceControlWorker::CreateStatic( &CreateWorker<FPerforceConnectWorker> ) );
	PerforceSourceControlProvider.RegisterWorker( "CheckOut", FGetPerforceSourceControlWorker::CreateStatic( &CreateWorker<FPerforceCheckOutWorker> ) );
	PerforceSourceControlProvider.RegisterWorker( "UpdateStatus", FGetPerforceSourceControlWorker::CreateStatic( &CreateWorker<FPerforceUpdateStatusWorker> ) );
	PerforceSourceControlProvider.RegisterWorker( "MarkForAdd", FGetPerforceSourceControlWorker::CreateStatic( &CreateWorker<FPerforceMarkForAddWorker> ) );
	PerforceSourceControlProvider.RegisterWorker( "Delete", FGetPerforceSourceControlWorker::CreateStatic( &CreateWorker<FPerforceDeleteWorker> ) );
	PerforceSourceControlProvider.RegisterWorker( "Revert", FGetPerforceSourceControlWorker::CreateStatic( &CreateWorker<FPerforceRevertWorker> ) );
	PerforceSourceControlProvider.RegisterWorker( "Sync", FGetPerforceSourceControlWorker::CreateStatic( &CreateWorker<FPerforceSyncWorker> ) );
	PerforceSourceControlProvider.RegisterWorker( "CheckIn", FGetPerforceSourceControlWorker::CreateStatic( &CreateWorker<FPerforceCheckInWorker> ) );
	PerforceSourceControlProvider.RegisterWorker( "GetWorkspaces", FGetPerforceSourceControlWorker::CreateStatic( &CreateWorker<FPerforceGetWorkspacesWorker> ) );

	// load our settings
	PerforceSourceControlSettings.LoadSettings();

	// Bind our source control provider to the editor
	IModularFeatures::Get().RegisterModularFeature( "SourceControl", &PerforceSourceControlProvider );
}

void FPerforceSourceControlModule::ShutdownModule()
{
	// shut down the provider, as this module is going away
	PerforceSourceControlProvider.Close();

	// unbind provider from editor
	IModularFeatures::Get().UnregisterModularFeature( "SourceControl", &PerforceSourceControlProvider );
}

FPerforceSourceControlSettings& FPerforceSourceControlModule::AccessSettings()
{
	return PerforceSourceControlSettings;
}

void FPerforceSourceControlModule::SaveSettings()
{
	if (FApp::IsUnattended() || IsRunningCommandlet())
	{
		return;
	}

	PerforceSourceControlSettings.SaveSettings();
}

IMPLEMENT_MODULE(FPerforceSourceControlModule, PerforceSourceControl);

#undef LOCTEXT_NAMESPACE