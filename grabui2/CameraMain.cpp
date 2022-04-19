#include <thread>
#include "CameraMain.h"
#include "BaslerConfig.h"
#include "util.h"

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


void CameraMain::Init()
{
	CameraBase::Init();

	// �p�����[�^�ݒ�
	camera.RegisterConfiguration(new CSoftTriggerCustomize, Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_Delete);

	// �C�x���g�n���h���ݒ�
	camera.RegisterConfiguration(new CSampleConfigurationEventPrinter, Pylon::RegistrationMode_Append, Pylon::Cleanup_Delete);
	camera.RegisterImageEventHandler(new CMainImageEventPrinter(this), Pylon::RegistrationMode_Append, Pylon::Cleanup_Delete);

	// �f�o�C�X�I�[�v��
	// Open the camera for accessing the parameters.
	camera.Open();

	// �Q�C���m�[�h�擾
	if (camera.GetSfncVersion() >= Pylon::Sfnc_2_0_0)
	{
		// double
		typeGain = GAIN_TYPE::GAIN_DOUBLE;

		doubleGain.Attach(camera.GetNodeMap().GetNode("Gain"));

		doubleGainMax = doubleGain.GetMax();
		doubleGainMin = doubleGain.GetMin();
	}
	else
	{
		// long long
		typeGain = GAIN_TYPE::GAIN_INT64;

		intGain.Attach(camera.GetNodeMap().GetNode("GainRaw"));

		intGainMax = intGain.GetMax();
		intGainMin = intGain.GetMin();
	}

	// �I�o����
	doubleExposureTime.Attach(camera.GetNodeMap().GetNode("ExposureTime"));
	flagExposureTimeValid = doubleExposureTime.IsValid();

	// 255�����Z�b�g
	pixel255 = 0;

}

// �B�e�X���b�h�A�N���X�̒��ɂ͖���
void threadMainGrabbing(CameraMain* caller)
{
	// Can the camera device be queried whether it is ready to accept the next frame trigger?
	if (caller->CanWaitForFrameTriggerReady())
	{
		// Start the grabbing using the grab loop thread, by setting the grabLoopType parameter
		// to GrabLoop_ProvidedByInstantCamera. The grab results are delivered to the image event handlers.
		// The GrabStrategy_OneByOne default grab strategy is used.
		//camera->StartGrabbing(Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByInstantCamera);

		// Wait for user input to trigger the camera or exit the program.
		// The grabbing is stopped, the device is closed and destroyed automatically when the camera object goes out of scope.
		while (caller->isGrabbing())
		{
			// �V���b�^�[�҂�
			Sleep(1000);

			// �V���b�^�[�g���K
			// Execute the software trigger. Wait up to 1000 ms for the camera to be ready for trigger.
			if (caller->WaitForFrameTriggerReady(100, Pylon::TimeoutHandling_ThrowException))
			{
				caller->ExecuteSoftwareTrigger();
			}

		};
	}

};

void CameraMain::StartGrabbing()
{
	try {
		// �f�o�C�X�ɓ`����
		camera.StartGrabbing(Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByInstantCamera);

		std::thread th(threadMainGrabbing, this);
		th.detach();
	}
	catch (const Pylon::GenericException& e)
	{
		// Error handling.
		std::cerr << "An exception occurred." << std::endl
			<< e.GetDescription() << std::endl;
	}
}
void CameraMain::StopGrabbing()
{
	unsetGrabbing();
}

void CameraMain::AfterGrabbing(const Pylon::CGrabResultPtr& ptrGrabResult)
{
	if (!ptrGrabResult->GrabSucceeded()) {
		// �f�[�^�擾���s
		std::cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << std::endl;
	}
	else {
		// �f�[�^�擾����
		PrintTime();

		// �摜�̏��
		uint32_t w = ptrGrabResult->GetWidth();
		uint32_t h = ptrGrabResult->GetHeight();
		std::cout << "SizeX: " << w << std::endl;
		std::cout << "SizeY: " << h << std::endl;

		// �L���b�V������
		const uint8_t* pImage = (uint8_t*)ptrGrabResult->GetBuffer();
		image.CacheImage(w, h, 1, pImage);
		//image->CopyToGpu(w, h, 1, pImage);

		// 255�J�E���g

		// 255����

		// �ۑ�

	}
}