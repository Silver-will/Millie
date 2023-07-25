#include"General_utility.h"
#include<iostream>
namespace Glob
{
	const int WIDTH = 1028;
	const int HEIGHT = 720;
	GLfloat sensitivity = 0.05f;
	bool fMouse = true;
	GLfloat lastX{}, lastY{}, xOffset{}, yOffset{};
	active_shading_model active{active_shading_model::none};
}
void Log(const string message, const std::source_location loc)
{
	std::cout << "ERROR\n" << message << " in: "
		<< loc.file_name() << " At or before line: " << loc.line() << "\n";
}