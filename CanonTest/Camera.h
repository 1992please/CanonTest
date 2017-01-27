#pragma once

#include <iostream>
#include "EDSDK.h"

class Camera
{
public:
	void Setup();
	static EdsError EDSCALLBACK Camera::HandleCameraAdded(EdsVoid* inContext);

};