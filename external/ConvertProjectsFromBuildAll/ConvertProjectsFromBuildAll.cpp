#include "ConvertProjectsFromBuildAll.hpp"
#include <fstream>
#include <functional>
#include <regex>
#include <sstream>
#include <string>

namespace ConvertProjectsFromBuildAll
{
std::vector<fs::path> ConvertProjectsFromBuildAll::get_BuildAll_bats(fs::path const& buildAll_path)
{
  std::vector<fs::path> bat_paths;

  const std::regex txt_regex(R"(^call (C:\\SVN2017\\Install\\ARX\\.*x64\.bat))");
  std::smatch base_match;

  std::ifstream infile(buildAll_path);
  std::string line;
  while (std::getline(infile, line))
  {
    if (std::regex_match(line, base_match, txt_regex))
    {
      // The first sub_match is the whole string; the next
      // sub_match is the first parenthesized expression.
      if (base_match.size() == 2)
      {
        std::ssub_match base_sub_match = base_match[1];
        bat_paths.emplace_back(base_sub_match.str());
      }
    }
  }

  return bat_paths;
}

std::vector<ProjectPath> get_project_paths(fs::path const& product_bat_path)
{
  std::vector<ProjectPath> project_paths;

  const std::regex build_folder_name_regex("^call .*set_var_(Rel.*x64).bat");
  const std::regex project_relative_path_regex(
    R"(^MSBuild .*"C:.SVN2017.(CGSA.*)\\.*proj".*)", std::regex_constants::icase);
  std::smatch base_match;

  fs::path build_folder_name = "";

  std::ifstream infile(product_bat_path);
  std::string line;

  while (std::getline(infile, line))
  {
    // First we must find build folder name from
    // e.g.: "call %~dp0\BuildVariables\set_var_Rel15x64.bat" => "Rel15x64"
    if (std::regex_match(line, base_match, build_folder_name_regex))
    {
      // The first sub_match is the whole string; the next
      // sub_match is the first parenthesized expression.
      if (base_match.size() == 2)
      {
        std::ssub_match base_sub_match = base_match[1];
        build_folder_name = base_sub_match.str();
      }
    }
    if (std::regex_match(line, base_match, project_relative_path_regex))
    {
      // The first sub_match is the whole string; the next
      // sub_match is the first parenthesized expression.
      if (base_match.size() == 2)
      {
        std::ssub_match base_sub_match = base_match[1];
        ProjectPath project_path;
        project_path.build_folder_name = build_folder_name;
        project_path.project_relative_path = base_sub_match.str();

        // For NET project adds bin folder
        if (line.find("csproj") != std::string::npos)
        {
          project_path.project_relative_path.append("bin");
        }

        project_paths.push_back(project_path);
      }
    }
  }
  return project_paths;
}

}  // namespace ConvertProjectsFromBuildAll
