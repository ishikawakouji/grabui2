#pragma once
#include <iostream>
#include <string>
#include <direct.h>
using namespace std;

/*
    �J�����g�f�B���N�g���̈ړ�
    �ړ��悪������΍��

    ��؂�� "/" ��
*/
extern int moveToDir(const string& dir);

extern void GetTimeString(string* output);

extern void AddString(string* inout, char* format, float val);

extern void PrintTime();