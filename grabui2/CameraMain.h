#pragma once
#include "CameraBase.h"

class CameraMain : public CameraBase
{
private:
	// �Q�C���̃m�[�h�^�C�v
	enum class GAIN_TYPE {
		GAIN_DOUBLE,
		GAIN_INT64
	};

	enum class GAIN_TYPE typeGain;

	// �Q�C���̃m�[�h�Adouble
	Pylon::CFloatParameter doubleGain;
	double doubleGainMax;
	double doubleGainMin;

	// �Q�C���̃m�[�h�Along long (int64_t)
	Pylon::CIntegerParameter intGain;
	int64_t intGainMax;
	int64_t intGainMin;

	// �I�o���Ԃ̃m�[�h
	Pylon::CFloatParameter doubleExposureTime;
	bool flagExposureTimeValid;

	// 255�l�ɂȂ����s�N�Z���̌�
	int pixel255;

public:
	void AttachDevice(IPylonDevice* device) {
		this->camera.Attach(device);
		Init();
	}

	void Init();

	// �B�e���[�h�ƃX���b�h�N���A�X���b�h�I��
	void StartGrabbing();
	void StopGrabbing();

	// �B�e��̏���
	void AfterGrabbing(const Pylon::CGrabResultPtr& ptrGrabResult);
};