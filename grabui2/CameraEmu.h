#pragma once
#include "CameraBase.h"

class CameraEmu : public CameraBase
{
public:
	void AttachDevice(IPylonDevice* device) {
		this->camera.Attach(device);
		Init();

		// �f�o�C�X���擾����popup�������߂�
		string devsn = camera.GetDeviceInfo().GetSerialNumber().c_str();
		popupWinName = devsn + "_config";
	}

	void Init();

	void StartGrabbing();

	void AfterGrabbing(const Pylon::CGrabResultPtr& ptrGrabResult);

#ifdef DEBUG_VIEW
public:
	void DrawDebugImage() {}
#endif
};