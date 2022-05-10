#include "CameraBase.h"

void CameraBase::setNode()
{
	// カメラオープン
	//camera.Open();

	// 露出時間
	doubleExposureTime.Attach(camera.GetNodeMap().GetNode("ExposureTime"));
	flagExposureTimeValid = doubleExposureTime.IsValid();
	if (flagExposureTimeValid) {
		exposureTimeCache = doubleExposureTime.GetValue();
	}
	else {
		exposureTimeCache = 1000.0;
	}

	// ゲインノード取得
	if (camera.GetSfncVersion() >= Pylon::Sfnc_2_0_0)
	{
		// double
		typeGain = GAIN_TYPE::GAIN_DOUBLE;

		doubleGain.Attach(camera.GetNodeMap().GetNode("Gain"));

		doubleGainMax = doubleGain.GetMax();
		doubleGainMin = doubleGain.GetMin();

		doubleGainCache = doubleGain.GetValue();
	}
	else
	{
		// long long
		typeGain = GAIN_TYPE::GAIN_INT64;

		intGain.Attach(camera.GetNodeMap().GetNode("GainRaw"));

		intGainMax = intGain.GetMax();
		intGainMin = intGain.GetMin();

		intGainCache = intGain.GetValue();
	}
}

string CameraBase::getGainMinMaxString()
{
	char buf[64];

	switch (typeGain)
	{
	case GAIN_TYPE::GAIN_DOUBLE:
		sprintf_s(buf, 64, "(%.1f/%.1f)", doubleGainMin, doubleGainMax);
		break;
	case GAIN_TYPE::GAIN_INT64:
		sprintf_s(buf, 64, "(%I64d/%I64d)", intGainMin, intGainMax);
		break;
	default:
		sprintf_s(buf, 64, "(N/A)");
		break;
	}

	return string(buf);
}

string CameraBase::getGainString()
{
	char buf[64];

	switch (typeGain)
	{
	case GAIN_TYPE::GAIN_DOUBLE:
		sprintf_s(buf, 64, "%.1f", GetDoubleGain());
		break;
	case GAIN_TYPE::GAIN_INT64:
		sprintf_s(buf, 64, "%I64d", GetIntGain());
		break;
	default:
		sprintf_s(buf, 64, "N/A");
		break;
	}

	return string(buf);
}

string CameraBase::getExposureString()
{
	char buf[64];

	sprintf_s(buf, 64, "%.1fus", GetDoubleExposureTime());
	return string(buf);
}

string CameraBase::getCount255String()
{
	char buf[64];
	sprintf_s(buf, 64, "%i", count255);
	return string(buf);
}

// 露出時間とゲインの調整
// 即時反映
void CameraBase::popupConfig(const char* winname)
{
	ImGui::SetNextWindowSize(ImVec2(0.0, 0.0));
	if (ImGui::BeginPopupModal(winname))
	{
		static double dGain;
		static int iGain;

		static double exposureTime;

		static bool init = true;

		if (init) {
			exposureTime = GetDoubleExposureTime();

			switch (typeGain)
			{
			case GAIN_TYPE::GAIN_DOUBLE:
				dGain = GetDoubleGain();
				break;
			case GAIN_TYPE::GAIN_INT64:
				iGain = (int)GetIntGain();
				break;
			default:
				iGain = 0;
				break;
			}
			init = false;
		}

		// 露出時間調整
		ImGui::Text(u8"露出時間"); ImGui::SameLine();
		char buf[64];
		sprintf_s(buf, 64, "%.1f/%.1f", minExTime, maxExTime);
		ImGui::Text(buf);
		
		ImGui::SameLine();
		if (ImGui::InputDouble("exTime", &exposureTime, 1.0, 10.0, "%0.1f", ImGuiInputTextFlags_EnterReturnsTrue))
		{
			exposureTime = SetDoubleExposureTime(exposureTime);
		}

		// ゲイン調整
		ImGui::Text(u8"ゲイン"); ImGui::SameLine();
		ImGui::Text(getGainMinMaxString().c_str());

		ImGui::SameLine();
		switch (typeGain)
		{
		case GAIN_TYPE::GAIN_DOUBLE:
			if (ImGui::InputDouble("dGain", &dGain, 0.5, 1.0, "%.1f", ImGuiInputTextFlags_EnterReturnsTrue))
			{
				dGain = SetDoubleGain(dGain);
			}
			break;
		case GAIN_TYPE::GAIN_INT64:
			if (ImGui::InputInt("iGain", &iGain, 1, 10, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				iGain = (int)SetIntGain(iGain);
			}
			break;
		default:
			ImGui::Text("N/A");
			break;
		}

		if (ImGui::Button("OK")) {
			// 即時反映なのでここでは不要
#if 0
			SetDoubleExposureTime(exposureTime);

			switch (typeGain)
			{
			case GAIN_TYPE::GAIN_DOUBLE:
				SetDoubleGain(dGain);
				break;
			case GAIN_TYPE::GAIN_INT64:
				SetIntGain((int64_t)iGain);
				break;
			default:
				iGain = 0;
				break;
			}
#endif
			// 次にオープンするときは設定値を読み込む
			init = true;

			ImGui::CloseCurrentPopup();
		}
#if 0
		ImGui::SameLine();
		if (ImGui::Button("cancel")) {
			exposureTime = GetDoubleExposureTime();

			switch (typeGain)
			{
			case GAIN_TYPE::GAIN_DOUBLE:
				dGain = GetDoubleGain();
				break;
			case GAIN_TYPE::GAIN_INT64:
				iGain = (int)GetIntGain();
				break;
			default:
				iGain = 0;
				break;
			}
			ImGui::CloseCurrentPopup();
		}
#endif

		ImGui::EndPopup();
	}

}
