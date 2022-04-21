#pragma once
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "util.h"

#include "TemplateParameterSet.h"
#include "CameraBase.h"
#include "Line1TriggerConfiguration.h"

class ColorHosei {
private:

	// �ő�o�b�t�@��
	const int iMaxNumBuffer = 10;

#if 1
	// ���C�}�b�N���i�œ����A���^#2
	// RGB�Ɩ�(B128,B3-118,R128,G255)  acA2440�@2022.02.10
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
	// ���C�}�b�N ��LED 100%  acA2440  �Ώ��Ȃ��@2021.12.03
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
	// �F�␳ 11/2�C��
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
	void Config(GENAPI_NAMESPACE::INodeMap& nodemap) {
		using namespace Pylon;

		// �I�����Ԃ̎w��i0�̏ꍇ�͕ύX���Ȃ�
		SetValue<CEnumParameter, const char*>(nodemap, "ExposureMode", "Timed");
		SetValue<CFloatParameter, double>(nodemap, "ExposureTime", 1000.0);

		// �s�N�Z���t�H�[�}�b�g
		SetValue<CEnumParameter, const char*>(nodemap, "PixelFormat", "BayerRG8");

		// acquisition start trigger�@�\�𖳌����́A���Ȃ�

		// �f���U�C�N���[�h��BaslerPGI�ABayer�ȊO�łȂ��Ɛݒ�ł��Ȃ�
		TrySetValue<CEnumParameter, const char*>(nodemap, "DemosaicingMode", "BaslerPGI");

		// LightSourcePreset
		if (SetValue<CEnumParameter, const char*>(nodemap, "LightSourcePreset", "Off"))
		{
			//CEnumParameter(nodemap, "BalanceWhiteAuto").SetValue("Off");

			// OFF �ɂł����̂�
			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>(nodemap, "BalanceRatioSelector", "Blue", "BalanceRatio", 1.0);
			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>(nodemap, "BalanceRatioSelector", "Green", "BalanceRatio", 1.0);
			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>(nodemap, "BalanceRatioSelector", "Red", "BalanceRatio", 1.0);

			// �z���C�g�o�����X��C�Ӓ���
			// �����p�}�g���N�X�v�Z
			double lightMatrix[3][3] = {
				{ RLight_R / 255., GLight_R / 255., BLight_R / 255. },
				{ RLight_G / 255., GLight_G / 255., BLight_G / 255. },
				{ RLight_B / 255., GLight_B / 255., BLight_B / 255. }
			};

			// �t�s��
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

		// �Q�C��
		SetValue<CEnumParameter, const char*>(nodemap, "GainAuto", "Off");
		SetValue<CEnumParameter, const char*>(nodemap, "GainSelector", "All");

		// �����o��
		CEnumParameter(nodemap, "LineSelector").SetValue("Line2");
		CEnumParameter(nodemap, "LineSource").SetValue("ExposureActive");

#if 0
		// ��������
		CEnumParameter(nodemap, "TriggerSelector").SetValue("FrameStart");
		CEnumParameter(nodemap, "TriggerMode").SetValue("On");
		CEnumParameter(nodemap, "TriggerSource").SetValue("Line1");
		CBooleanParameter(nodemap, "LineInverter").SetValue(false);
#endif
	}
};

/**
* �R���t�B�O
*/
class  CSoftTriggerCustomize : public Pylon::CSoftwareTriggerConfiguration {

private:

	// �ő�o�b�t�@��
	const int iMaxNumBuffer = 10;

#if 0
	// ���C�}�b�N���i�œ����A���^#2
	// RGB�Ɩ�(B128,B3-118,R128,G255)  acA2440�@2022.02.10
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
	// ���C�}�b�N ��LED 100%  acA2440  �Ώ��Ȃ��@2021.12.03
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
	// �F�␳ 11/2�C��
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

