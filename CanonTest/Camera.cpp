#pragma once
#include "Camera.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h""


char* concat(const char *s1, const char *s2)
{
	const size_t len1 = strlen(s1);
	const size_t len2 = strlen(s2);
	char *result = (char*)malloc(len1 + len2 + 1);//+1 for the zero-terminator
										   //in real code you would check for errors in malloc here
	memcpy(result, s1, len1);
	memcpy(result + len1, s2, len2 + 1);//+1 to copy the null-terminator
	return result;
}

EdsError EDSCALLBACK HandleObjectEvent(EdsObjectEvent InEvent, EdsBaseRef InRef, EdsVoid * InContext)
{
	// do something

	switch (InEvent)
	{
		case kEdsObjectEvent_DirItemRequestTransfer:
			DownloadImage(InRef);
			break;
		default:
			break;
	}

	// Object must be released
	if (InRef)
	{
		EdsRelease(InRef);
	}
	return EDS_ERR_OK;
}

EdsError EDSCALLBACK HandlePropertyEvent(EdsUInt32 InEvent, EdsUInt32 InPropertyID, EdsUInt32 InParam, EdsVoid* InContext)
{
	return EDS_ERR_OK;
}

EdsError EDSCALLBACK HandleStateEvent(EdsUInt32 InEvent, EdsUInt32 InParam, EdsVoid* InContext)
{
	return EDS_ERR_OK;
}

EdsError DownloadImage(EdsDirectoryItemRef directoryItem)
{
	EdsError Error = EDS_ERR_OK; EdsStreamRef stream = NULL;
	// Get directory item information
	EdsDirectoryItemInfo dirItemInfo;
	Error = EdsGetDirectoryItemInfo(directoryItem, &dirItemInfo);
	// Create file stream for transfer destination
	char* dest = concat("C:/Saved Pictures/" , dirItemInfo.szFileName);

	if (Error == EDS_ERR_OK)
	{
		Error = EdsCreateFileStream(dest,
			kEdsFileCreateDisposition_CreateAlways,
			kEdsAccess_ReadWrite, &stream);
	}
	// Download image
	if (Error == EDS_ERR_OK)
	{
		Error = EdsDownload(directoryItem, dirItemInfo.size, stream);
	}
	// Issue notification that download is complete
	if (Error == EDS_ERR_OK)
	{
		Error = EdsDownloadComplete(directoryItem);
	}
	// Release stream
	if (stream != NULL)
	{
		EdsRelease(stream);
		stream = NULL;
	}
	return Error;
}

EdsError TakePicture(EdsCameraRef camera)
{
	EdsError Error;
	Error = EdsSendCommand(camera, kEdsCameraCommand_PressShutterButton, kEdsCameraCommand_ShutterButton_Completely);
	if (Error == EDS_ERR_OK)
	{
		//Error = EdsSendCommand(camera, kEdsCameraCommand_PressShutterButton, kEdsCameraCommand_ShutterButton_OFF);
	}
	return Error;
}

EdsError GetVolume(EdsCameraRef camera, EdsVolumeRef * Volume)
{
	EdsError Error = EDS_ERR_OK;
	EdsUInt32 count = 0;
	// Get the number of camera volumes
	Error = EdsGetChildCount(camera, &count);
	if (Error == EDS_ERR_OK && count == 0)
	{
		Error = EDS_ERR_DIR_NOT_FOUND;
	}
	// Get initial volume
	if (Error == EDS_ERR_OK)
	{
		Error = EdsGetChildAtIndex(camera, 0, Volume);
	}
	return Error;
}

