// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved. 
#include "Tsumugi/Public/Tsumugi.h" 
class FTsumugi : public ITsumugi 
{ 
	/** IModuleInterface implementation */ 
	virtual void StartupModule() override; 
	virtual void ShutdownModule() override; 
}; 
IMPLEMENT_MODULE( FTsumugi, Tsumugi ) 
void FTsumugi::StartupModule() 
{ 
	// This code will execute after your module is loaded into memory (but after global variables are initialized, of course.) 
} 
void FTsumugi::ShutdownModule() 
{ 
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading, 
	// we call this function before unloading the module. 
} 
