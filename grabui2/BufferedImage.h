#pragma once
#include "imgui.h"
#include "gl/gl3w.h"

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

    // �C���[�W�̈ʒu�Ɗg�嗦
    ImVec2 imagePos;
    float imageScale = 1.0f;
    bool fitted = true;


private:
	// �f�[�^��GPU�֓W�J
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

    // �傫���𑋂Ƀt�B�b�g������
    void imguiFitCenter(const ImVec2& regionmax, const ImVec2& padding, const float frameh, const int cols, const int rows, float& scale, ImVec2& pos);


public:
	BufferedImage() {
		glGenTextures(1, &gltxtname);
	    //imtxtname = (ImTextureID)gltxtname;
	}

    void CopyToGpu(int cols, int rows, int channel,  unsigned char* data) {
        // �Ƃ肠����
        width = cols;
        height = rows;
        uint8Gray2gltexture(cols, rows, data);
        enable = true;
    }
        
    // �`��
    void DrawImage();
};