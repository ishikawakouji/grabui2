#pragma once
#include "imgui.h"
#include <GL/gl3w.h>            // Initialize with gl3wInit()
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// ImGui::Image() のラッピング
// バッファの更新と、描画を分けている
// バッファは GPU上のテクスチャとして保持、与えられたデータからコピーする
class BufferedImage
{
private:
    // テクスチャ
    GLuint gltxtname = 0;
    ImTextureID imtxtname = 0;

    // テクスチャにデータがコピーされているかどうか
    bool enable = false;

    // イメージのサイズ
    int width = 0;
    int height = 0;

    // イメージのバッファ
    unsigned char* localbuf = NULL;

    // イメージの位置と拡大率
    ImVec2 imagePos;
    float imageScale = 1.0f;
    bool fitted = true;


private:
    // データをGPUへ展開
    void uint8Gray2gltexture(int cols, int rows, const unsigned char* data);

    // 大きさを窓にフィットさせる
    void imguiFitCenter(const ImVec2& regionmax, const ImVec2& padding, const float frameh, const int cols, const int rows, float& scale, ImVec2& pos);

    void CopyToGpu(int cols, int rows, int channel, const unsigned char* data);

public:
    BufferedImage() {
        // GLテクスチャを生成
        glGenTextures(1, &gltxtname);
        //imtxtname = (ImTextureID)gltxtname;
    }

    void CacheImage(int cols, int rows, int channel, const unsigned char* data) {
        if (cols != width || rows != height) {
            free(localbuf);

            width = cols;
            height = rows;

            localbuf = (unsigned char*)malloc(width * height);
        }
        memcpy_s(localbuf, width * height, data, cols * rows);
        enable = true;
    }

    // 描画
    void DrawImage();
};
