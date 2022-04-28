#pragma once
#include "CameraMain.h"

class CameraSub : public CameraMain {
protected:
	void setConfig();

public:
	void AttachDevice(IPylonDevice* device) {
		this->camera.Attach(device);

		preInit();

		// 内部トリガを使わない
		unsetMain();

		setConfig();

		Init();
	}

};