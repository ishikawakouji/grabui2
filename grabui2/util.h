#pragma once
#include <iostream>
#include <string>
#include <direct.h>
using namespace std;

/*
    カレントディレクトリの移動
    移動先が無ければ作る

    区切りは "/" で
*/
extern int moveToDir(const string& dir);

extern void GetTimeString(string* output);

extern void AddString(string* inout, char* format, float val);

extern void PrintTime();

extern int count255(size_t size, const uint8_t* pImageBuffer);

extern int count_median255(uint32_t width, uint32_t height, const uint8_t* pImageBuffer);