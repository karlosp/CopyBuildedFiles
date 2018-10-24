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

int main()
{
  std::vector<cpfba::BuildAllFiles> all_files{
    //{"ACAD 2018", R"(C:\SVN2017\Install\BuildMachinesTasks\V-W1064U-BLD7\BuildAll.bat)"},
    {"ACAD 2019", R"(C:\SVN2017\Install\BuildMachinesTasks\V-W1064U-BLD8\BuildAll.bat)"}};

  for (const auto& all_file : all_files)
  {
    auto result = cpfba::get_BuildAll_bats(all_file.build_all);
    for (auto const& res : result)
    {
      std::cout << res.string() << "\n";
      auto project_paths = cpfba::get_project_paths(res);
      std::sort(project_paths.begin(), project_paths.end(), [](auto const& rhs, auto const& lhs) {
        return rhs.build_folder_name < lhs.build_folder_name;
      });
      for (auto const& project_path : project_paths)
      {
        auto project_relative_path = project_path.project_relative_path.string();
        project_relative_path = replace_all(project_relative_path, "\\", "\\\\");
        std::string extension = "arx";
        if (project_path.project_relative_path.stem() == "bin")
        {
          extension = "dll";
        }
        std::cout << "\t" << fmt::format(R"("{}\\*\\*.{}",)", project_relative_path, extension)
                  << "\n";
      }
    }
    std::cout << "\n";
  }

  return 0;
}
