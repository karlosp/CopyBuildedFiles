#include <set>
#include <iostream>
#include <vector>
#include "ConvertProjectsFromBuildAll.hpp"
#include "spdlog/fmt/fmt.h"
#include "spdlog/fmt/ostr.h"

namespace cpfba = ConvertProjectsFromBuildAll;

namespace
{
std::string replace_all(
  const std::string& str,     // where to work
  const std::string& find,    // substitute 'find'
  const std::string& replace  //      by 'replace'
)
{
  using namespace std;
  string result;
  size_t find_len = find.size();
  size_t pos, from = 0;
  while (string::npos != (pos = str.find(find, from)))
  {
    result.append(str, from, pos - from);
    result.append(replace);
    from = pos + find_len;
  }
  result.append(str, from, string::npos);
  return result;
  /*
      This code might be an improvement to James Kanze's
      because it uses std::string methods instead of
      general algorithms [as 'std::search()'].
  */
}

}  // namespace

/*----------------------------------------------------
  This is auxiliary project to parse projects from
  existing .bat files
------------------------------------------------------*/
int main()
{
  std::vector<cpfba::BuildAllFiles> all_files{
    {"ACAD 2013-4, BCAD V13-5", R"(C:\SVN2017\Install\BuildMachinesTasks\V-W764U-BLD3\BuildAll.bat)"},
    {"ACAD 2015-6", R"(C:\SVN2017\Install\BuildMachinesTasks\V-W764U-BLD4\BuildAll.bat)"},
    {"BCAD V16-18", R"(C:\SVN2017\Install\BuildMachinesTasks\V-W764U-BLD5\BuildAll.bat)"},
    {"ACAD 2017", R"(C:\SVN2017\Install\BuildMachinesTasks\V-W1064U-BLD6\BuildAll.bat)"},
    {"ACAD 2018", R"(C:\SVN2017\Install\BuildMachinesTasks\V-W1064U-BLD7\BuildAll.bat)"},
    {"ACAD 2019", R"(C:\SVN2017\Install\BuildMachinesTasks\V-W1064U-BLD8\BuildAll.bat)"}};

  for (const auto& all_file : all_files)
  {
    std::cout << all_file.build_all << "\n\n";
    auto BuildAll_bats = cpfba::get_BuildAll_bats(all_file.build_all);
    for (auto const& BuildAll_bat : BuildAll_bats)
    {
      auto project_paths = cpfba::get_project_paths(BuildAll_bat);
      std::sort(project_paths.begin(), project_paths.end(), [](auto const& rhs, auto const& lhs) {
        return rhs.build_folder_name < lhs.build_folder_name;
      });

      // Get unique build_folder_names
      std::set<std::filesystem::path> build_folder_names;
      for (auto& project_path : project_paths)
      {
        build_folder_names.emplace(project_path.build_folder_name);
      }
      //Platform: 'ACAD 2018', file: 'C:\SVN2017\Install\ARX\CGS2017All-Build14u3x64.bat'
      fmt::print("Platform: '{}', file: '{}'\n", all_file.platform_name,BuildAll_bat.string());
      /*
      "build_folder_names": [
        "Rel015x64",
        "Rel15x64",
        "RelC2019x64",
        "RelCB15x64",
        ],
      */
      std::cout << R"("build_folder_names": [)" << "\n";
      for (auto const& build_folder_name: build_folder_names)
      {
        fmt::print("{},\n", build_folder_name);
      }
      std::cout << "],\n";
      for (auto const& project_path : project_paths)
      {
        auto project_relative_path = project_path.project_relative_path.string();
        project_relative_path = replace_all(project_relative_path, "\\", "\\\\");
        std::string extension = "arx";
        if (project_path.project_relative_path.stem() == "bin")
        {
          extension = "dll";
        }
        /* std::cout << fmt::format("\t{}\t", project_path.build_folder_name)
                   << fmt::format(R"("{}\\*\\*.{}",)", project_relative_path, extension)
                   << "\n";*/
        std::cout << "\t" << fmt::format(R"("{}\\*\\*.{}",)", project_relative_path, extension)
                  << "\n";
      }
    }
    std::cout << "\n\n";
    std::system("pause");
  }

  return 0;
}
