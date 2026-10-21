#pragma once

#include <string>
#include <codecvt>
#include <vector>
#include <memory>

#include "../hittables/triangle.h"

class model_loader
{
public:
	static std::vector<std::shared_ptr<triangle>> load_model(std::string);
	static std::string get_model_path();
	//static std::wstring exe_path();
	//static std::string to_string(const std::wstring&);
private:
	static void split(std::string line, std::vector<std::string>&);
};