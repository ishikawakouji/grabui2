#pragma once
#include "CameraMain.h"

class CameraSub : public CameraMain {
protected:
	void setConfig();

public:
	void AttachDevice(IPylonDevice* device) {
		this->camera.Attach(device);

		preInit();

		// �����g���K���g��Ȃ�
		unsetMain();

		setConfig();

		Init();
	}

};