EdsError GetFirstCamera(EdsCameraRef *Camera)
{
	EdsError Error = EDS_ERR_OK;
	EdsCameraListRef cameraList = NULL;
	EdsUInt32 count = 0;
	// Get camera list
	Error = EdsGetCameraList(&cameraList);
	// Get number of cameras
	if (Error == EDS_ERR_OK)
	{
		Error = EdsGetChildCount(cameraList, &count);
		if (count == 0)
		{
			Error = EDS_ERR_DEVICE_NOT_FOUND;
		}
	}
	// Get first camera retrieved
	if (Error == EDS_ERR_OK)
	{
		Error = EdsGetChildAtIndex(cameraList, 0, Camera);
	}
	// Release camera list
	if (cameraList != NULL)
	{
		EdsRelease(cameraList);
		cameraList = NULL;
	}

	return Error;
}

EdsError GetTv(EdsCameraRef Camera, EdsUInt32 *Tv)
{
	EdsError Error = EDS_ERR_OK; 
	EdsDataType dataType;
	EdsUInt32 dataSize;
	Error = EdsGetPropertySize(Camera, kEdsPropID_Tv, 0, &dataType, &dataSize);
	if (Error == EDS_ERR_OK)
	{
		Error = EdsGetPropertyData(Camera, kEdsPropID_Tv, 0, dataSize, Tv);
	}
	return Error;
}

EdsError GetTvDesc(EdsCameraRef camera, EdsPropertyDesc *TvDesc)
{
	return EdsGetPropertyDesc(camera, kEdsPropID_Tv, TvDesc);
}

EdsError SetTv(EdsCameraRef camera, EdsUInt32 TvValue)
{
	return EdsSetPropertyData(camera, kEdsPropID_Tv, 0, sizeof(TvValue), &TvValue);
}

void OldCode()
{
	// Initialize the SDK
	EdsError Error = EdsInitializeSDK();
	if (Error != EDS_ERR_OK)
	{
		printf("ERROR - failed to initialize SDK\n");
	}

	//while (!bContinue);


	// Getting the list of cameras
	EdsCameraListRef CameraList = NULL;
	Error = EdsGetCameraList(&CameraList);

	if (Error != EDS_ERR_OK) {
		printf("ERROR - failed to get camera list\n");
		EdsRelease(CameraList);
		return;
	}

	// Getting the count of cameras connected
	EdsUInt32 CameraCount = 0;
	Error = EdsGetChildCount(CameraList, &CameraCount);
	if (Error != EDS_ERR_OK) {
		printf("ERROR - failed to get camera count\n");
		EdsRelease(CameraList);
		return;
	}

	// Make sure the number of cameras are more than 0
	if (CameraCount > 0) {
		// Get the camera at index 0
		EdsCameraRef Camera = NULL;
		Error = EdsGetChildAtIndex(CameraList, 0, &Camera);
		if (Error != EDS_ERR_OK) {
			printf("ERROR - failed to get camera\n");
			return;
		}

		// tell the api to retain the reference of the camera
		EdsRetain(Camera);

		// get the device info of the camera, this step is optional
		EdsDeviceInfo DeviceInfo;
		Error = EdsGetDeviceInfo(Camera, &DeviceInfo);
		if (Error != EDS_ERR_OK) {
			printf("ERROR - failed to get device info\n");
			return;
		}
		else
		{
			printf("Discovered Device: %s\n", DeviceInfo.szDeviceDescription);
		}

		// set event handlers
		Error = EdsSetObjectEventHandler(Camera, kEdsObjectEvent_All, HandleObjectEvent, NULL);
		if (Error != EDS_ERR_OK) {
			printf("ERROR - failed to set object event handler\n");
		}
		Error = EdsSetPropertyEventHandler(Camera, kEdsPropertyEvent_All, HandlePropertyEvent, NULL);
		if (Error != EDS_ERR_OK) {
			printf("ERROR - failed to set property event handler\n");
		}
		Error = EdsSetCameraStateEventHandler(Camera, kEdsStateEvent_All, HandleStateEvent, NULL);
		if (Error != EDS_ERR_OK) {
			printf("ERROR - failed to set object event handler\n");
		}

		// Start opening session with camera
		Error = EdsOpenSession(Camera);
		if (Error != EDS_ERR_OK) {
			printf("ERROR - failed to open session\n");
		}

		// Set the photo to be saved on the camera for now
		EdsUInt32 SaveTo = kEdsSaveTo_Camera;
		Error = EdsSetPropertyData(Camera, kEdsPropID_SaveTo, 0, sizeof(SaveTo), &SaveTo);
		if (Error != EDS_ERR_OK) {
			printf("ERROR - failed to set save destination host/device");
		}

		if (SaveTo == kEdsSaveTo_Host)
		{
			// ??? - requires UI lock?
			EdsCapacity Capacity = { 0x7FFFFFFF, 0x1000, 1 };
			Error = EdsSetCapacity(Camera, Capacity);
			if (Error != EDS_ERR_OK)
			{
				printf("failed to set capacity of host");
			}
		}
		// Take the photo
		Error = EdsSendCommand(Camera, kEdsCameraCommand_TakePicture, 0);
		if (Error != EDS_ERR_OK)
		{
			printf("ERROR - Failed to take picture");
		}
		//while (!bContinue);
		// Close the session
		Error = EdsCloseSession(Camera);
		if (Error != EDS_ERR_OK) {
			printf("ERROR - failed to open session\n");
		}

		EdsRelease(Camera);
	}
	printf("Hey\n");
}

