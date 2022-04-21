#include <thread>
#include "CameraSub.h"
#include "BaslerConfig.h"
#include "util.h"

// Line1 �p�ݒ�
void CameraSub::setConfig() {
	// �p�����[�^�ݒ�
	camera.RegisterConfiguration(new CLine1TriggerCustomize, Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_Delete);

	// �C�x���g�n���h���ݒ�
	camera.RegisterConfiguration(new CSampleConfigurationEventPrinter, Pylon::RegistrationMode_Append, Pylon::Cleanup_Delete);
	camera.RegisterImageEventHandler(new CMainImageEventPrinter(this), Pylon::RegistrationMode_Append, Pylon::Cleanup_Delete);
}
