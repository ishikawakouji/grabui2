#include "CameraBase.h"

void CameraBase::setNode()
{
	// ˜IoŽžŠÔ
	doubleExposureTime.Attach(camera.GetNodeMap().GetNode("ExposureTime"));
	flagExposureTimeValid = doubleExposureTime.IsValid();

	// ƒQƒCƒ“ƒm[ƒhŽæ“¾
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

	sprintf_s(buf, 64, "%.1f us", GetDoubleExposureTime());
	return string(buf);
}

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

		// ˜IoŽžŠÔ’²®
		ImGui::Text(u8"˜IoŽžŠÔ"); ImGui::SameLine();
		ImGui::Text("(500/1000)");
		
		ImGui::SameLine();
		ImGui::InputDouble("double", &exposureTime, 0.0, 0.0, "%0.1f", ImGuiInputTextFlags_EnterReturnsTrue);
		if (exposureTime > 1000.0f) {
			exposureTime = 1000.0f;
		}
		if (exposureTime < 500.0f) {
			exposureTime = 500.0f;
		}

		// ƒQƒCƒ“’²®
		ImGui::Text(u8"ƒQƒCƒ“"); ImGui::SameLine();
		ImGui::Text(getGainMinMaxString().c_str());

		ImGui::SameLine();
		switch (typeGain)
		{
		case GAIN_TYPE::GAIN_DOUBLE:
			ImGui::InputDouble("double", &dGain, 0.0, 0.0, "%.1f", ImGuiInputTextFlags_EnterReturnsTrue);
			if (dGain > GetDoubleGainMax()) {
				dGain = GetDoubleGainMax();
			}
			if (dGain < GetDoubleGainMin()) {
				dGain = GetDoubleGainMin();
			}
			break;
		case GAIN_TYPE::GAIN_INT64:
			ImGui::InputInt("int", &iGain, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue);
			if (iGain > GetIntGainMax()) {
				iGain = GetIntGainMax();
			}
			if (iGain < GetIntGainMin()) {
				iGain = GetIntGainMin();
			}
			break;
		default:
			ImGui::Text("N/A");
			break;
		}

		if (ImGui::Button("OK")) {
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
			ImGui::CloseCurrentPopup();
		}
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

		ImGui::EndPopup();
	}

}
