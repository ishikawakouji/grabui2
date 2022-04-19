#pragma once
#include <string>
#include <pylon/PylonIncludes.h>

#include "BufferedImage.h"

using namespace Pylon;

class CameraBase
{
private:
	// �B�e�����ǂ���
	bool fgrabbing = false;
	 
	// �ۑ��t���O
	bool fsave = false;

	// 255�J�E���g
	bool fcount255 = false;

	// 255����
	bool ftune255 = false;

public:
	// �B�e���t���O
	bool isGrabbing() { return fgrabbing; }
	void setGrabbing() { fgrabbing = true; }
	void unsetGrabbing() { fgrabbing = false; }

	// �ۑ��t���O
	bool isSave() { return fsave; }
	void setSave() { fsave = true; }
	void unsetSave() { fsave = false; }

	// 255�J�E���g
	bool isCount255() { return fcount255; }
	void setCount255() { fcount255 = true; }
	void unsetCount255() { fcount255 = false; }

	// 255����
	bool isTune255() { return ftune255; }
	void setTune255() { ftune255 = true; }
	void unsetTune255() { ftune255 = false; }

protected:
	// �J�����C���X�^���X
	CInstantCamera camera;

	// �C���[�W�o�b�t�@
	BufferedImage image;

protected:
	// ��񂩂���̐ݒ�
	void Init() {
		//serialNo = camera.GetDeviceInfo().GetSerialNumber().c_str();
	}

public:
	// �����f�o�C�X�֋��n���A�X���b�h����Ă΂��̂� public �Ō��J
	bool CanWaitForFrameTriggerReady() {
		return camera.CanWaitForFrameTriggerReady();
	}

	bool WaitForFrameTriggerReady(unsigned int timeoutMs, Pylon::ETimeoutHandling timeoutHandling) {
		return camera.WaitForFrameTriggerReady(timeoutMs, timeoutHandling);
	}

	void ExecuteSoftwareTrigger() {
		camera.ExecuteSoftwareTrigger();
	}

public:
	// �f�o�C�X���A�^�b�`
	virtual void AttachDevice(IPylonDevice* device) = 0;

#if 0
	// �C���[�W�o�b�t�@���A�^�b�`
	void AttachImageBuffer(BufferedImage* buf) {
		image = buf;
	}
#endif

	// grab �X���b�h�N���A�I��
	virtual void StartGrabbing() = 0;
	virtual void StopGrabbing() = 0;

	// grab ����
	virtual void AfterGrabbing(const Pylon::CGrabResultPtr& ptrGrabResult) = 0;

	// �`��
	void DrawImage() {
		image.DrawImage();
	}

};