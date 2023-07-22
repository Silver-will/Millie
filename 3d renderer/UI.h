#ifndef UI_H
#define UI_H
#include"imgui/imgui.h"
#include"imgui/imgui_impl_opengl3.h"
#include"imgui/imgui_impl_glfw.h"
#include"General_utility.h"
#include<nfd/include/nfd.h>
void SetupUI(bool* p_open);
string GetPathFromFileDialog();

#endif