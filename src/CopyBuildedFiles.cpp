// clang-format off
#include "StdAfx.h"
// clang-format on
#include "CopyBuildedFiles/CopyBuildedFiles.hpp"

namespace
{
namespace fs = std::filesystem;

inline std::vector<fs::path> get_platform_project_path(
  std::string project, cbf::Platform const& platform)
{
  std::vector<fs::path> platform_project_paths;
  const auto first_star_pos = project.find("*");
  const auto file_ext_star_pos = project.find("*.");

  if (first_star_pos != std::string::npos && first_star_pos != file_ext_star_pos)
  {
    // Example:
    // From:  CGSA_C3D/ARX/C3D_LIB/*/*.arx"
    // To:    CGSA_C3D/ARX/C3D_LIB/(Rel15x64|Rel015x64)/*.arx"
    for (const auto& build_folder_name : platform.build_folder_names)
    {
      auto tmp_project_path = project;
      tmp_project_path.replace(first_star_pos, 1u, build_folder_name.string());
      platform_project_paths.push_back(tmp_project_path);
    }
  }
  else
  {
    platform_project_paths.push_back(project);
  }

  //  If we did not find any '*' e.g.
  // 'UrbanoInterface/arx/VS2015u3_64/ARSXUrbanoInterface.arx'
  // returns original path.

  return platform_project_paths;
}
}  // namespace

tl::expected<bool, std::vector<std::filesystem::path>> cbf::check_sources(
  const cbf::CopyBuildedFiles& cbf)
{
  std::vector<fs::path> non_existing_sources;

  for (const auto& product : cbf.products)
  {
    for (const auto& platform : product.second.platforms)
    {
      if (!fs::exists(platform.src_root_path))
      {
        non_existing_sources.push_back(platform.src_root_path);
        continue;
      }

      for (const auto& plaform_specific_file : platform.platform_specific_files)
      {
        const auto full_path = platform.src_root_path / plaform_specific_file;
        if (!fs::exists(full_path))
        {
          non_existing_sources.push_back(full_path);
        }
      }

      // Check default projects, but only for defined products
      for (const auto& projects : cbf.default_projects)
      {
        if (cbf.products.count(projects.first) == 0)
        {
          // Skip default project which are not used
          continue;
        }
        for (const auto& project : projects.second.projects)
        {
          const auto platform_project_paths = get_platform_project_path(project, platform);

          bool found = false;
          // In one of [Rel15, Rel015] find at least one file
          for (auto const& platform_project_path : platform_project_paths)
          {
            // If path contains wildcard check if at last one file with that extension exists
            if (platform_project_path.stem() == "*")
            {
              // at least one file with extension must exits
              const auto parent_path =
                fs::path(platform.src_root_path / platform_project_path).parent_path();
              try
              {
                if (fs::exists(parent_path))
                {
                  for (auto& p : fs::directory_iterator(parent_path))
                  {
                    if (p.is_regular_file())
                    {
                      if (p.path().extension() == platform_project_path.extension())
                      {
                        found = true;
                        break;
                      }
                    }
                  }
                }
              }
              catch (const std::exception&)
              {
                non_existing_sources.push_back(parent_path);
              }
            }
            // If we have fixed file, just check if not exists.
            else if (!fs::exists(platform_project_path))
            {
              non_existing_sources.push_back(platform_project_path);
            }
          }  // END In one of [Rel15, Rel015]
          if (!found)
          {
            // \\v-w1064u-bld7\SVN2017
            auto platform_project_path = platform.src_root_path;
            // \\v-w1064u-bld7\SVN2017\CGSA_TC\ARX\TC_SweptPath
            platform_project_path /= platform_project_paths.front().parent_path().parent_path();

            std::string build_folder_names;
            for (auto const& build_folder_name : platform.build_folder_names)
            {
              build_folder_names.append(build_folder_name.string()).append(",");
            }
            // remove last ","
            build_folder_names.pop_back();

            // E.g.: \\v-w1064u-bld7\SVN2017\CGSA_TC\ARX\TC_SweptPath\Rel14u3x64,RelC2018x64
            non_existing_sources.push_back(platform_project_path.append(build_folder_names));
          }
        }
      }
    }
  }

  if (!non_existing_sources.empty())
  {
    return tl::make_unexpected(non_existing_sources);
  }

  return true;
}

void cbf::copy(const CopyBuildedFiles& cbf) {}
