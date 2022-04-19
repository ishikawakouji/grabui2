#pragma once
#include <stdio.h>
#include "util.h"

#include <pylon/PylonIncludes.h>
#include "TemplateParameterSet.h"

/**
* エミュレータ用コンフィグ
*/
class  CEmuSoftTriggerCustomize : public Pylon::CSoftwareTriggerConfiguration {

private:

	// 最大バッファ数
	const int iMaxNumBuffer = 10;

public:
	//static 
	void ApplyConfiguration(GENAPI_NAMESPACE::INodeMap& nodemap) {

		__super::ApplyConfiguration(nodemap);

		std::cout << "OnOpened -> ApplyConfiguration" << std::endl;

		using namespace Pylon;

		// 露光時間の指定（0の場合は変更しない
		SetValue<CEnumParameter, const char*>(nodemap, "ExposureMode", "Timed");
		SetValue<CFloatParameter, double>(nodemap, "ExposureTime", 1000.0);

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

class CEmuSampleConfigurationEventPrinter : public Pylon::CConfigurationEventHandler
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

