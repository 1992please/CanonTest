#include "EDSDK.h"
#include "EDSDKErrors.h"
#include "EDSDKTypes.h"
#include "stdio.h"

static bool bExit;

struct Settings;

EdsError EDSCALLBACK HandleObjectEvent(EdsUInt32 InEvent, EdsBaseRef InRef, EdsVoid* InContext);

EdsError EDSCALLBACK HandlePropertyEvent(EdsUInt32 InEvent, EdsUInt32 InPropertyID, EdsUInt32 InParam, EdsVoid* InContext);

EdsError EDSCALLBACK HandleStateEvent(EdsUInt32 InEvent, EdsUInt32 InParam, EdsVoid* InContext);

void main()
{
	// Initialize the SDK
	EdsError Error = EdsInitializeSDK();
	if (Error != EDS_ERR_OK)
	{
		printf("ERROR - failed to initialize SDK\n");
	}

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
		EdsError error = EdsGetDeviceInfo(Camera, &DeviceInfo);
		if (Error != EDS_ERR_OK) {
			printf("ERROR - failed to get device info\n");
			return;
		}
		else
		{
			printf("Discovered Device: %s\n", DeviceInfo.szDeviceDescription);
		}

		// set event handlers
		error = EdsSetObjectEventHandler(Camera, kEdsObjectEvent_All, HandleObjectEvent, NULL);
		if (error != EDS_ERR_OK) {
			printf("ERROR - failed to set object event handler\n");
		}
		error = EdsSetPropertyEventHandler(Camera, kEdsPropertyEvent_All, HandlePropertyEvent, NULL);
		if (error != EDS_ERR_OK) {
			printf("ERROR - failed to set property event handler\n");
		}
		error = EdsSetCameraStateEventHandler(Camera, kEdsStateEvent_All, HandleStateEvent, NULL);
		if (error != EDS_ERR_OK) {
			printf("ERROR - failed to set object event handler\n");
		}

		// Start opening session with camera
		error = EdsOpenSession(Camera);
		if (error != EDS_ERR_OK) {
			printf("ERROR - failed to open session\n");
		}

		// Set the photo to be saved on the camera for now
		EdsUInt32 SaveTo = kEdsSaveTo_Camera;
		error = EdsSetPropertyData(Camera, kEdsPropID_SaveTo, 0, sizeof(SaveTo), &SaveTo);
		if (error != EDS_ERR_OK) {
			printf("ERROR - failed to set save destination host/device");
		}

		// Take the photo
		error = EdsSendCommand(Camera, kEdsCameraCommand_TakePicture, 0);
		if (error != EDS_ERR_OK)
		{
			printf("ERROR - Failed to take picture");
		}

		// Close the session
		error = EdsCloseSession(Camera);
		if (error != EDS_ERR_OK) {
			printf("ERROR - failed to open session\n");
		}

		EdsRelease(Camera);
	}
	printf("Hey\n");
}


EdsError EDSCALLBACK HandleObjectEvent(EdsUInt32 InEvent, EdsBaseRef InRef, EdsVoid* InContext)
{
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


struct Settings {
	Settings() : bShouldKeepAlive(true), mPictureSaveLocation(kEdsSaveTo_Host) {}

	Settings& setShouldKeepAlive(bool flag) {
		bShouldKeepAlive = flag; return *this;
	}
	inline bool getShouldKeepAlive() const {
		return bShouldKeepAlive;
	}
	Settings& setPictureSaveLocation(EdsUInt32 location) {
		mPictureSaveLocation = location; return *this;
	}
	inline EdsUInt32 getPictureSaveLocation() const {
		return mPictureSaveLocation;
	}

private:
	bool bShouldKeepAlive;
	EdsUInt32 mPictureSaveLocation;
};