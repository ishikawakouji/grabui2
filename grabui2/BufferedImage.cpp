#include <memory>
#include <thread>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include "BufferedImage.h"

void BufferedImage::uint8Gray2gltexture(int cols, int rows, const unsigned char* data)
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

    //return (ImTextureID)(intptr_t)gltxtname;
    imtxtname = (ImTextureID)(intptr_t)gltxtname;
}

// BGR 3ch を BGRA として表示
void  BufferedImage::cvMatBGR2BGRAgltexture(int cols, int rows, const unsigned char* data)
{
    //GLuint texture;
    //glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, gltxtname);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.cols, image.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cols, rows, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    //ImTextureID imtexture{ (ImTextureID)(intptr_t)texture };

    imtxtname = (ImTextureID)(intptr_t)gltxtname;
}

void BufferedImage::imguiFitCenter(const ImVec2& regionmax, const ImVec2& padding, const float frameh, const int cols, const int rows, float& scale, ImVec2& pos)
{
    // 拡大縮小
    float ws = (regionmax.x - padding.x * 2.f) / (float)cols;
    float hs = (regionmax.y - frameh - padding.y * 4.f) / (float)rows;
    scale = ws > hs ? hs : ws;

    // センタリング
    pos = ImVec2((regionmax.x + padding.x - cols * scale) * 0.5f, (regionmax.y + frameh - rows * scale) * 0.5f);
    ImGui::SetCursorPos(pos);

}

void BufferedImage::CopyToGpu(int cols, int rows, int channel, unsigned char* data) {
    // emulaterなら channel = 1, pysical なら channel = 3
    width = cols;
    height = rows;
    imgSize = cols * rows * channel;
    switch (imgCh) {
    default:
        printf("channel is broken\n");
        break;
    case 1:
        uint8Gray2gltexture(cols, rows, data);
        break;
    case 3:
        // bayer を cvMat へ
        cv::Mat img1(rows, cols, CV_8UC1, data);

        // 色変換
        cv::Mat img2(rows, cols, CV_8UC3);
        cv::cvtColor(img1, img2, CV_BayerBG2BGR);

        // GPUへ
        cvMatBGR2BGRAgltexture(cols, rows, img2.data);

        break;
    }
    

    //enable = true;
}

void BufferedImage::DrawImage() {

    // データが無ければなにもしない
    if (!enable) return;

    // データをGPUへ
    // GPUへの描画と、画面への描画は同一スレッドでなければならないため
    CopyToGpu(width, height, 1, localbuf);

    ImGuiIO io = ImGui::GetIO();

    if (fitted) {
        imguiFitCenter(ImGui::GetContentRegionMax(), ImGui::GetStyle().FramePadding, ImGui::GetFrameHeight(), width, height, imageScale, imagePos);
        fitted = false;
    }

    if (ImGui::IsWindowHovered()) {

        if (ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Middle) || ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Right) || io.MouseWheel != 0.0f)
        {
            fitted = false;
        }
        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
        {
            fitted = true;
        }

        if (fitted) {
            imguiFitCenter(ImGui::GetContentRegionMax(), ImGui::GetStyle().FramePadding, ImGui::GetFrameHeight(), width, height, imageScale, imagePos);
        }
        else {
            // check wheel
            if (io.MouseWheel != 0.0f) {
                float nextscale = (1.0f + io.MouseWheel * 0.1f);

                if (nextscale < 0.05f) nextscale = 0.05f;
                if (imageScale * nextscale > 0.01f) {

                    // 画面の中央
                    float wx = (ImGui::GetContentRegionMax().x + ImGui::GetStyle().FramePadding.x) * 0.5f;
                    float wy = (ImGui::GetContentRegionMax().y + ImGui::GetFrameHeight()) * 0.5f;

                    // 中央と画像始点の差
                    float dx = imagePos.x - wx;
                    float dy = imagePos.y - wy;

                    // スケールで調整
                    dx *= nextscale;
                    dy *= nextscale;

                    // 新しい位置
                    imagePos.x = wx + dx;
                    imagePos.y = wy + dy;

                    imageScale *= nextscale;
                }
            }
            else {
                ImVec2 deltapos;
                if (ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Middle)) {
                    deltapos = ImGui::GetMouseDragDelta(ImGuiMouseButton_::ImGuiMouseButton_Middle);
                }
                else if (ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Right)) {
                    deltapos = ImGui::GetMouseDragDelta(ImGuiMouseButton_::ImGuiMouseButton_Right);
                }

                imagePos.x += deltapos.x;
                imagePos.y += deltapos.y;
            }

            ImGui::ResetMouseDragDelta(ImGuiMouseButton_::ImGuiMouseButton_Middle);
            ImGui::ResetMouseDragDelta(ImGuiMouseButton_::ImGuiMouseButton_Right);

        }
    }

    // テクスチャを描画
    ImGui::SetCursorPos(imagePos);
    ImGui::Image(imtxtname, ImVec2(width * imageScale, height * imageScale));
}

// イメージ保存

void threadSave(BufferedImage* pBuffer)
{

    cv::Mat image(pBuffer->getHeight(), pBuffer->getWidth(), CV_8UC1, pBuffer->getImageDataPtr());
    cv::imwrite(pBuffer->getFileName(), image);

    //Beep(500, 500);

}

void BufferedImage::imgWrite(const char* filename)
{
    memcpy_s(saveFileName, FILE_NAME_LEN, filename, FILE_NAME_LEN);

    std::thread th(threadSave, this);
    th.detach();
}
