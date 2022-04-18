#pragma once
#include "imgui.h"
#include "gl/gl3w.h"

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

    // イメージの位置と拡大率
    ImVec2 imagePos;
    float imageScale = 1.0f;
    bool fitted = true;


private:
	// データをGPUへ展開
    void uint8Gray2gltexture(int cols, int rows, unsigned char* data)
    {
        //GLuint texture;
        //glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, gltxtname);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.cols, image.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, cols, rows, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, data);
        //ImTextureID imtexture{ (ImTextureID)(intptr_t)texture };

        //return (ImTextureID)(intptr_t)texture;
        imtxtname = (ImTextureID)(intptr_t)gltxtname;
    }

    // 大きさを窓にフィットさせる
    void imguiFitCenter(const ImVec2& regionmax, const ImVec2& padding, const float frameh, const int cols, const int rows, float& scale, ImVec2& pos);


public:
	BufferedImage() {
		glGenTextures(1, &gltxtname);
	    //imtxtname = (ImTextureID)gltxtname;
	}

    void CopyToGpu(int cols, int rows, int channel,  unsigned char* data) {
        // とりあえず
        width = cols;
        height = rows;
        uint8Gray2gltexture(cols, rows, data);
        enable = true;
    }
        
    // 描画
    void DrawImage();
};