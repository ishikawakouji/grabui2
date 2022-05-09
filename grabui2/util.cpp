#include <Windows.h>
#include <algorithm>
#include <opencv2/opencv.hpp>

#include "util.h"

/*
    カレントディレクトリの移動
    移動先が無ければ作る

    区切りは "/" で
*/
int makeAndMoveDir(const string& dir) {

    string strPath = dir;

    // 前後の余分な区切りをとりあえずカット
    strPath.erase(strPath.find_last_not_of('/') + 1);
    strPath.erase(0, strPath.find_first_not_of(". /"));

    // 前から順に作る
    string objPath;
    size_t cpos = 0;

    while (objPath != strPath) {
        cpos = strPath.find_first_of('/', ++cpos);
        objPath = strPath.substr(0, cpos);

        int ret = _mkdir(objPath.c_str());
    }

    // 最後に移動
    int ret = _chdir(strPath.c_str());

    return ret;
}

int moveDir(const string& dir) {
	int ret = _chdir(dir.c_str());
	return ret;
}

void GetTimeString(string* output) {
    char buf[64];
    SYSTEMTIME lt;
    GetLocalTime(&lt);
    sprintf_s(buf, 64, "%04d%02d%02d_%02d%02d%02d_%03d", lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds);
    *output = buf;
}

void AddString(string* inout, char* format, float val) {
    char buf[128];
    sprintf_s(buf, 128, format, val);

    *inout += buf;
}

void PrintTime() {
    string buf;
    GetTimeString(&buf);
    cout << buf << endl;
}

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

