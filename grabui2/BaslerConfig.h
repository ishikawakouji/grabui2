#pragma once
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "util.h"

#include "TemplateParameterSet.h"
#include "CameraBase.h"

/**
* 露出オーバーのカウント
*/
// 単純なカウント
int count255(size_t size, const uint8_t* pImageBuffer) {
	int pix = 0;
	for (size_t i = 0; i < size; ++i) {
		if (pImageBuffer[i] == 255) pix++;
	}
	return pix;
}

// 周囲を見て中央値が255になるものをカウント
int count_median255(uint32_t width, uint32_t height, const uint8_t* pImageBuffer) {
	int pix = 0;

	// GR のブロックを一度に見るので 2つずつ移動
	// BG
	for (uint32_t h = 2; h < height - 2; h += 2) {
		for (uint32_t w = 2; w < width - 2; w += 2) {

			// G1
			int count = 0;
			int G = width * h + w;
			if (pImageBuffer[G - 2 * width] == 255) ++count;
			if (pImageBuffer[G - width + 1] == 255) ++count;
			if (pImageBuffer[G - width - 1] == 255) ++count;
			if (pImageBuffer[G - 2] == 255) ++count;
			if (pImageBuffer[G] == 255) ++count;
			if (pImageBuffer[G + 2] == 255) ++count;
			if (pImageBuffer[G + width - 1] == 255) ++count;
			if (pImageBuffer[G + width + 1] == 255) ++count;
			if (pImageBuffer[G + 2 * width] == 255) ++count;

			if (count > 4) ++pix;

			// R
			count = 0;
			int R = G + 1;
			if (pImageBuffer[R - 2 * width] == 255) ++count;
			if (pImageBuffer[R - 2 * width - 2] == 255) ++count;
			if (pImageBuffer[R - 2 * width + 2] == 255) ++count;
			if (pImageBuffer[R - 2] == 255) ++count;
			if (pImageBuffer[R] == 255) ++count;
			if (pImageBuffer[R + 2] == 255) ++count;
			if (pImageBuffer[R + 2 * width - 2] == 255) ++count;
			if (pImageBuffer[R + 2 * width + 2] == 255) ++count;
			if (pImageBuffer[R + 2 * width] == 255) ++count;

			if (count > 4) ++pix;

			// B
			count = 0;
			int B = G + width;
			if (pImageBuffer[B - 2 * width] == 255) ++count;
			if (pImageBuffer[B - 2 * width - 2] == 255) ++count;
			if (pImageBuffer[B - 2 * width + 2] == 255) ++count;
			if (pImageBuffer[B - 2] == 255) ++count;
			if (pImageBuffer[B] == 255) ++count;
			if (pImageBuffer[B + 2] == 255) ++count;
			if (pImageBuffer[B + 2 * width - 2] == 255) ++count;
			if (pImageBuffer[B + 2 * width + 2] == 255) ++count;
			if (pImageBuffer[B + 2 * width] == 255) ++count;

			if (count > 4) ++pix;

			// G2
			count = 0;
			G = B + 1;
			if (pImageBuffer[G - 2 * width] == 255) ++count;
			if (pImageBuffer[G - width + 1] == 255) ++count;
			if (pImageBuffer[G - width - 1] == 255) ++count;
			if (pImageBuffer[G - 2] == 255) ++count;
			if (pImageBuffer[G] == 255) ++count;
			if (pImageBuffer[G + 2] == 255) ++count;
			if (pImageBuffer[G + width - 1] == 255) ++count;
			if (pImageBuffer[G + width + 1] == 255) ++count;
			if (pImageBuffer[G + 2 * width] == 255) ++count;

			if (count > 4) ++pix;

		} // width + 2
	} // height + 2

	return pix;
}


/**
* コンフィグ
*/
class  CSoftTriggerCustomize : public Pylon::CSoftwareTriggerConfiguration {

private:

