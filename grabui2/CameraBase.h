#pragma once
#include <string>
#include "imgui.h"
#include <pylon/PylonIncludes.h>

#include "BufferedImage.h"

using namespace std;
using namespace Pylon;

class CameraBase
{
private:
	// �B�e�����ǂ���
	bool fgrabbing = false;
	 
	// �ۑ��t���O
	bool fsave = false;

	// 255�J�E���g
	bool fcount255 = false;

	// 255����
	bool ftune255 = false;

	// �ۑ��t�@�C�����̃|�C���^
	char saveFileName[FILE_NAME_LEN];

public:
	// �ۑ�����
	void setSaveFileName(const char* filename) {
		memcpy_s(saveFileName, FILE_NAME_LEN, filename, FILE_NAME_LEN);
	}
	char* getSaveFileName() {
		return saveFileName;
	}

public:
	// �B�e���t���O
	bool isGrabbing() { return fgrabbing; }
	void setGrabbing() { fgrabbing = true; }
	void unsetGrabbing() { fgrabbing = false; }

	// �ۑ��t���O
	bool isSave() { return fsave; }
	void setSave() { fsave = true; }
	void unsetSave() { fsave = false; }

	// 255�J�E���g
	bool isCount255() { return fcount255; }
	void setCount255() { fcount255 = true; }
	void unsetCount255() { fcount255 = false; }

	// 255����
	bool isTune255() { return ftune255; }
	void setTune255() { ftune255 = true; }
	void unsetTune255() { ftune255 = false; }

protected:
	// �J�����C���X�^���X
	CInstantCamera camera;

	// �C���[�W�o�b�t�@
	BufferedImage image;

protected:
	// �I�o���Ԃ̃m�[�h
	Pylon::CFloatParameter doubleExposureTime;
	bool flagExposureTimeValid;
	double exposureTimeCache = 0.0;

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
	double doubleGainCache = 0.0;

	// �Q�C���̃m�[�h�Along long (int64_t)
	Pylon::CIntegerParameter intGain;
	int64_t intGainMax;
	int64_t intGainMin;
	int64_t intGainCache = 0;

protected:
	// ��񂩂���̐ݒ�
	void Init() {
		// node�ݒ�� Open()�̌�
		//setNode();
	}

public:
	// �����f�o�C�X�֋��n���A�X���b�h����Ă΂��̂� public �Ō��J
	bool CanWaitForFrameTriggerReady() {
		return camera.CanWaitForFrameTriggerReady();
	}

	bool WaitForFrameTriggerReady(unsigned int timeoutMs, Pylon::ETimeoutHandling timeoutHandling) {
		return camera.WaitForFrameTriggerReady(timeoutMs, timeoutHandling);
	}

	void ExecuteSoftwareTrigger() {
		camera.ExecuteSoftwareTrigger();
	}

public:
	// �f�o�C�X���A�^�b�`
	virtual void AttachDevice(IPylonDevice* device) = 0;

#if 0
	// �C���[�W�o�b�t�@���A�^�b�`
	void AttachImageBuffer(BufferedImage* buf) {
		image = buf;
	}
#endif

	~CameraBase() {
		StopGrabbing();
	}

	// grab �X���b�h�N���A�I��
	virtual void StartGrabbing() = 0;
	virtual void StopGrabbing() {
		if (!isGrabbing()) {
			return;
		}
		unsetGrabbing();
		camera.Close();
	}

	void setNode();

	// grab ����
	virtual void AfterGrabbing(const Pylon::CGrabResultPtr& ptrGrabResult) = 0;

	// �`��
	void DrawImage() {
		image.DrawImage();
	}

	// �Q�C���̂��Ƃ�
	int64_t GetIntGain() { return intGainCache; }
	double GetDoubleGain() { return doubleGainCache; }

	void SetIntGain(int64_t val) {
		intGain.SetValue(val);
		intGainCache = val;
	}
	void SetDoubleGain(double val) {
		doubleGain.SetValue(val);
		doubleGainCache = val;
	}

	int64_t GetIntGainMax() { return intGainMax; }
	int64_t GetIntGainMin() { return intGainMin; }

	double GetDoubleGainMax() { return doubleGainMax; }
	double GetDoubleGainMin() { return doubleGainMin; }

	// �I�o����
	double GetDoubleExposureTime() {
		if (flagExposureTimeValid) {
			return exposureTimeCache;
		}
		return 1000.0;
	}
	void SetDoubleExposureTime(double val) {
		if (flagExposureTimeValid) {
			doubleExposureTime.SetValue(val);
			exposureTimeCache = val;
		}
	}

	// �\���p
	string getGainMinMaxString();
	string getGainString();
	string getExposureString();

	// �p�����[�^�ݒ�|�b�v�A�b�v
	void popupConfig(const char* winname);

	// �ۑ��J�n�Aimage �ɂ��܂���
	void startSaveImage()
	{
		image.imgWrite(saveFileName);
	}
};