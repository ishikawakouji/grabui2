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