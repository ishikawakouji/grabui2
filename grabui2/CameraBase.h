#pragma once
#include <string>
#include "imgui.h"
#include <pylon/PylonIncludes.h>

#include "BufferedImage.h"

using namespace std;
using namespace Pylon;

class CameraBase
{
public:
	// camera main との整合を取るため
	bool isMain() { return true; }
private:
	// ゲイン調整に必要なパラメータ
	int minWaitTime = 50; // 最低撮影間隔 ms
	int minArea = 20000; // 確認するエリアの面積の最小値 pixcels
	int pixMinVal = 225; // 見る範囲を決めるときのピクセル値の最小値
	double minExTime = 1000.0; // 露出時間の最小値 usec
	double maxExTime = 4000.0; // 露出時間の最大値 usec

	// 計算中かどうか
	bool runningAfterGrabbing = false;

public:
	// パラメータの入出力
	void setWaitTime(int val) {
		minWaitTime = val;
	}
	int getWaitTime() {
		return minWaitTime;
	}
	void setMinArea(int val) {
		minArea = val;
	}
	void setPixMinVal(int val) {
		pixMinVal = val;
	}
	void setExTimeRange(double vmin, double vmax) {
		minExTime = vmin;
		maxExTime = vmax;
	}

	void setAfterGrabbing() {
		runningAfterGrabbing = true;
	}
	void unsetAfterGrabbing() {
		runningAfterGrabbing = false;
	}
	bool isAfterGrabbing() {
		return runningAfterGrabbing;
	}
private:
	// 撮影中かどうか
	bool fgrabbing = false;
	 
	// 保存フラグ
	bool fsave = false;

	// 255カウント
	bool fcount255 = false;
	int count255 = 0;
	bool fphysical = false;

	// 255調整
	bool ftune255 = false;

	// 保存ファイル名のポインタ
	char saveFileName[FILE_NAME_LEN];

public:
	// 255カウント
	void setCount255(int val) { count255 = val; }
	int getCount255() {
		return count255;
	}
	// 保存する
	void setSaveFileName(const char* filename) {
		memcpy_s(saveFileName, FILE_NAME_LEN, filename, FILE_NAME_LEN);
	}
	char* getSaveFileName() {
		return saveFileName;
	}

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
	// 露出時間のノード
	Pylon::CFloatParameter doubleExposureTime;
	bool flagExposureTimeValid;
	double exposureTimeCache = 0.0;

	// ゲインのノードタイプ
	enum class GAIN_TYPE {
		GAIN_DOUBLE,
		GAIN_INT64
	};
	enum class GAIN_TYPE typeGain;

	// ゲインのノード、double
	Pylon::CFloatParameter doubleGain;
	double doubleGainMax;
	double doubleGainMin;
	double doubleGainCache = 0.0;

	// ゲインのノード、long long (int64_t)
	Pylon::CIntegerParameter intGain;
	int64_t intGainMax;
	int64_t intGainMin;
	int64_t intGainCache = 0;

protected:
	// 一回かぎりの設定
	void Init() {
		// node設定は Open()の後
		//setNode();
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

	// リアルカメラ
	void setPhysicalCamera() {
		image.setColorCamera();
		fphysical = true;
	}
	bool isPhysicalCamera() {
		return fphysical;
	}

#if 0
	// イメージバッファをアタッチ
	void AttachImageBuffer(BufferedImage* buf) {
		image = buf;
	}
#endif

	~CameraBase() {
		StopGrabbing();
	}

	// grab スレッド起動、終了
	virtual void StartGrabbing() = 0;
	virtual void StopGrabbing() {
		if (!isGrabbing()) {
			return;
		}
		unsetGrabbing();
		camera.Close();
	}

	void setNode();

	// grab 処理
	virtual void AfterGrabbing(const Pylon::CGrabResultPtr& ptrGrabResult) = 0;

	// 描画
	void DrawImage() {
		image.DrawImage();
	}

	// ゲインのやりとり
	int64_t GetIntGain() { return intGainCache; }
	double GetDoubleGain() { return doubleGainCache; }

	int64_t SetIntGain(int64_t val) {
		if (val < intGainMin) val = intGainMin;
		if (val > intGainMax) val = intGainMax;

		intGain.SetValue(val);
		intGainCache = val;
		return val;
	}
	double SetDoubleGain(double val) {
		if (val < doubleGainMin) val = doubleGainMin;
		if (val > doubleGainMax) val = doubleGainMax;

		doubleGain.SetValue(val);
		doubleGainCache = val;
		return val;
	}

	int64_t GetIntGainMax() { return intGainMax; }
	int64_t GetIntGainMin() { return intGainMin; }

	double GetDoubleGainMax() { return doubleGainMax; }
	double GetDoubleGainMin() { return doubleGainMin; }

	// 露出時間
	double GetDoubleExposureTime() {
		if (flagExposureTimeValid) {
			return exposureTimeCache;
		}
		return 1000.0;
	}
	double SetDoubleExposureTime(double val) {
		if (val < minExTime) val = minExTime;
		if (val > maxExTime) val = maxExTime;

		if (flagExposureTimeValid) {
			doubleExposureTime.SetValue(val);
			exposureTimeCache = val;
		}
		return val;
	}

	// 表示用
	string getGainMinMaxString();
	string getGainString();
	string getExposureString();
	string getCount255String();

	// パラメータ設定ポップアップ
	void popupConfig(const char* winname);

	// 保存開始、image におまかせ
	void startSaveImage()
	{
		image.imgWrite(saveFileName);
	}

#ifdef DEBUG_VIEW
public:
	virtual void DrawDebugImage() = 0;
#endif
};