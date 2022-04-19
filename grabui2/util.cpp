#include <Windows.h>
#include "util.h"

/*
    �J�����g�f�B���N�g���̈ړ�
    �ړ��悪������΍��

    ��؂�� "/" ��
*/
int moveToDir(const string& dir) {

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