#pragma once
#include "Camera.h"

void Camera::Setup()
{
	EdsError error = EdsSetCameraAddedHandler(HandleCameraAdded, this);
}

EdsError EDSCALLBACK Camera::HandleCameraAdded(EdsVoid* inContext)
{
	printf("hello");
	return EDS_ERR_OK;
}