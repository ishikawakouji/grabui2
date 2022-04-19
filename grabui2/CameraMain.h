#pragma once
#include "CameraBase.h"

class CameraMain : public CameraBase
{
private:
	// ゲインのノードタイプ
	enum class GAIN_TYPE {
		GAIN_DOUBLE,
		GAIN_INT64
	};

	enum class GAIN_TYPE typeGain;

	// ゲインのノード、double
	Pylon::CFloatParameter doubleGain;
	double doubleGainMax;
	double doubleGainMin;

	// ゲインのノード、long long (int64_t)
	Pylon::CIntegerParameter intGain;
	int64_t intGainMax;
	int64_t intGainMin;

	// 露出時間のノード
	Pylon::CFloatParameter doubleExposureTime;
	bool flagExposureTimeValid;

	// 255値になったピクセルの個数
	int pixel255;

public:
	void AttachDevice(IPylonDevice* device) {
		this->camera.Attach(device);
		Init();
	}

	void Init();

	// 撮影モードとスレッド起動、スレッド終了
	void StartGrabbing();
	void StopGrabbing();

	// 撮影後の処理
	void AfterGrabbing(const Pylon::CGrabResultPtr& ptrGrabResult);
};