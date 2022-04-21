#include <thread>
#include "CameraSub.h"
#include "BaslerConfig.h"
#include "util.h"

// Line1 用設定
void CameraSub::setConfig() {
	// パラメータ設定
	camera.RegisterConfiguration(new CLine1TriggerCustomize, Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_Delete);

	// イベントハンドラ設定
	camera.RegisterConfiguration(new CSampleConfigurationEventPrinter, Pylon::RegistrationMode_Append, Pylon::Cleanup_Delete);
	camera.RegisterImageEventHandler(new CMainImageEventPrinter(this), Pylon::RegistrationMode_Append, Pylon::Cleanup_Delete);
}
