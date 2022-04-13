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