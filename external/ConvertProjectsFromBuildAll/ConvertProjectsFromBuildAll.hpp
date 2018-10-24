#pragma once
#include <string>
#include <vector>
#include <filesystem>

namespace ConvertProjectsFromBuildAll
{
namespace fs = std::filesystem;
struct BuildAllFiles
{
  //e.g: ACAD 2019"
  std::string platform_name;
  //e.g: C:\SVN2017\Install\BuildMachinesTasks\V-W1064U-BLD8\BuildAll.bat
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
// e.g: C:\SVN2017\Install\ARX\CGS2017All-Build15x64.bat, 
// C:\SVN2017\Install\ARX\Naviate\Naviate2017_15-Buildx64.bat
std::vector<fs::path> get_BuildAll_bats(fs::path const& buildAll_path);

// e.g: 
std::vector<ProjectPath> get_project_paths(fs::path const& product_bat_path);

}