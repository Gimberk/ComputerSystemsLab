#include "model_loader.h"

#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <iostream>
#include <cstdlib>

struct Face {
    vec3 vertex_indices;
    vec3 normal_indices;
};

std::string model_loader::get_model_path() { 
    #ifndef _MSC_VER
        return "/csl/users/2027jfleming/Desktop/ComputerSystemsLab/RayTracer/assets/obj/";
    #else
        return "C:\\Users\\james\\source\\repos\\RayTracer\\RayTracer\\assets\\obj\\";
    #endif
}

std::vector<std::shared_ptr<triangle>> model_loader::load_model(const std::string path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file at " << path << std::endl;
        std::abort();
    }

    std::vector<vec3> vertices;
    std::vector<vec3> normals;
    std::vector<Face> faces;
    std::string line;

    while (std::getline(file, line)) {
        if (line.size() <= 1) continue;
        if (line.substr(0, 2) == "v ") {
            std::istringstream s(line.substr(2));
            vec3 v;
            s >> v.x; s >> v.y; s >> v.z;
            vertices.push_back(v);
        }
        else if (line.substr(0, 3) == "vn ") {
            std::istringstream s(line.substr(3));
            vec3 n;
            s >> n.x; s >> n.y; s >> n.z;
            normals.push_back(n);
        }
        else if (line.substr(0, 2) == "f ") {
            std::istringstream s(line.substr(2));
            Face face;
            char dummy;

            // OBJ indices are 1-based, convert them to 0-based
            s >> face.vertex_indices.x >> dummy >> dummy >> face.normal_indices.x;
            face.vertex_indices.x--;
            face.normal_indices.x--;

            s >> face.vertex_indices.y >> dummy >> dummy >> face.normal_indices.y;
            face.vertex_indices.y--;
            face.normal_indices.y--;

            s >> face.vertex_indices.z >> dummy >> dummy >> face.normal_indices.z;
            face.vertex_indices.z--;
            face.normal_indices.z--;

            faces.push_back(face);
        }
    }
    
    file.close();

    std::vector<std::shared_ptr<triangle>> tris;
    for (const Face& face : faces) {
        vec3 vertexA = vertices[face.vertex_indices.x];
        vec3 vertexB = vertices[face.vertex_indices.y];
        vec3 vertexC = vertices[face.vertex_indices.z];

        vec3 normalA = normals[face.normal_indices.x];
        vec3 normalB = normals[face.normal_indices.y];
        vec3 normalC = normals[face.normal_indices.z];

        std::shared_ptr<triangle> tri = std::make_shared<triangle>(std::array<vec3, 3>{vertexA, vertexB, vertexC}, std::make_unique<material>(get_null_mat()));
        
        vec3 triangleNormals[3] = { normalA, normalB, normalC };
        //tri.SetNormals(triangleNormals); // only needed for barycentric shading
        tris.push_back(tri);
    }

    return tris;
}


void model_loader::split(std::string line, std::vector<std::string>& out) {
	std::string buffer;
	for (int i = 0; i < line.size(); i++) {
		const char c = line[i];
		if (c == '\n' || c == ' ') {
			out.push_back(buffer);
			buffer = "";
		}
		else buffer += c;
	}

	out.push_back(buffer);
}

/*
std::wstring model_loader::exe_path() {
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
	return std::wstring(buffer).substr(0, pos);
}

std::string model_loader::to_string(const std::wstring& wstr)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;
	return converterX.to_bytes(wstr);
}
*/