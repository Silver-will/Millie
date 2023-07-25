#include"UI.h"
#include"Lights.h"
#include<vector>
#include<nfd/include/nfd.h>
using namespace std::literals::string_literals;
using namespace Light_values;

//directly read display and edit lighting values
void setLights(vec3& lightValues, string name);

void SetupUI(bool* p_open)
{
    static bool openNew = false;
    static bool openDefault = false;
    static bool reset = false;
    static bool close = false;
    static bool phong = false;
    static bool pbr = false;
    ImGuiWindowFlags window_flags = 0;
    
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(700, main_viewport->WorkSize.y - 20), ImGuiCond_FirstUseEver);
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
                ImGui::EndMenu();
            }
            ImGui::MenuItem("reset", NULL, &reset);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    if (ImGui::CollapsingHeader("Shading models"))
    {
        static std::vector<string> models{ "none", "blinn-phong", "pbr" };
        if (ImGui::TreeNode("Active model"))
        {
            static int selected = -1;
            for (int i = 0; i < models.size(); i++)
            {
                if (ImGui::Selectable(models.at(i).c_str(), selected == i))
                {
                    selected = i;
                    Glob::active = active_shading_model{ i };
                }
            }
            ImGui::TreePop();
        }
    }
    if (ImGui::CollapsingHeader("Blinn-phong"))
    {
        string index{};
        if (ImGui::TreeNode("Point Lights"))
        {
            for (size_t i = 0; i < points.size(); i++)
            {
                index = std::to_string(i);
                ImGui::SeparatorText(("Point "s + index).c_str());

                if (ImGui::TreeNode("Positions"))
                {
                    static GLfloat pos[3] = { points[i].position.x, points[i].position.y, points[i].position.z };
                    ImGui::InputFloat3("x,y,z", pos);
                    points[i].position = vec3(pos[0], pos[1], pos[2]);
                    ImGui::TreePop();
                    ImGui::Spacing();
                }
                if (ImGui::TreeNode("ambient"))
                {
                    //couldn't set a maximum and minimum value for InputFloat3() so this will have to do
                    static auto& amb = points[i].ambient;
                    setLights(amb, "amb");
                    ImGui::TreePop();
                    ImGui::Spacing();
                }
                if (ImGui::TreeNode("diffuse"))
                {
                    static auto& diff = points[i].diffuse;
                    setLights(diff, "diff");
                    ImGui::TreePop();
                    ImGui::Spacing();
                }
                if (ImGui::TreeNode("specular"))
                {
                    static auto& spec = points[i].specular;
                    setLights(spec, "spec");
                    ImGui::TreePop();
                    ImGui::Spacing();
                }
                if (ImGui::TreeNode("attenuation"))
                {
                    //move this declaration to a higher scope later
                    ImGui::SliderFloat("Constant", &points[i].constant, 0.0f, 1.0f, "&.05f");
                    ImGui::SliderFloat("Linear", &points[i].linear, 0.0f, 1.0f, "&.05f");
                    ImGui::SliderFloat("quadratic", &points[i].quadratic, 0.0f, 1.0f, "&.05f");
                    ImGui::TreePop();
                    ImGui::Spacing();
                    
                }

            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Spot Lights"))
        {
            for (size_t i = 0; i < spots.size(); i++)
            {
                index = std::to_string(i);
                ImGui::SeparatorText(("Spots "s + index).c_str());

                if (ImGui::TreeNode("Positions and Direction"))
                {
                    static float pos[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
                    ImGui::InputFloat3("pos", pos);
                    ImGui::InputFloat3("dir", &pos[3]);
                    spots[i].position = vec3(pos[0], pos[1], pos[2]);
                    spots[i].direction = vec3(pos[3], pos[4], pos[5]);
                    ImGui::TreePop();
                    ImGui::Spacing();
                }
                if (ImGui::TreeNode("ambient"))
                {
                    //couldn't set a maximum and minimum value for InputFloat3() so this will have to do
                    static std::vector<GLfloat> amb{ points[i].ambient.x, points[i].ambient.y, points[i].ambient.z };
                    ImGui::InputFloat3("R,G,B amb", amb.data());
                    points[i].ambient = vec3(amb.at(0), amb.at(1), amb.at(2));
                    ImGui::TreePop();
                    ImGui::Spacing();
                }
                if (ImGui::TreeNode("diffuse"))
                {
                    static std::vector<GLfloat> diff{ points[i].diffuse.x, points[i].diffuse.y, points[i].diffuse.z };
                    ImGui::InputFloat3("R,G,B amb", diff.data());
                    points[i].diffuse = vec3(diff.at(0), diff.at(1), diff.at(2));
                    ImGui::TreePop();
                    ImGui::Spacing();
                }
                if (ImGui::TreeNode("specular"))
                {
                    static auto& spec = spots[i].specular;
                    setLights(spec, "spec");
                    ImGui::TreePop();
                    ImGui::Spacing();
                }
                if (ImGui::TreeNode("attenuation"))
                {
                    //move this declaration to a higher scope later
                    ImGui::SliderFloat("Constant", &spots[i].constant, 0.0f, 1.0f, "&.05f");
                    ImGui::SliderFloat("Linear", &spots[i].linear, 0.0f, 1.0f, "&.05f");
                    ImGui::SliderFloat("quadratic", &spots[i].quadratic, 0.0f, 1.0f, "&.05f");
                    ImGui::TreePop();
                    ImGui::Spacing();
                }
                if (ImGui::TreeNode("Cutoff"))
                {
                    ImGui::SliderFloat("Cutoff", &spots[i].cutOff, 0.01f, 1.0f);
                    ImGui::SliderFloat("outerCutoff", &spots[i].OuterCutoff, 0.0f, 1.0f, "&.05f");
                    ImGui::TreePop();
                    ImGui::Spacing();
                }
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Direct Light"))
        {
            static GLfloat pos[3] = { 0.0f, -1.0f, -0.5f };
            ImGui::InputFloat3("x,y,z", pos);
            direct.direction = vec3(pos[0], pos[1], pos[2]);
            ImGui::SeparatorText("ambient");

            static std::vector<GLfloat> amb{ 0.05f,0.05f,0.05f };
            ImGui::InputFloat3("R,G,B amb", amb.data());
            direct.diffuse = vec3(amb.at(0), amb.at(1), amb.at(2));
            

            ImGui::SeparatorText("diffuse");
            static std::vector<GLfloat> diff{0.3f,0.3f,0.3f};
            ImGui::InputFloat3("R,G,B diff", diff.data());
            direct.diffuse = vec3(diff.at(0), diff.at(1), diff.at(2));

            ImGui::SeparatorText("specular");
            static std::vector<GLfloat> spec{0.4f,0.4f,0.4f};
            ImGui::InputFloat3("R,G,B spec", spec.data());
            direct.specular = vec3(spec.at(0), spec.at(1), spec.at(2));
            ImGui::TreePop();
            ImGui::Spacing();
        }
        if (ImGui::TreeNode("Material"))
        {
            ImGui::InputFloat("Shine", &shine, 0.0, 128.0);
            ImGui::TreePop();
            ImGui::Spacing();
        }

    }
    ImGui::End();
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

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void setLights(vec3& lightValues, string name)
{
    ImGui::InputFloat(("R"s + name).c_str(), &lightValues[0], 0.00f, 1.0f);
    ImGui::InputFloat(("G"s + name).c_str(), &lightValues[1], 0.00f, 1.0f);
    ImGui::InputFloat(("B"s + name).c_str(), &lightValues[2], 0.00f, 1.0f);
}