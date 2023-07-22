#include"UI.h"
void SetupUI(bool* p_open)
{
    static bool openNew = false;
    static bool openDefault = false;
    static bool save = false;
    static bool reset = false;
    static bool close = false;
    static bool phong = false;
    static bool pbr = false;
    ImGuiWindowFlags window_flags = 0;
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(main_viewport->Size.x/10 , main_viewport->Size.y), ImGuiCond_FirstUseEver);  

    if (!ImGui::Begin("A lightweight 3d renderer", p_open, window_flags))
    {
        ImGui::End();
        return;
    }

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::BeginMenu("Open"))
            {
                ImGui::MenuItem("Default", NULL, &openDefault);
                ImGui::MenuItem("New", NULL, &openNew);
            }
            ImGui::MenuItem("Save", NULL, &save);
            ImGui::MenuItem("reset", NULL, &reset);
        }
    }
    if (ImGui::CollapsingHeader("Shading models"))
    {
        if (ImGui::TreeNode("Active model"))
        {
            static int selected = -1;

        }
    }
}

string GetPathFromFileDialog()
{
    nfdchar_t* path = nullptr;
    nfdresult_t result = NFD_OpenDialog(NULL, NULL, &path);

    if (result == NFD_OKAY)
    {
        string ret = path;
    }
    else if(result == NFD_CANCEL)
    {

    }
    else
    {
        Log(string(NFD_GetError()));
        return "E";
    }
}