#pragma once
#include <string>
#include "imgui.h"
#include <pylon/PylonIncludes.h>

#include "BufferedImage.h"

using namespace std;
using namespace Pylon;

class CameraBase
{
public:
	// camera main �Ƃ̐�������邽��
	bool isMain() { return true; }
private:
	// �Q�C�������ɕK�v�ȃp�����[�^
	int minWaitTime = 50; // �Œ�B�e�Ԋu ms
	int minArea = 20000; // �m�F����G���A�̖ʐς̍ŏ��l pixcels
	int pixMinVal = 225; // ����͈͂����߂�Ƃ��̃s�N�Z���l�̍ŏ��l
	double minExTime = 1000.0; // �I�o���Ԃ̍ŏ��l usec
	double maxExTime = 4000.0; // �I�o���Ԃ̍ő�l usec

	// �v�Z�����ǂ���
	bool runningAfterGrabbing = false;

	// �����Q�C�����������s���邩�ǂ���
	bool fdoAutoGainTune = false;

public:
	// �p�����[�^�̓��o��
	void setWaitTime(int val) {
		minWaitTime = val;
	}
	int getWaitTime() {
		return minWaitTime;
	}
	void setMinArea(int val) {
		minArea = val;
	}
	int getMinArea() {
		return minArea;
	}
	void setPixMinVal(int val) {
		pixMinVal = val;
	}
	int getPixMinVal() {
		return pixMinVal;
	}
	void setExTimeRange(double vmin, double vmax) {
		minExTime = vmin;
		maxExTime = vmax;
	}

	void setAfterGrabbing() {
		runningAfterGrabbing = true;
	}
	void unsetAfterGrabbing() {
		runningAfterGrabbing = false;
	}
	bool isAfterGrabbing() {
		return runningAfterGrabbing;
	}
	void setAutoGainTune(bool val) {
		fdoAutoGainTune = val;
	}
	bool doAutoGainTune() {
		return fdoAutoGainTune;
	}
private:
	// �I�[�v�����Ă��邩�ǂ���
	bool fopen = false;

	// �B�e�����ǂ���
	bool fgrabbing = false;
	 
	// �ۑ��t���O
	bool fsave = false;

	// 255�J�E���g
	bool fcount255 = false;
	int count255 = 0;
	bool fphysical = false;

	// 255����
	bool ftune255 = false;

	// �ۑ��t�@�C�����̃|�C���^
	char saveFileName[FILE_NAME_LEN];

public:
	// 255�J�E���g
	void setCount255(int val) { count255 = val; }
	int getCount255() {
		return count255;
	}
	// �ۑ�����
	void setSaveFileName(const char* filename) {
		memcpy_s(saveFileName, FILE_NAME_LEN, filename, FILE_NAME_LEN);
	}
	char* getSaveFileName() {
		return saveFileName;
	}

public:
	// �J�����I�[�v��
	bool isOpen() {
		return fopen;
	}
	void setOpen() { fopen = true; }
	void unsetOpen() {
		fopen = false;
	}

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

	// �J�����ݒ�popup �̖��O
	string popupWinName;

protected:
	// �I�o���Ԃ̃m�[�h
	enum class EXPOSURE_TYPE {
		EX_DOUBLE,
		EX_RAW
	};
	enum class EXPOSURE_TYPE typeExposureTime;

	Pylon::CFloatParameter doubleExposureTime;
	bool flagExposureTimeValid;
	//double exposureTimeCache = 0.0;
	Pylon::CIntegerParameter intExposureTime;

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

	// popup��
	string getPopupWinName() {
		return popupWinName;
	}

	// ���A���J����
	void setPhysicalCamera() {
		image.setColorCamera();
		fphysical = true;
	}
	bool isPhysicalCamera() {
		return fphysical;
	}

#if 0
	// �C���[�W�o�b�t�@���A�^�b�`
	void AttachImageBuffer(BufferedImage* buf) {
		image = buf;
	}
#endif

	~CameraBase() {
		CameraClose();
	}

	// open/close
	void CameraOpen() {
		if (!isOpen()) {
			camera.Open();
			setOpen();
		}
	}
	void CameraClose() {
		if (isOpen()) {
			camera.Close();
			unsetOpen();
		}
	}

	// grab �X���b�h�N���A�I��
	virtual void StartGrabbing() = 0;
	virtual void StopGrabbing() {
		if (!isGrabbing()) {
			return;
		}
		camera.StopGrabbing();
		unsetGrabbing();
		//camera.Close();
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

	int64_t SetIntGain(int64_t val) {
		if (val < intGainMin) val = intGainMin;
		if (val > intGainMax) val = intGainMax;

		if (isOpen()) {
			intGain.SetValue(val);
			intGainCache = val;
		}
		else {
			val = intGainCache;
		}
		return val;
	}
	double SetDoubleGain(double val) {
		if (typeGain != GAIN_TYPE::GAIN_DOUBLE) return 0.0;
		if (val < doubleGainMin) val = doubleGainMin;
		if (val > doubleGainMax) val = doubleGainMax;

		if (isOpen()) {
			doubleGain.SetValue(val);
			doubleGainCache = val;
		}
		else {
			val = doubleGainCache;
		}
		return val;
	}

	int64_t GetIntGainMax() { return intGainMax; }
	int64_t GetIntGainMin() { return intGainMin; }

	double GetDoubleGainMax() { return doubleGainMax; }
	double GetDoubleGainMin() { return doubleGainMin; }

	// �I�o����
	double GetDoubleExposureTime() {
		if (flagExposureTimeValid) {
			return doubleExposureTime.GetValue();
		}
		else {
			return (double)intExposureTime.GetValue();
		}
	}
	double SetDoubleExposureTime(double val) {
		if (val < minExTime) val = minExTime;
		if (val > maxExTime) val = maxExTime;

		if (isOpen()) {
			if (flagExposureTimeValid) {
				doubleExposureTime.SetValue(val);
				val = doubleExposureTime.GetValue();
			}
			else {
				intExposureTime.SetValue((int64_t)val);
				val = (double)intExposureTime.GetValue();
			}
		}
		else {
			val = 1000.0;
		}
		return val;
	}

	// �\���p
	string getGainMinMaxString();
	string getGainString();
	string getExposureString();
	string getCount255String();

	// �p�����[�^�ݒ�|�b�v�A�b�v
	void popupConfig(const char* winname);

	// �ۑ��J�n�Aimage �ɂ��܂���
	void startSaveImage()
	{
		image.imgWrite(saveFileName);
	}

#ifdef DEBUG_VIEW
public:
	virtual void DrawDebugImage() = 0;
#endif
};