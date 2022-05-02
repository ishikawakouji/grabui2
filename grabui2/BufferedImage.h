#pragma once
#include "imgui.h"
#include <GL/gl3w.h>            // Initialize with gl3wInit()
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#define FILE_NAME_LEN 128

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
    int imgCh = 1;
    int imgSize = 0;

    // �C���[�W�̃o�b�t�@
    unsigned char* localbuf = NULL;

    // �C���[�W�̈ʒu�Ɗg�嗦
    ImVec2 imagePos;
    float imageScale = 1.0f;
    bool fitted = true;

    // �ۑ����̃t�@�C����
    char saveFileName[FILE_NAME_LEN] = { '\0' };

public:
    int getWidth() { return width; }
    int getHeight() { return height; }
    unsigned char* getImageDataPtr() { return localbuf; }
    char* getFileName() { return saveFileName; }
    void setColorCamera() {
        imgCh = 3;
    }

private:
    // �f�[�^��GPU�֓W�J
    void uint8Gray2gltexture(int cols, int rows, const unsigned char* data);
    void cvMatBGR2BGRAgltexture(int cols, int rows, const unsigned char* data);

    // �傫���𑋂Ƀt�B�b�g������
    void imguiFitCenter(const ImVec2& regionmax, const ImVec2& padding, const float frameh, const int cols, const int rows, float& scale, ImVec2& pos);

    void CopyToGpu(int cols, int rows, int channel, unsigned char* data);

public:
    BufferedImage() {
        // GL�e�N�X�`���𐶐�
        glGenTextures(1, &gltxtname);
        //imtxtname = (ImTextureID)gltxtname;
    }

    ~BufferedImage() {
        free(localbuf);
    }

    void CacheImage(int cols, int rows, int channel, const unsigned char* data) {
        size_t datalen = (size_t)cols * (size_t)rows;

        // �����ɗ���Ƃ��́A�܂������ł悢
        if (cols != width || rows != height) {
            free(localbuf);

            width = cols;
            height = rows;

            localbuf = (unsigned char*)malloc(datalen);
        }
        memcpy_s(localbuf, datalen, data, datalen);
        enable = true;
    }

    // �`��
    void DrawImage();

    // �ۑ�
    void imgWrite(const char* filename);
};
