#pragma once
#include "CameraBase.h"

class CameraMain : public CameraBase
{
private:
	bool fisMain = true;
	bool fcanTrigger = true;

	// 255値になったピクセルの個数
	int pixel255 = 0;

public:
	void unsetMain() { fisMain = false; }
	bool isMain() { return fisMain;  }

	void continueTrigger() {
		fcanTrigger = true;
	}
	void suspendTrigger() { fcanTrigger = false; }
	bool canTrigger() { return fcanTrigger; }

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

	// 撮影モードとスレッド起動、スレッド終了
	void StartGrabbing();

	// 撮影後の処理
	void AfterGrabbing(const Pylon::CGrabResultPtr& ptrGrabResult);

#ifdef DEBUG_VIEW
public:
	// デバッグ用のimage view
	BufferedImage debugImage;
	void DrawDebugImage() {
		string dwinname = string("debug_") + camera.GetDeviceInfo().GetSerialNumber().c_str();
		ImGui::Begin(dwinname.c_str());
		{
			debugImage.DrawImage();
		}
		ImGui::End();
	}

#endif
};

/**
* イベントハンドラ
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

		// カメラにまかせる
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

			// ファイル名準備

			// TODO: カメラにおまかせ？
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

			// 保存するか
			if (!pCamera->IsSaved()) {
				// グローバルにコピー
				memcpy_s(Gname_buffer, 64, buf, 64);

				std::thread th(threadSave, ptrGrabResult);
				th.detach();
				pCamera->ItSaved();
			}

			// 255のカウント
			int pix = 0;
			size_t size = ptrGrabResult->GetBufferSize();

			// 周囲9つを見てメジアンが255かどか
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