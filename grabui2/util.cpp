#include <Windows.h>
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
