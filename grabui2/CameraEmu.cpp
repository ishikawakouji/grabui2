#include <thread>
#include "CameraEmu.h"
#include "EmuConfig.h"
#include "util.h"

#include <pylon/PylonGUIIncludes.h>
/**
* イベントハンドラ
*/
class CEmuImageEventPrinter : public Pylon::CImageEventHandler
{
private:
	CameraBase* pCamera;
public:
	CEmuImageEventPrinter(CameraBase* camera) {
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

		// カメラにまかせる
		pCamera->AfterGrabbing(ptrGrabResult);
	}
};

void CameraEmu::Init()
{
	CameraBase::Init();

	// パラメータ設定
	camera.RegisterConfiguration(new CEmuSoftTriggerCustomize, Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_Delete);

	// イベントハンドラ設定
	camera.RegisterConfiguration(new CEmuSampleConfigurationEventPrinter, Pylon::RegistrationMode_Append, Pylon::Cleanup_Delete);
	camera.RegisterImageEventHandler(new CEmuImageEventPrinter(this), Pylon::RegistrationMode_Append, Pylon::Cleanup_Delete);

	// デバイスオープン
	// Open the camera for accessing the parameters.
	camera.Open();
}

// 撮影スレッド、クラスの中には無い
void threadEmuGrabbing(CameraEmu* caller)
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

void CameraEmu::StartGrabbing()
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

		std::thread th(threadEmuGrabbing, this);
		th.detach();
	}
	catch (const Pylon::GenericException& e)
	{
		// Error handling.
		std::cerr << "An exception occurred." << std::endl
			<< e.GetDescription() << std::endl;
	}
}

void CameraEmu::AfterGrabbing(const Pylon::CGrabResultPtr& ptrGrabResult)
{
	if (!ptrGrabResult->GrabSucceeded()) {
		// データ取得失敗
		std::cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << std::endl;
	}
	else {
		// データ取得した
		PrintTime();

		//Pylon::DisplayImage(1, ptrGrabResult);

		// 画像の状態
		uint32_t w = ptrGrabResult->GetWidth();
		uint32_t h = ptrGrabResult->GetHeight();
		std::cout << "SizeX: " << w << std::endl;
		std::cout << "SizeY: " << h << std::endl;
		std::cout << "size " << ptrGrabResult->GetBufferSize() << std::endl;

		// 一度キャッシュ
		const uint8_t* pImage = (uint8_t*)ptrGrabResult->GetBuffer();
		std::cout << "dot " << (uint32_t)pImage[0] << std::endl;
		image.CacheImage(w, h, 1, pImage);
		//image->CopyToGpu(w, h, 1, pImage);
	}
}