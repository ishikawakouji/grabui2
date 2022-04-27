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
	int pixel255 = 0;

protected:
	void preInit() {
		CameraBase::Init();
	}

	void setConfig();
public:
	void AttachDevice(IPylonDevice* device) {
		this->camera.Attach(device);
		
		preInit();
		setConfig();

		Init();
	}

	void Init();

	// �B�e���[�h�ƃX���b�h�N���A�X���b�h�I��
	void StartGrabbing();

	// �B�e��̏���
	void AfterGrabbing(const Pylon::CGrabResultPtr& ptrGrabResult);
};

/**
* �C�x���g�n���h��
*/
class CMainImageEventPrinter : public Pylon::CImageEventHandler
{
private:
	CameraBase* pCamera;
public:
	CMainImageEventPrinter(CameraBase* camera) {
		pCamera = camera;
	}

	virtual void OnImagesSkipped(Pylon::CInstantCamera& camera, size_t countOfSkippedImages)
	{
		std::cout << "OnImagesSkipped event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
		std::cout << countOfSkippedImages << " images have been skipped." << std::endl;
		std::cout << std::endl;
	}


	virtual void OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& ptrGrabResult)
	{
		std::cout << "OnImageGrabbed event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;

		// �J�����ɂ܂�����
		pCamera->AfterGrabbing(ptrGrabResult);

#if 0

		// Image grabbed successfully?
		if (ptrGrabResult->GrabSucceeded())
		{
			char buf[64];
			SYSTEMTIME lt;
			GetLocalTime(&lt);
			sprintf_s(buf, 64, "%04d%02d%02d_%02d%02d%02d_%03d", lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds);
			*/
				string buf;
			GetTimeString(&buf);

			std::cout << "Time: " << buf << std::endl;

			std::cout << "SizeX: " << ptrGrabResult->GetWidth() << std::endl;
			std::cout << "SizeY: " << ptrGrabResult->GetHeight() << std::endl;
			const uint8_t* pImageBuffer = (uint8_t*)ptrGrabResult->GetBuffer();
			//std::cout << "Gray value of first pixel: " << (uint32_t)pImageBuffer[0] << std::endl;
			std::cout << std::endl;

			//Pylon::DisplayImage(1, ptrGrabResult);

			// �t�@�C��������

			// TODO: �J�����ɂ��܂����H
			if (pCamera->IsGainDouble()) {
				sprintf_s(buf, 64, "%04d%02d%02d_%02d%02d%02d_%03d_G%3.1f_%dus.png",
					lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds,
					pCamera->GetDoubleGain(), (int)pCamera->GetDoubleExposureTime());
			}
			else {
				sprintf_s(buf, 64, "%04d%02d%02d_%02d%02d%02d_%03d_G%3.1f_%dus.png",
					lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds,
					(float)pCamera->GetIntGain(), (int)pCamera->GetDoubleExposureTime());
			}

			//printf("filename (%s)\n", buf);

			// �ۑ����邩
			if (!pCamera->IsSaved()) {
				// �O���[�o���ɃR�s�[
				memcpy_s(Gname_buffer, 64, buf, 64);

				std::thread th(threadSave, ptrGrabResult);
				th.detach();
				pCamera->ItSaved();
			}

			// 255�̃J�E���g
			int pix = 0;
			size_t size = ptrGrabResult->GetBufferSize();

			// ����9�����ă��W�A����255���ǂ�
			pix = count_median255(ptrGrabResult->GetWidth(), ptrGrabResult->GetHeight(), pImageBuffer);

			pCamera->SetPixel255(pix);
			std::cout << "buffer size: " << size << std::endl;
			std::cout << "255 pixs: " << pix << std::endl;
			std::cout << std::endl;

		}
		else
		{
			std::cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << std::endl;
		}
#endif
	}
};