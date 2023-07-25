#ifndef UI_H
#define UI_H
#include"imgui/imgui.h"
#include"imgui/imgui_impl_opengl3.h"
#include"imgui/imgui_impl_glfw.h"
#include"General_utility.h"
void SetupUI(bool* p_open);
//opens a native file explorer context to enable anyone to load models
string GetPathFromFileDialog();
static void HelpMarker(const char* desc);
#endif