	ColorHosei hosei;

public:
	//static 
	void ApplyConfiguration(GENAPI_NAMESPACE::INodeMap& nodemap) {

		__super::ApplyConfiguration(nodemap);

		std::cout << "OnOpened -> ApplyConfiguration SoftTrigger" << std::endl;

		hosei.Config(nodemap);

#if 0

		using namespace Pylon;

		// �I�����Ԃ̎w��i0�̏ꍇ�͕ύX���Ȃ�
		SetValue<CEnumParameter, const char*>(nodemap, "ExposureMode", "Timed");
		SetValue<CFloatParameter, double>(nodemap, "ExposureTime", 1000.0);

		// �s�N�Z���t�H�[�}�b�g
		SetValue<CEnumParameter, const char*>(nodemap, "PixelFormat", "BayerRG8");

		// acquisition start trigger�@�\�𖳌����́A���Ȃ�

		// �f���U�C�N���[�h��BaslerPGI�ABayer�ȊO�łȂ��Ɛݒ�ł��Ȃ�
		TrySetValue<CEnumParameter, const char*>(nodemap, "DemosaicingMode", "BaslerPGI");

		// LightSourcePreset
		if (SetValue<CEnumParameter, const char*>(nodemap, "LightSourcePreset", "Off"))
		{
			//CEnumParameter(nodemap, "BalanceWhiteAuto").SetValue("Off");

			// OFF �ɂł����̂�
			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>(nodemap, "BalanceRatioSelector", "Blue", "BalanceRatio", 1.0);
			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>(nodemap, "BalanceRatioSelector", "Green", "BalanceRatio", 1.0);
			SelectAndSetValue<CEnumParameter, const char*, CFloatParameter, double>(nodemap, "BalanceRatioSelector", "Red", "BalanceRatio", 1.0);

			// �z���C�g�o�����X��C�Ӓ���
			// �����p�}�g���N�X�v�Z
			double lightMatrix[3][3] = {
				{ RLight_R / 255., GLight_R / 255., BLight_R / 255. },
				{ RLight_G / 255., GLight_G / 255., BLight_G / 255. },
				{ RLight_B / 255., GLight_B / 255., BLight_B / 255. }
			};

			// �t�s��
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

		// �Q�C��
		SetValue<CEnumParameter, const char*>(nodemap, "GainAuto", "Off");
		SetValue<CEnumParameter, const char*>(nodemap, "GainSelector", "All");

		// �����o��
		CEnumParameter(nodemap, "LineSelector").SetValue("Line2");
		CEnumParameter(nodemap, "LineSource").SetValue("ExposureActive");
#endif

#if 0
		// ��������
		CEnumParameter(nodemap, "TriggerSelector").SetValue("FrameStart");
		CEnumParameter(nodemap, "TriggerMode").SetValue("On");
		CEnumParameter(nodemap, "TriggerSource").SetValue("Line1");
		CBooleanParameter(nodemap, "LineInverter").SetValue(false);
#endif

	}

	void ApplyInstantCameraParameter(Pylon::CInstantCamera& camera) {
		std::cout << "OnOpened -> ApplyInstantParameter" << std::endl;

		// �ő�o�b�t�@��
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

// Line1 Trigger
class CLine1TriggerCustomize : public CLine1TriggerConfiguration {
private:
	// �ő�o�b�t�@��
	const int iMaxNumBuffer = 10;

	ColorHosei hosei;

public:
	//static 
	void ApplyConfiguration(GENAPI_NAMESPACE::INodeMap& nodemap) {
		__super::ApplyConfiguration(nodemap);

		std::cout << "OnOpened -> ApplyConfiguration Line1" << std::endl;

		using namespace Pylon;

		// �������͂֏�������
		CEnumParameter(nodemap, "TriggerSelector").SetValue("FrameStart");
		CEnumParameter(nodemap, "TriggerMode").SetValue("On");
		CEnumParameter(nodemap, "TriggerSource").SetValue("Line1");
		CBooleanParameter(nodemap, "LineInverter").SetValue(false);

		hosei.Config(nodemap);
	}

	void ApplyInstantCameraParameter(Pylon::CInstantCamera& camera) {
		std::cout << "OnOpened -> ApplyInstantParameter" << std::endl;

		// �ő�o�b�t�@��
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

