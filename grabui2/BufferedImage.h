#pragma once
#include "imgui.h"
#include <GL/gl3w.h>            // Initialize with gl3wInit()
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// ImGui::Image() �̃��b�s���O
// �o�b�t�@�̍X�V�ƁA�`��𕪂��Ă���
// �o�b�t�@�� GPU��̃e�N�X�`���Ƃ��ĕێ��A�^����ꂽ�f�[�^����R�s�[����
class BufferedImage
{
private:
    // �e�N�X�`��
    GLuint gltxtname = 0;
    ImTextureID imtxtname = 0;

    // �e�N�X�`���Ƀf�[�^���R�s�[����Ă��邩�ǂ���
    bool enable = false;

    // �C���[�W�̃T�C�Y
    int width = 0;
    int height = 0;

    // �C���[�W�̃o�b�t�@
    unsigned char* localbuf = NULL;

    // �C���[�W�̈ʒu�Ɗg�嗦
    ImVec2 imagePos;
    float imageScale = 1.0f;
    bool fitted = true;


private:
    // �f�[�^��GPU�֓W�J
    void uint8Gray2gltexture(int cols, int rows, const unsigned char* data);

    // �傫���𑋂Ƀt�B�b�g������
    void imguiFitCenter(const ImVec2& regionmax, const ImVec2& padding, const float frameh, const int cols, const int rows, float& scale, ImVec2& pos);

    void CopyToGpu(int cols, int rows, int channel, const unsigned char* data);

public:
    BufferedImage() {
        // GL�e�N�X�`���𐶐�
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

    // �`��
    void DrawImage();
};