	// 最大バッファ数
	const int iMaxNumBuffer = 10;

#if 1
	// レイマック部品で内製、小型#2
	// RGB照明(B128,B3-118,R128,G255)  acA2440　2022.02.10
	const double BGRLight_Rate = 0.504012522;  // 255 / (BLight_G * BLight_Rate + GLight_G * GLight_Rate + RLight_G * RLight_Rate)
	const double BLight_Rate = 255.0 / 211.0 * 1.0;
	const double GLight_Rate = 255.0 / 243.0 * 1.35;
	const double RLight_Rate = 255.0 / 204.0 * 0.98;
	const double BLight_B = 211 * BLight_Rate * BGRLight_Rate;
	const double BLight_G = 75 * BLight_Rate * BGRLight_Rate;
	const double BLight_R = 5 * BLight_Rate * BGRLight_Rate;
	const double GLight_B = 57 * GLight_Rate * BGRLight_Rate;
	const double GLight_G = 243 * GLight_Rate * BGRLight_Rate;
	const double GLight_R = 15 * GLight_Rate * BGRLight_Rate;
	const double RLight_B = 5 * RLight_Rate * BGRLight_Rate;
	const double RLight_G = 58 * RLight_Rate * BGRLight_Rate;
	const double RLight_R = 204 * RLight_Rate * BGRLight_Rate;
#endif

#if 0
	// レイマック 面LED 100%  acA2440  緑少なく　2021.12.03
	const double BGRLight_Rate = 0.76506;  // 255 / (BLight_G * BLight_Rate + GLight_G * GLight_Rate + RLight_G * RLight_Rate)
	const double BLight_Rate = 255.0 / 211.0 * 0.48;
	const double GLight_Rate = 255.0 / 243.0;
	const double RLight_Rate = 255.0 / 204.0 * 0.48;
	const double BLight_B = 211 * BLight_Rate * BGRLight_Rate;
	const double BLight_G = 75 * BLight_Rate * BGRLight_Rate;
	const double BLight_R = 5 * BLight_Rate * BGRLight_Rate;
	const double GLight_B = 57 * GLight_Rate * BGRLight_Rate;
	const double GLight_G = 243 * GLight_Rate * BGRLight_Rate;
	const double GLight_R = 15 * GLight_Rate * BGRLight_Rate;
	const double RLight_B = 5 * RLight_Rate * BGRLight_Rate;
	const double RLight_G = 58 * RLight_Rate * BGRLight_Rate;
	const double RLight_R = 204 * RLight_Rate * BGRLight_Rate;
#endif

#if 0
	// 色補正 11/2修正
	const double BGRLight_Rate = 0.7298;  // 255 / (BLight_G * BLight_Rate + GLight_G * GLight_Rate + RLight_G * RLight_Rate)
	const double BLight_Rate = 0.8;
	const double GLight_Rate = 1.0;
	const double RLight_Rate = 0.8;
	const double BLight_B = 211 * BLight_Rate * BGRLight_Rate;
	const double BLight_G = 75 * BLight_Rate * BGRLight_Rate;
	const double BLight_R = 5 * BLight_Rate * BGRLight_Rate;
	const double GLight_B = 57 * GLight_Rate * BGRLight_Rate;
	const double GLight_G = 243 * GLight_Rate * BGRLight_Rate;
	const double GLight_R = 15 * GLight_Rate * BGRLight_Rate;
	const double RLight_B = 5 * RLight_Rate * BGRLight_Rate;
	const double RLight_G = 58 * RLight_Rate * BGRLight_Rate;
	const double RLight_R = 204 * RLight_Rate * BGRLight_Rate;
#endif

public:
	//static 
	void ApplyConfiguration(GENAPI_NAMESPACE::INodeMap& nodemap) {

		__super::ApplyConfiguration(nodemap);

		std::cout << "OnOpened -> ApplyConfiguration" << std::endl;

		using namespace Pylon;

		// 露光時間の指定（0の場合は変更しない
		SetValue<CEnumParameter, const char*>(nodemap, "ExposureMode", "Timed");
		SetValue<CFloatParameter, double>(nodemap, "ExposureTime", 1000.0);

		// ピクセルフォーマット
		SetValue<CEnumParameter, const char*>(nodemap, "PixelFormat", "BayerRG8");

		// acquisition start trigger機能を無効化は、しない

		// デモザイクモードでBaslerPGI、Bayer以外でないと設定できない
		TrySetValue<CEnumParameter, const char*>(nodemap, "DemosaicingMode", "BaslerPGI");

		// LightSourcePreset
		if (SetValue<CEnumParameter, const char*>(nodemap, "LightSourcePreset", "Off"))
		{
			//CEnumParameter(nodemap, "BalanceWhiteAuto").SetValue("Off");

			// OFF にできたので
			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>(nodemap, "BalanceRatioSelector", "Blue", "BalanceRatio", 1.0);
			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>(nodemap, "BalanceRatioSelector", "Green", "BalanceRatio", 1.0);
			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>(nodemap, "BalanceRatioSelector", "Red", "BalanceRatio", 1.0);

			// ホワイトバランスを任意調整
			// 調整用マトリクス計算
			double lightMatrix[3][3] = {
				{ RLight_R / 255., GLight_R / 255., BLight_R / 255. },
				{ RLight_G / 255., GLight_G / 255., BLight_G / 255. },
				{ RLight_B / 255., GLight_B / 255., BLight_B / 255. }
			};

			// 逆行列
			cv::Mat invLightMat = cv::Mat(3, 3, CV_64F, lightMatrix).inv();

			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>
				(nodemap, "ColorTransformationValueSelector", "Gain00", "ColorTransformationValue", invLightMat.at<double>(0, 0));
			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>
				(nodemap, "ColorTransformationValueSelector", "Gain01", "ColorTransformationValue", invLightMat.at<double>(0, 1));
			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>
				(nodemap, "ColorTransformationValueSelector", "Gain02", "ColorTransformationValue", invLightMat.at<double>(0, 2));
			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>
				(nodemap, "ColorTransformationValueSelector", "Gain10", "ColorTransformationValue", invLightMat.at<double>(1, 0));
			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>
				(nodemap, "ColorTransformationValueSelector", "Gain11", "ColorTransformationValue", invLightMat.at<double>(1, 1));
			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>
				(nodemap, "ColorTransformationValueSelector", "Gain12", "ColorTransformationValue", invLightMat.at<double>(1, 2));
			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>
				(nodemap, "ColorTransformationValueSelector", "Gain20", "ColorTransformationValue", invLightMat.at<double>(2, 0));
			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>
				(nodemap, "ColorTransformationValueSelector", "Gain21", "ColorTransformationValue", invLightMat.at<double>(2, 1));
			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>
				(nodemap, "ColorTransformationValueSelector", "Gain22", "ColorTransformationValue", invLightMat.at<double>(2, 2));

		}

		// ゲイン
		SetValue<CEnumParameter, const char*>(nodemap, "GainAuto", "Off");
		SetValue<CEnumParameter, const char*>(nodemap, "GainSelector", "All");

		// 同期出力
		CEnumParameter(nodemap, "LineSelector").SetValue("Line2");
		CEnumParameter(nodemap, "LineSource").SetValue("ExposureActive");

		// 同期入力
		CEnumParameter(nodemap, "TriggerSelector").SetValue("FrameStart");
		CEnumParameter(nodemap, "TriggerMode").SetValue("On");
		CEnumParameter(nodemap, "TriggerSource").SetValue("Line1");

	}

