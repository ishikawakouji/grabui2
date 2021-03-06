#pragma once
/*
* utf8, wide, Shift-JIS を相互に変換する方法の一部を実装
* ref https://nekko1119.hatenablog.com/entry/2017/01/02/054629
*/

#include <string>

/*
* utf8 -> wide -> SJIS
*/
extern std::string utf8_to_wide_to_multi_winapi(std::string const& src);
