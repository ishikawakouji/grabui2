#include <thread>
#include "CameraMain.h"
#include "BaslerConfig.h"
#include "util.h"


void CameraMain::setConfig() {
	// パラメータ設定
	camera.RegisterConfiguration(new CSoftTriggerCustomize, Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_Delete);

	// イベントハンドラ設定
	camera.RegisterConfiguration(new CSampleConfigurationEventPrinter, Pylon::RegistrationMode_Append, Pylon::Cleanup_Delete);
	camera.RegisterImageEventHandler(new CMainImageEventPrinter(this), Pylon::RegistrationMode_Append, Pylon::Cleanup_Delete);
}

void CameraMain::Init()
{
	// preInitでやっている
	//CameraBase::Init();

	CameraBase::setNode();
#if 0 // setNode
	// デバイスオープン
	// Open the camera for accessing the parameters.
	camera.Open();

	// ゲインノード取得
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

	// 露出時間
	doubleExposureTime.Attach(camera.GetNodeMap().GetNode("ExposureTime"));
	flagExposureTimeValid = doubleExposureTime.IsValid();
#endif

	// 255個数リセット
	pixel255 = 0;

}

// 撮影スレッド、クラスの中には無い
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
			// シャッター待ち
			Sleep(1000);

			if (!caller->isGrabbing()) {
				break;
			}

			// シャッタートリガ
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
	// すでに起動中ならなにもしない
	if (isGrabbing()) {
		return;
	}

	try {
		// デバイスに伝える
		camera.StartGrabbing(Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByInstantCamera);

		// 撮影中
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
		// データ取得失敗
		std::cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << std::endl;
	}
	else {
		// データ取得した
		PrintTime();

		// 画像の状態
		uint32_t w = ptrGrabResult->GetWidth();
		uint32_t h = ptrGrabResult->GetHeight();
		std::cout << "SizeX: " << w << std::endl;
		std::cout << "SizeY: " << h << std::endl;

		// キャッシュする
		const uint8_t* pImage = (uint8_t*)ptrGrabResult->GetBuffer();
		image.CacheImage(w, h, 1, pImage);
		//image->CopyToGpu(w, h, 1, pImage);

		// 255カウント

		// 255調整

		// 保存
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