	void ApplyInstantCameraParameter(Pylon::CInstantCamera& camera) {
		std::cout << "OnOpened -> ApplyInstantParameter" << std::endl;

		// 最大バッファ数
		camera.MaxNumBuffer = iMaxNumBuffer;


	}

	//Set basic camera settings.
	virtual void OnOpened(Pylon::CInstantCamera& camera)
	{
		try
		{
			ApplyInstantCameraParameter(camera);
			ApplyConfiguration(camera.GetNodeMap());
		}
		catch (const Pylon::GenericException& e)
		{
			throw RUNTIME_EXCEPTION("Could not apply configuration. Pylon::GenericException caught in OnOpened method msg=%hs", e.what());
		}
		catch (const std::exception& e)
		{
			throw RUNTIME_EXCEPTION("Could not apply configuration. std::exception caught in OnOpened method msg=%hs", e.what());
		}
		catch (...)
		{
			throw RUNTIME_EXCEPTION("Could not apply configuration. Unknown exception caught in OnOpened method.");
		}
	}
};

class CSampleConfigurationEventPrinter : public Pylon::CConfigurationEventHandler
{
public:
	void OnAttach(Pylon::CInstantCamera& /*camera*/)
	{
		std::cout << "OnAttach event" << std::endl;
	}

	void OnAttached(Pylon::CInstantCamera& camera)
	{
		std::cout << "OnAttached event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
	}

	void OnOpen(Pylon::CInstantCamera& camera)
	{
		std::cout << "OnOpen event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
	}

	void OnOpened(Pylon::CInstantCamera& camera)
	{
		std::cout << "OnOpened event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
	}

	void OnGrabStart(Pylon::CInstantCamera& camera)
	{
		std::cout << "OnGrabStart event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
	}

	void OnGrabStarted(Pylon::CInstantCamera& camera)
	{
		std::cout << "OnGrabStarted event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
	}

	void OnGrabStop(Pylon::CInstantCamera& camera)
	{
		std::cout << "OnGrabStop event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
	}

	void OnGrabStopped(Pylon::CInstantCamera& camera)
	{
		std::cout << "OnGrabStopped event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
	}

	void OnClose(Pylon::CInstantCamera& camera)
	{
		std::cout << "OnClose event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
	}

	void OnClosed(Pylon::CInstantCamera& camera)
	{
		std::cout << "OnClosed event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
	}

	void OnDestroy(Pylon::CInstantCamera& camera)
	{
		std::cout << "OnDestroy event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
	}

	void OnDestroyed(Pylon::CInstantCamera& /*camera*/)
	{
		std::cout << "OnDestroyed event" << std::endl;
	}

	void OnDetach(Pylon::CInstantCamera& camera)
	{
		std::cout << "OnDetach event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
	}

	void OnDetached(Pylon::CInstantCamera& camera)
	{
		std::cout << "OnDetached event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
	}

	void OnGrabError(Pylon::CInstantCamera& camera, const char* errorMessage)
	{
		std::cout << "OnGrabError event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
		std::cout << "Error Message: " << errorMessage << std::endl;
	}

	void OnCameraDeviceRemoved(Pylon::CInstantCamera& camera)
	{
		std::cout << "OnCameraDeviceRemoved event for device " << camera.GetDeviceInfo().GetModelName() << std::endl;
	}
};

