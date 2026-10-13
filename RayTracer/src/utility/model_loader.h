#pragma once

#include "../hittables/mesh.h"

#include <string>
#include <codecvt>

class model_loader
{
public:
	static mesh load_model(const char*);
	static std::wstring exe_path();
	static std::string to_string(const std::wstring&);
private:
	static std::string get_model_path() { return "\\assets\\RAYTRACING_MODELS\\"; }
	static void split(std::string line, std::vector<std::string>& out);
};