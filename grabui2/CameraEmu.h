#pragma once
#include "CameraBase.h"

class CameraEmu : public CameraBase
{
public:
	void AttachDevice(IPylonDevice* device) {
		this->camera.Attach(device);
		Init();
	}

	void Init();

	void StartGrabbing();
	void StopGrabbing();

	void AfterGrabbing(const Pylon::CGrabResultPtr& ptrGrabResult);
};