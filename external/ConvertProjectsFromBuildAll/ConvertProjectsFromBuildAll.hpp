#pragma once
#include <string>
#include <vector>
#include <filesystem>

namespace ConvertProjectsFromBuildAll
{
namespace fs = std::filesystem;
struct BuildAllFiles
{
  std::string platform_name;
  fs::path build_all;
};

struct ProjectPath
{
  // e.g: CGSAPPS\ARX\CGSA_XML_LIB
  fs::path project_relative_path;

  // e.g: Rel15x64
  fs::path build_folder_name;
};

// Returns paths found in BuildAll.bat
std::vector<fs::path> get_BuildAll_bats(fs::path const& buildAll_path);

std::vector<ProjectPath> get_project_paths(fs::path const& product_bat_path);

}