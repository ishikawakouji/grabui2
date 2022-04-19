#include <Windows.h>
#include "util.h"

/*
    カレントディレクトリの移動
    移動先が無ければ作る

    区切りは "/" で
*/
int moveToDir(const string& dir) {

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