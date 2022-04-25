#include <memory>
#include <thread>
#include <stdio.h>
#include <opencv2/opencv.hpp>
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

void BufferedImage::imguiFitCenter(const ImVec2& regionmax, const ImVec2& padding, const float frameh, const int cols, const int rows, float& scale, ImVec2& pos)
{
    // �g��k��
    float ws = (regionmax.x - padding.x * 2.f) / (float)cols;
    float hs = (regionmax.y - frameh - padding.y * 4.f) / (float)rows;
    scale = ws > hs ? hs : ws;

    // �Z���^�����O
    pos = ImVec2((regionmax.x + padding.x - cols * scale) * 0.5f, (regionmax.y + frameh - rows * scale) * 0.5f);
    ImGui::SetCursorPos(pos);

}

void BufferedImage::CopyToGpu(int cols, int rows, int channel, const unsigned char* data) {
    // �Ƃ肠����
    width = cols;
    height = rows;
    uint8Gray2gltexture(cols, rows, data);

    //enable = true;
}

void BufferedImage::DrawImage() {

    // �f�[�^��������΂Ȃɂ����Ȃ�
    if (!enable) return;

    // �f�[�^��GPU��
    // GPU�ւ̕`��ƁA��ʂւ̕`��͓���X���b�h�łȂ���΂Ȃ�Ȃ�����
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

                    // ��ʂ̒���
                    float wx = (ImGui::GetContentRegionMax().x + ImGui::GetStyle().FramePadding.x) * 0.5f;
                    float wy = (ImGui::GetContentRegionMax().y + ImGui::GetFrameHeight()) * 0.5f;

                    // �����Ɖ摜�n�_�̍�
                    float dx = imagePos.x - wx;
                    float dy = imagePos.y - wy;

                    // �X�P�[���Œ���
                    dx *= nextscale;
                    dy *= nextscale;

                    // �V�����ʒu
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

    // �e�N�X�`����`��
    ImGui::SetCursorPos(imagePos);
    ImGui::Image(imtxtname, ImVec2(width * imageScale, height * imageScale));
}

// �C���[�W�ۑ�

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