void ApplicationRun()
{
	MSG msg;
	EdsError Error = EDS_ERR_OK;
	EdsCameraRef Camera = NULL;
	EdsDeviceInfo DeviceInfo;

	bool isSDKLoaded = false;
	// Initialize SDK
	Error = EdsInitializeSDK();
	if (Error == EDS_ERR_OK)
	{
		isSDKLoaded = true;
	}
	// Get first camera
	if (Error == EDS_ERR_OK)
	{
		Error = GetFirstCamera(&Camera);
	}
	// Set event handler
	if (Error == EDS_ERR_OK)
	{
		Error = EdsSetObjectEventHandler(Camera, kEdsObjectEvent_All,
			HandleObjectEvent, NULL);
	}
	// Set event handler
	if (Error == EDS_ERR_OK)
	{
		Error = EdsSetPropertyEventHandler(Camera, kEdsPropertyEvent_All,
			HandlePropertyEvent, NULL);
	}
	// Set event handler
	if (Error == EDS_ERR_OK)
	{
		Error = EdsSetCameraStateEventHandler(Camera, kEdsStateEvent_All,
			HandleStateEvent, NULL);
	}
	// Open session with camera
	if (Error == EDS_ERR_OK)
	{
		Error = EdsOpenSession(Camera);
	}

	if (Error == EDS_ERR_OK)
	{
		Error = EdsGetDeviceInfo(Camera, &DeviceInfo);
	}

	if (Error == EDS_ERR_OK)
	{
		printf("Device name: %s.\n", DeviceInfo.szDeviceDescription);
	}


	bContinue = false;

	// Set the photo to be saved on the camera for now
	EdsUInt32 SaveTo = kEdsSaveTo_Host;
	Error = EdsSetPropertyData(Camera, kEdsPropID_SaveTo, 0, sizeof(SaveTo), &SaveTo);

	EdsCapacity Capacity = { 0x7FFFFFFF, 0x1000, 1 };
	Error = EdsSetCapacity(Camera, Capacity);

	Error = EdsSendStatusCommand(Camera, kEdsCameraStatusCommand_ExitDirectTransfer, 0);

	/////
	//Error = TakePicture(Camera);
	while (!bContinue && GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//
	// Close session with camera
	if (Error == EDS_ERR_OK)
	{
		Error = EdsCloseSession(Camera);
	}
	// Release camera
	if (Camera != NULL)
	{
		EdsRelease(Camera);
	}
	// Terminate SDK
	if (isSDKLoaded)
	{
		EdsTerminateSDK();
	}
}