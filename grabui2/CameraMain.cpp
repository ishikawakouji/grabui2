#include <thread>
#include "CameraMain.h"
#include "BaslerConfig.h"
#include "util.h"


void CameraMain::setConfig() {
	// �p�����[�^�ݒ�
	camera.RegisterConfiguration(new CSoftTriggerCustomize, Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_Delete);

	// �C�x���g�n���h���ݒ�
	camera.RegisterConfiguration(new CSampleConfigurationEventPrinter, Pylon::RegistrationMode_Append, Pylon::Cleanup_Delete);
	camera.RegisterImageEventHandler(new CMainImageEventPrinter(this), Pylon::RegistrationMode_Append, Pylon::Cleanup_Delete);
}

void CameraMain::Init()
{
	// preInit�ł���Ă���
	//CameraBase::Init();

	CameraBase::setNode();
#if 0 // setNode
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
#endif

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
		while (true)
		{
			// �V���b�^�[�҂�
			Sleep(1000);

			if (!caller->isGrabbing()) {
				break;
			}

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
	// ���łɋN�����Ȃ�Ȃɂ����Ȃ�
	if (isGrabbing()) {
		return;
	}

	try {
		// �f�o�C�X�ɓ`����
		camera.StartGrabbing(Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByInstantCamera);

		// �B�e��
		setGrabbing();

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
		if (isSave()) {
			string devName = camera.GetDeviceInfo().GetSerialNumber().c_str();
			string timeStamp;
			GetTimeString(&timeStamp);
			string gainStr = getGainString();
			string exTimeStr = getExposureString();

			string filename = devName + "_" + timeStamp + "_G" + gainStr + "_" + exTimeStr + ".png";
			if (filename.length() < FILE_NAME_LEN) {
				setSaveFileName(filename.c_str());
				startSaveImage();
			}
			else {
				cout << "file name too long, " << filename << endl;
			}

			unsetSave();
		}

	}
}