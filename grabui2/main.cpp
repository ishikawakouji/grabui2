// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

#include <GL/gl3w.h>            // Initialize with gl3wInit()
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// �����̃��[�e�B���e�B
#include "util.h"
#include "utf8sjis.h"
#include "BufferedImage.h"

#include <pylon/PylonIncludes.h>
//#include <pylon/PylonGUI.h>
#include "CameraEmu.h"
#include "CameraMain.h"
#include "CameraSub.h"


// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

// �f�[�^��GPU�֓W�J
//#include <GL/gl3w.h>
#if 0
ImTextureID uint8Gray2gltexture(GLuint& texture, int cols, int rows, uint8_t* data)
{
    //GLuint texture;
    //glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.cols, image.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, cols, rows, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, data);
    //ImTextureID imtexture{ (ImTextureID)(intptr_t)texture };

    return (ImTextureID)(intptr_t)texture;
}
#endif

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "grab UI v2", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Initialize OpenGL loader
    bool err = gl3wInit() != 0;

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);
    ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\meiryo.ttc", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    IM_ASSERT(font != NULL);

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // ������ data �f�B���N�g���ֈړ�
    // �N�_�ƂȂ�f�B���N�g�����L��
    makeAndMoveDir("data");
    char* tmp;
    string currentDirPath;
    if ((tmp = _getcwd(NULL, 0)) != NULL) {
        currentDirPath = tmp;
    }
    string origPath = currentDirPath;

    // data����̑��΃p�X
    string diffSjisPath;
    string diffu8Path;

    // �J�����̏�Ԃ��m�F
    using namespace Pylon;
    static const size_t c_maxCamerasToUse = 3;
    
    PylonInitialize();
    int exitCode = 0;
    // Number of images to be grabbed.
    static const uint32_t c_countOfImagesToGrab = 3;

    // �摜�\���E�B���h�E�̖��O
    string winname[3] = { "a", "b", "c" };

    // �C���[�W�o�b�t�@
    //BufferedImage image[3];

    // �J�����N���X
    CameraBase *cameraArr[3];
    size_t cameraArrNum = 0;
    bool doneMainCamera = false;

    // �B�eON OFF
    bool runGrab = false;

    // �S�̃Q�C���Œ�
    bool fixGain = true;

    try
    {
        // Get the transport layer factory.
        CTlFactory& tlFactory = CTlFactory::GetInstance();

        // Get all attached devices and exit application if no device is found.
        DeviceInfoList_t devices;
        if (tlFactory.EnumerateDevices(devices) == 0)
        {
            throw RUNTIME_EXCEPTION("No camera present.");
        }

        // Create an array of instant cameras for the found devices and avoid exceeding a maximum number of devices.
        CInstantCameraArray cameras(min(devices.size(), c_maxCamerasToUse));

        // Create and attach all Pylon Devices.
        cameraArrNum = cameras.GetSize();
        for (size_t i = 0; i < cameraArrNum; ++i)
        {
            // �J�������G�~�����[�^���ǂ������ׂ�
            //cout << "device mode name " << devices[i].GetModelName() << endl;
            if (devices[i].GetModelName() == "Emulation") {
                cameraArr[i] = new CameraEmu();
            }
            else {
                if (doneMainCamera) {
                    cameraArr[i] = new CameraSub();
                }
                else {
                    cameraArr[i] = new CameraMain();
                    doneMainCamera = true;
                }
            }

            //cameras[i].Attach(tlFactory.CreateDevice(devices[i]));
            cameraArr[i]->AttachDevice(tlFactory.CreateDevice(devices[i]));
            //cameraArr[i].AttachImageBuffer(&image[i]);

            // Print the model name of the camera.
            //cout << "Using device " << cameras[i].GetDeviceInfo().GetModelName() << endl;
            cout << "device S/N " << devices[i].GetSerialNumber() << endl;

            // kick grabbing thread
            //cameraArr[i]->StartGrabbing();
        }

        // grab start
        //cameras.StartGrabbing();
        // This smart pointer will receive the grab result data.
        //CGrabResultPtr ptrGrabResult;

        // Main loop
        while (!glfwWindowShouldClose(window))
        {
            // Poll and handle events (inputs, window resize, etc.)
            // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
            // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
            // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
            // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
            glfwPollEvents();

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // �������玩���̃A�v��

            // �㉺�̃��C�A�E�g


            // �󋵂�\��
            int w, h;
            float infoh;
            glfwGetWindowSize(window, &w, &h);
            ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
            ImGui::SetNextWindowSize(ImVec2(w - 0.0f, 0.0f));
            ImGui::Begin("info", 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);
            {
                // �ۑ���ύX
                if (ImGui::Button(u8"�ۑ���")) {
                    ImGui::OpenPopup("ChangeDir");
                }
                
                if (ImGui::BeginPopupModal("ChangeDir"))
                {
                    ImGui::Text(u8"�Ԗ�");
                    ImGui::SameLine();
                    static char saveCarName[64] = { '\0' };
                    ImGui::InputText(u8"�Ԗ�������", saveCarName, 64);

                    ImGui::Separator();

                    ImGui::Text(u8"�p�l��");
                    static string savePanelName;
                    if (ImGui::Button(u8"���O�t�F")) {
                        savePanelName = "LFF";
                    }
                    ImGui::SameLine();
                    if (ImGui::Button(u8"���O�h�A")) {
                        savePanelName = "LFD";
                    }
                    ImGui::SameLine();
                    if (ImGui::Button(u8"����h�A")) {
                        savePanelName = "LRD";
                    }
                    ImGui::SameLine();
                    if (ImGui::Button(u8"����t�F")) {
                        savePanelName = "LRF";
                    }
                    ImGui::SameLine();
                    if (ImGui::Button(u8"�E�O�t�F")) {
                        savePanelName = "RFF";
                    }
                    ImGui::SameLine();
                    if (ImGui::Button(u8"�E�O�h�A")) {
                        savePanelName = "RFD";
                    }
                    ImGui::SameLine();
                    if (ImGui::Button(u8"�E��h�A")) {
                        savePanelName = "RRD";
                    }
                    ImGui::SameLine();
                    if (ImGui::Button(u8"�E��t�F")) {
                        savePanelName = "RRF";
                    }
                    
                    string nullPath;
                    ImGui::Text((nullPath + saveCarName + "/" + savePanelName).c_str());
                    
                    if (ImGui::Button("OK")) {
                        // ��x�A�N�_�֖߂�
                        moveDir(origPath);
                        // SJIS��
                        diffu8Path = nullPath + saveCarName + "/" + savePanelName;
                        diffSjisPath = utf8_to_wide_to_multi_winapi((nullPath + saveCarName + "/" + savePanelName));
                        makeAndMoveDir(diffSjisPath);
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("cancel")) {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
                
                ImGui::SameLine();

                // �ۑ���(=�J�����g)�̕\��
                ImGui::Text((currentDirPath + "/" + diffu8Path).c_str());

                ImGui::Separator();

                // �J�����̃V���A���ԍ���\��
                for (size_t i = 0; i < cameraArrNum; ++i)
                {
                    ImGui::Text(devices[i].GetSerialNumber().c_str());
                }

                ImGui::Separator();

                // �B�e�J�n�A��~�{�^��
                ImGui::Checkbox(u8"�B�e", &runGrab);

                // �S�̂̃Q�C���Œ�ON/OFF
                ImGui::Separator();
                ImGui::Checkbox(u8"�S�̃Q�C���Œ�", &fixGain);

                // ���̃E�B���h�E�̍���������
                infoh = ImGui::GetWindowHeight();
            }
            ImGui::End();

            // �J�����̋N���A��~
            if (runGrab) {
                for (int i = 0; i < cameraArrNum; ++i) {
                    cameraArr[i]->StartGrabbing();
                }
            }
            else {
                for (int i = 0; i < cameraArrNum; ++i) {
                    cameraArr[i]->StopGrabbing();
                }
            }

            // �G��\��
            // Grab c_countOfImagesToGrab from the cameras.
            // �X���b�h���� soft trigger, on grabbing �C�x���g�ŉ摜�o�b�t�@�������ς�
#if 0
            for (uint32_t i = 0; i < c_countOfImagesToGrab && cameras.IsGrabbing(); ++i)
            {
                cameras.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);

                // Image grabbed successfully?
                if (ptrGrabResult->GrabSucceeded())
                {
                    intptr_t cameraContextValue = ptrGrabResult->GetCameraContext();
                    // DisplayImage(cameraContextValue, ptrGrabResult);

                    // Print the index and the model name of the camera.
                    cout << "Camera " << cameraContextValue << ": " << cameras[cameraContextValue].GetDeviceInfo().GetSerialNumber() << endl;
                
                    // result buffer ���� GL texture �֓W�J
                    int col = ptrGrabResult->GetWidth();
                    int row = ptrGrabResult->GetHeight();
                    imageBuf[cameraContextValue].CopyToGpu(col, row, 1, (unsigned char*)ptrGrabResult->GetBuffer());
                    /*
                    imTxArray[cameraContextValue] = uint8Gray2gltexture(glTxArray[i], col, row, (uint8_t*)ptrGrabResult->GetBuffer());
                    grabbed[cameraContextValue] = true;
                    */

                }
            }
#endif

            //static bool imageFlag = grabbed[0] && grabbed[1] && grabbed[2];
            
            //ImGui::SetNextWindowPos(ImVec2(0.0f, infoh));
            //ImGui::SetNextWindowSize(ImVec2(w - 0.0f, h-infoh));
            //ImGui::Begin("grab image"); // , & imageFlag);
            {
                float childw = (w - 0.0f) / 3.0f;//ImGui::GetContentRegionAvail().x / 3.0f;
                float childx = 0.0f;
                for (int i = 0; i < cameraArrNum; ++i) {
                    //ImGui::SetNextWindowSize(ImVec2(childw, childw));
                    ImGui::SetNextWindowPos(ImVec2(childx, infoh));
                    ImGui::SetNextWindowSize(ImVec2(childw, childw));
                    ImGui::Begin(winname[i].c_str(), NULL, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
                    {
                        cameraArr[i]->DrawImage();
                        //imageBuf[i].DrawImage();
                        //ImGui::Image(imTxArray[i], ImVec2(childw, childw));
                    }
                    ImGui::End();
                    childx += childw;
                    ImGui::SameLine();
                }
            }
            //ImGui::End();

            // �����܂�

            // Rendering
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }


    } // Pylon try-exception
    catch (const GenericException& e)
    {
        // Error handling
        cerr << "An exception occurred." << endl
            << e.GetDescription() << endl;
        exitCode = 1;
    }

    // �J�����C���X�^���X�폜
    for (int i = 0; i < cameraArrNum; ++i) {
        delete cameraArr[i];
    }

    // Releases all pylon resources.
    PylonTerminate();

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();



    return 0;
}
