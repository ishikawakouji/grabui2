#include <Windows.h>
#include <algorithm>
#include <opencv2/opencv.hpp>

#include "util.h"

/*
    �J�����g�f�B���N�g���̈ړ�
    �ړ��悪������΍��

    ��؂�� "/" ��
*/
int makeAndMoveDir(const string& dir) {

    string strPath = dir;

    // �O��̗]���ȋ�؂���Ƃ肠�����J�b�g
    strPath.erase(strPath.find_last_not_of('/') + 1);
    strPath.erase(0, strPath.find_first_not_of(". /"));

    // �O���珇�ɍ��
    string objPath;
    size_t cpos = 0;

    while (objPath != strPath) {
        cpos = strPath.find_first_of('/', ++cpos);
        objPath = strPath.substr(0, cpos);

        int ret = _mkdir(objPath.c_str());
    }

    // �Ō�Ɉړ�
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
* �I�o�I�[�o�[�̃J�E���g
*/
// �P���ȃJ�E���g
int count255(size_t size, const uint8_t* pImageBuffer) {
	int pix = 0;
	for (size_t i = 0; i < size; ++i) {
		if (pImageBuffer[i] == 255) pix++;
	}
	return pix;
}

// ���͂����Ē����l��255�ɂȂ���̂��J�E���g
int count_median255(uint32_t width, uint32_t height, const uint8_t* pImageBuffer) {
	int pix = 0;

	// GR �̃u���b�N����x�Ɍ���̂� 2���ړ�
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

// �����l�ŁA����̑傫���ȏ�̉�̒��̍ő�l��225-254�Ɏ��܂�悤�ɐ���
int mask_median255_gain_tune(uint32_t width, uint32_t height, const uint8_t* pImageBuffer, CameraMain* pCamera)
{
	// ��ʑS�̂�255�I�[�o�[��f��
	int pix = 0;

	// �l���W�߂�
	uint8_t arr[9];
	int idx = 0;

	// �����l�C���[�W
	cv::Mat medianImg = cv::Mat::zeros(height, width, CV_8UC1);

	// GR �̃u���b�N����x�Ɍ���̂� 2���ړ�
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

	// ��l��
	cv::Mat binImg = cv::Mat::zeros(medianImg.size(), CV_8UC1);
	cv::threshold(medianImg, binImg, 200.0, 255.0, cv::THRESH_BINARY);

	// �򒊏o
	vector< vector<cv::Point> > contours;
	vector<cv::Vec4i> hierarchy;
	cv::findContours(binImg, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	
	// ����l�ȏ�̃G���A���}�X�N�ɓo�^
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

	// �}�X�N����
	cv::Mat resImg = cv::Mat::zeros(medianImg.size(), CV_8UC1);
	medianImg.copyTo(resImg, maskImg);

	// 255�v�f���o��
	cv::Mat elem255 = (resImg == 255) * 255;
	int num255 = cv::countNonZero(elem255);

	// �����Q�C���������邩�ǂ���
	if (!pCamera->doAutoGainTune()) {
		return num255;
	}

	// 255�ȏオ��������Q�C����������
	if (num255 > 0) {
		//int minarea = pCamera->getMinArea();
		double howmuch = num255 > 40000 ? 2.0 : 1.0;
		howmuch = num255 < 10000 ? 0.5 : howmuch;

		double gain = pCamera->GetDoubleGain();
		double nextgain = gain - 0.5 * howmuch;
		nextgain = pCamera->SetDoubleGain(nextgain);
		
		// �Q�C���͉���
		if (nextgain == gain) {
			// �I�o���ԂŒ���
			double extime = pCamera->GetDoubleExposureTime();
			double nextextime = extime * 0.9;
			nextextime = pCamera->SetDoubleExposureTime(nextextime);

			// �I�o���Ԃ������Ȃ�d�����Ȃ�
		}
		return num255;
	}

	// ����̒��ɓ����Ă��邩
	int pixminval = pCamera->getPixMinVal();
	cv::Mat over225 = (resImg >= pixminval) * 255;
	int num225 = cv::countNonZero(over225);

	if (num225 > 0) {
		// 225�ȏオ���݂���̂ł����
		return num255;
	}

	// �Q�C������������
	double extime = pCamera->GetDoubleExposureTime();
	double nextextime = extime * 1.1;
	nextextime = pCamera->SetDoubleExposureTime(nextextime);

	// �I�o���Ԃ�����Ȃ�Q�C����ύX
	if (extime == nextextime) {
		cv::Mat over100 = (resImg >= 100) * 255;
		int num100 = cv::countNonZero(over100);

		double howmuch = num100 > 10000 ? 1.0 : 0.5;
		double gain = pCamera->GetDoubleGain();
		double nextgain = gain + 0.5 * howmuch;
		nextgain = pCamera->SetDoubleGain(nextgain);

		// �Q�C��������Ȃ�d���Ȃ�
		//cout << "NEXT gain " << nextgain << endl;
	}


	return num255;
}

// �������Ƃ���ɂ��āA���͂����Ē����l��255�ɂȂ���̂��J�E���g
int mask_count_median255(uint32_t width, uint32_t height, const uint8_t* pImageBuffer) {
	int pix = 0;

	// �l���W�߂�
	uint8_t arr[9];
	int idx = 0;

	// �����l�C���[�W
	cv::Mat medianImg = cv::Mat::zeros(height, width, CV_8UC1);

	// GR �̃u���b�N����x�Ɍ���̂� 2���ړ�
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

	// ��l��
	cv::Mat binImg = cv::Mat::zeros(medianImg.size(), CV_8UC1);
	cv::threshold(medianImg, binImg, 200.0, 255.0, cv::THRESH_BINARY);

	// �򒊏o
	vector< vector<cv::Point> > contours;
	vector<cv::Vec4i> hierarchy;
	cv::findContours(binImg, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

	// �ő�̗֊s
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

	// �ő�̗֊s�Ń}�X�N�����
	cv::Mat maskImg = cv::Mat::zeros(medianImg.size(), CV_8UC1);
	cv::drawContours(maskImg, contours, area1st, cv::Scalar(255), cv::FILLED);

	// �}�X�N����
	cv::Mat resImg = cv::Mat::zeros(medianImg.size(), CV_8UC1);
	medianImg.copyTo(resImg, maskImg);

	// 255�v�f���o��
	cv::Mat elem255 = (resImg == 255) * 255;
	int num255 = cv::countNonZero(elem255);

	//return pix;
	return num255;
}