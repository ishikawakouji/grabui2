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

		// デバイス名取得してpopup名を決める
		string devsn = camera.GetDeviceInfo().GetSerialNumber().c_str();
		popupWinName = devsn + "_config";
	}

};