// 中央値で、既定の大きさ以上の塊の中の最大値が225-254に収まるように制御
int mask_median255_gain_tune(uint32_t width, uint32_t height, const uint8_t* pImageBuffer, CameraMain* pCamera)
{
	// 画面全体の255オーバー画素数
	int pix = 0;

	// 値を集める
	uint8_t arr[9];
	int idx = 0;

	// 中央値イメージ
	cv::Mat medianImg = cv::Mat::zeros(height, width, CV_8UC1);

	// GR のブロックを一度に見るので 2つずつ移動
	// BG
	for (uint32_t h = 2; h < height - 2; h += 2) {
		for (uint32_t w = 2; w < width - 2; w += 2) {

			// G1
			idx = 0;
			int G = width * h + w;
			arr[idx++] = pImageBuffer[G - 2 * width];
			arr[idx++] = pImageBuffer[G - width + 1];
			arr[idx++] = pImageBuffer[G - width - 1];
			arr[idx++] = pImageBuffer[G - 2];
			arr[idx++] = pImageBuffer[G];
			arr[idx++] = pImageBuffer[G + 2];
			arr[idx++] = pImageBuffer[G + width - 1];
			arr[idx++] = pImageBuffer[G + width + 1];
			arr[idx++] = pImageBuffer[G + 2 * width];
			std::sort(arr, arr + idx);
			if (arr[4] == 255) ++pix;
			medianImg.data[G] = arr[4];

			// R
			idx = 0;
			int R = G + 1;
			arr[idx++] = pImageBuffer[R - 2 * width];
			arr[idx++] = pImageBuffer[R - 2 * width - 2];
			arr[idx++] = pImageBuffer[R - 2 * width + 2];
			arr[idx++] = pImageBuffer[R - 2] == 255;
			arr[idx++] = pImageBuffer[R];
			arr[idx++] = pImageBuffer[R + 2];
			arr[idx++] = pImageBuffer[R + 2 * width - 2];
			arr[idx++] = pImageBuffer[R + 2 * width + 2];
			arr[idx++] = pImageBuffer[R + 2 * width];

			std::sort(arr, arr + idx);
			if (arr[4] == 255) ++pix;
			medianImg.data[R] = arr[4];

			// B
			idx = 0;
			int B = G + width;
			arr[idx++] = pImageBuffer[B - 2 * width];
			arr[idx++] = pImageBuffer[B - 2 * width - 2];
			arr[idx++] = pImageBuffer[B - 2 * width + 2];
			arr[idx++] = pImageBuffer[B - 2];
			arr[idx++] = pImageBuffer[B];
			arr[idx++] = pImageBuffer[B + 2];
			arr[idx++] = pImageBuffer[B + 2 * width - 2];
			arr[idx++] = pImageBuffer[B + 2 * width + 2];
			arr[idx++] = pImageBuffer[B + 2 * width];

			std::sort(arr, arr + idx);
			if (arr[4] == 255) ++pix;
			medianImg.data[B] = arr[4];

			// G2
			idx = 0;
			G = B + 1;
			arr[idx++] = pImageBuffer[G - 2 * width];
			arr[idx++] = pImageBuffer[G - width + 1];
			arr[idx++] = pImageBuffer[G - width - 1];
			arr[idx++] = pImageBuffer[G - 2];
			arr[idx++] = pImageBuffer[G];
			arr[idx++] = pImageBuffer[G + 2];
			arr[idx++] = pImageBuffer[G + width - 1];
			arr[idx++] = pImageBuffer[G + width + 1];
			arr[idx++] = pImageBuffer[G + 2 * width];

			std::sort(arr, arr + idx);
			if (arr[4] == 255) ++pix;
			medianImg.data[G] = arr[4];

		} // width + 2
	} // height + 2

	//cv::imwrite(string("median_image.png"), medianImg);

	// 二値化
	cv::Mat binImg = cv::Mat::zeros(medianImg.size(), CV_8UC1);
	cv::threshold(medianImg, binImg, 200.0, 255.0, cv::THRESH_BINARY);

	// 塊抽出
	vector< vector<cv::Point> > contours;
	vector<cv::Vec4i> hierarchy;
	cv::findContours(binImg, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	
	// 既定値以上のエリアをマスクに登録
	cv::Mat maskImg = cv::Mat::zeros(medianImg.size(), CV_8UC1);
	double minarea = (double)pCamera->getMinArea();
	double area = 0.0;
	{
		int i = 0;
		for (vector< vector<cv::Point> >::iterator x = contours.begin(); x != contours.end(); ++x) {
			double a = cv::contourArea(*x);
			if (a >= minarea) {
				cv::drawContours(maskImg, contours, i, cv::Scalar(255), cv::FILLED);
			}
			++i;
		}
	}

#ifdef DEBUG_VIEW
	pCamera->debugImage.CacheImage(maskImg.cols, maskImg.rows, 1, maskImg.data);
#endif

	// マスク処理
	cv::Mat resImg = cv::Mat::zeros(medianImg.size(), CV_8UC1);
	medianImg.copyTo(resImg, maskImg);

	// 255要素取り出し
	cv::Mat elem255 = (resImg == 255) * 255;
	int num255 = cv::countNonZero(elem255);

	// 自動ゲイン調整するかどうか
	if (!pCamera->doAutoGainTune()) {
		return num255;
	}

	// 255以上があったらゲインを下げる
	if (num255 > 0) {
		//int minarea = pCamera->getMinArea();
		double howmuch = num255 > 40000 ? 2.0 : 1.0;
		howmuch = num255 < 10000 ? 0.5 : howmuch;

		double gain = pCamera->GetDoubleGain();
		double nextgain = gain - 0.5 * howmuch;
		nextgain = pCamera->SetDoubleGain(nextgain);
		
		// ゲインは下限
		if (nextgain == gain) {
			// 露出時間で調整
			double extime = pCamera->GetDoubleExposureTime();
			double nextextime = extime * 0.9;
			nextextime = pCamera->SetDoubleExposureTime(nextextime);

			// 露出時間も下限なら仕方がない
		}
		return num255;
	}

	// 既定の中に入っているか
	int pixminval = pCamera->getPixMinVal();
	cv::Mat over225 = (resImg >= pixminval) * 255;
	int num225 = cv::countNonZero(over225);

	if (num225 > 0) {
		// 225以上が存在するのでおわり
		return num255;
	}

	// ゲインを下げすぎ
	double extime = pCamera->GetDoubleExposureTime();
	double nextextime = extime * 1.1;
	nextextime = pCamera->SetDoubleExposureTime(nextextime);

	// 露出時間が上限ならゲインを変更
	if (extime == nextextime) {
		cv::Mat over100 = (resImg >= 100) * 255;
		int num100 = cv::countNonZero(over100);

		double howmuch = num100 > 10000 ? 1.0 : 0.5;
		double gain = pCamera->GetDoubleGain();
		double nextgain = gain + 0.5 * howmuch;
		nextgain = pCamera->SetDoubleGain(nextgain);

		// ゲインも上限なら仕方ない
		//cout << "NEXT gain " << nextgain << endl;
	}


	return num255;
}

// 見たいところについて、周囲を見て中央値が255になるものをカウント
int mask_count_median255(uint32_t width, uint32_t height, const uint8_t* pImageBuffer) {
	int pix = 0;

	// 値を集める
	uint8_t arr[9];
	int idx = 0;

	// 中央値イメージ
	cv::Mat medianImg = cv::Mat::zeros(height, width, CV_8UC1);

	// GR のブロックを一度に見るので 2つずつ移動
	// BG
	for (uint32_t h = 2; h < height - 2; h += 2) {
		for (uint32_t w = 2; w < width - 2; w += 2) {

			// G1
			idx = 0;
			int G = width * h + w;
			arr[idx++] = pImageBuffer[G - 2 * width];
			arr[idx++] = pImageBuffer[G - width + 1];
			arr[idx++] = pImageBuffer[G - width - 1];
			arr[idx++] = pImageBuffer[G - 2];
			arr[idx++] = pImageBuffer[G];
			arr[idx++] = pImageBuffer[G + 2];
			arr[idx++] = pImageBuffer[G + width - 1];
			arr[idx++] = pImageBuffer[G + width + 1];
			arr[idx++] = pImageBuffer[G + 2 * width];

			std::sort(arr, arr + idx);
			if (arr[4] == 255) ++pix;
			medianImg.data[G] = arr[4];

			// R
			idx = 0;
			int R = G + 1;
			arr[idx++] = pImageBuffer[R - 2 * width];
			arr[idx++] = pImageBuffer[R - 2 * width - 2];
			arr[idx++] = pImageBuffer[R - 2 * width + 2];
			arr[idx++] = pImageBuffer[R - 2] == 255;
			arr[idx++] = pImageBuffer[R];
			arr[idx++] = pImageBuffer[R + 2];
			arr[idx++] = pImageBuffer[R + 2 * width - 2];
			arr[idx++] = pImageBuffer[R + 2 * width + 2];
			arr[idx++] = pImageBuffer[R + 2 * width];

			std::sort(arr, arr + idx);
			if (arr[4] == 255) ++pix;
			medianImg.data[R] = arr[4];

			// B
			idx = 0;
			int B = G + width;
			arr[idx++] = pImageBuffer[B - 2 * width];
			arr[idx++] = pImageBuffer[B - 2 * width - 2];
			arr[idx++] = pImageBuffer[B - 2 * width + 2];
			arr[idx++] = pImageBuffer[B - 2];
			arr[idx++] = pImageBuffer[B];
			arr[idx++] = pImageBuffer[B + 2];
			arr[idx++] = pImageBuffer[B + 2 * width - 2];
			arr[idx++] = pImageBuffer[B + 2 * width + 2];
			arr[idx++] = pImageBuffer[B + 2 * width];

			std::sort(arr, arr + idx);
			if (arr[4] == 255) ++pix;
			medianImg.data[B] = arr[4];

			// G2
			idx = 0;
			G = B + 1;
			arr[idx++] = pImageBuffer[G - 2 * width];
			arr[idx++] = pImageBuffer[G - width + 1];
			arr[idx++] = pImageBuffer[G - width - 1];
			arr[idx++] = pImageBuffer[G - 2];
			arr[idx++] = pImageBuffer[G];
			arr[idx++] = pImageBuffer[G + 2];
			arr[idx++] = pImageBuffer[G + width - 1];
			arr[idx++] = pImageBuffer[G + width + 1];
			arr[idx++] = pImageBuffer[G + 2 * width];

			std::sort(arr, arr + idx);
			if (arr[4] == 255) ++pix;
			medianImg.data[G] = arr[4];

		} // width + 2
	} // height + 2

	//cv::imwrite(string("median_image.png"), medianImg);

	// 二値化
	cv::Mat binImg = cv::Mat::zeros(medianImg.size(), CV_8UC1);
	cv::threshold(medianImg, binImg, 200.0, 255.0, cv::THRESH_BINARY);

	// 塊抽出
	vector< vector<cv::Point> > contours;
	vector<cv::Vec4i> hierarchy;
	cv::findContours(binImg, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

	// 最大の輪郭
	double area = 0.0;
	int area1st = 0;
	{
		int i = 0;
		for (vector< vector<cv::Point> >::iterator x = contours.begin(); x != contours.end(); ++x) {
			double a = cv::contourArea(*x);
			if (area < a) {
				area = a;
				area1st = i;
			}
			++i;
		}
	}

	// 最大の輪郭でマスクを作る
	cv::Mat maskImg = cv::Mat::zeros(medianImg.size(), CV_8UC1);
	cv::drawContours(maskImg, contours, area1st, cv::Scalar(255), cv::FILLED);

	// マスク処理
	cv::Mat resImg = cv::Mat::zeros(medianImg.size(), CV_8UC1);
	medianImg.copyTo(resImg, maskImg);

	// 255要素取り出し
	cv::Mat elem255 = (resImg == 255) * 255;
	int num255 = cv::countNonZero(elem255);

	//return pix;
	return num255;
}