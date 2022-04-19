#pragma once
#include <string>
#include <pylon/PylonIncludes.h>

#include "BufferedImage.h"

using namespace Pylon;

class CameraBase
{
private:
	// 撮影中かどうか
	bool fgrabbing = false;
	 
	// 保存フラグ
	bool fsave = false;

	// 255カウント
	bool fcount255 = false;

	// 255調整
	bool ftune255 = false;

public:
	// 撮影中フラグ
	bool isGrabbing() { return fgrabbing; }
	void setGrabbing() { fgrabbing = true; }
	void unsetGrabbing() { fgrabbing = false; }

	// 保存フラグ
	bool isSave() { return fsave; }
	void setSave() { fsave = true; }
	void unsetSave() { fsave = false; }

	// 255カウント
	bool isCount255() { return fcount255; }
	void setCount255() { fcount255 = true; }
	void unsetCount255() { fcount255 = false; }

	// 255調整
	bool isTune255() { return ftune255; }
	void setTune255() { ftune255 = true; }
	void unsetTune255() { ftune255 = false; }

protected:
	// カメラインスタンス
	CInstantCamera camera;

	// イメージバッファ
	BufferedImage image;

protected:
	// 一回かぎりの設定
	void Init() {
		//serialNo = camera.GetDeviceInfo().GetSerialNumber().c_str();
	}

public:
	// 内部デバイスへ橋渡し、スレッドから呼ばれるので public で公開
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
	// デバイスをアタッチ
	virtual void AttachDevice(IPylonDevice* device) = 0;

#if 0
	// イメージバッファをアタッチ
	void AttachImageBuffer(BufferedImage* buf) {
		image = buf;
	}
#endif

	// grab スレッド起動、終了
	virtual void StartGrabbing() = 0;
	virtual void StopGrabbing() = 0;

	// grab 処理
	virtual void AfterGrabbing(const Pylon::CGrabResultPtr& ptrGrabResult) = 0;

	// 描画
	void DrawImage() {
		image.DrawImage();
	}

};