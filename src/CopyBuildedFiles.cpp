// clang-format off
#include "StdAfx.h"
// clang-format on
#include "CopyBuildedFiles/CopyBuildedFiles.hpp"

namespace
{
namespace fs = std::filesystem;

inline fs::path get_platform_project_path(std::string project, cbf::Platform const& platform)
{
  const auto star_pos = project.find_first_of("*");

  if (star_pos != std::string::npos)
  {
    // Example:
    // From:  CGSA_C3D/ARX/C3D_LIB/*/*.arx"
    // To:    CGSA_C3D/ARX/C3D_LIB/Rel15/*.arx"
    project.replace(star_pos, 1u, platform.build_folder_name);
  }

  //  IF we did not find any * e.g.
  // 'UrbanoInterface/arx/VS2015u3_64/ARSXUrbanoInterface.arx'
  // returns original path.

  return project;
}
}  // namespace

tl::expected<bool, std::vector<std::filesystem::path>> cbf::check_sources(
  const cbf::CopyBuildedFiles& cbf)
{
  std::vector<fs::path> non_existing_sources;

  for (const auto& product : cbf.products)
  {
    for (const auto& platform : product.platforms)
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

      // Check default projects

      for (const auto& project : cbf.default_projects)
      {
        const auto platform_project_path = get_platform_project_path(project, platform);

        // If path contains wildcard check if at last one file with that extension exists
        if (platform_project_path.stem() == "*")
        {
          // at least one file with extension must exits
          int counter = 0;
          const auto parent_path =
            fs::path(platform.src_root_path / platform_project_path).parent_path();
          try
          {
            for (auto& p : fs::directory_iterator(parent_path))
            {
              if (p.is_regular_file())
              {
                if (p.path().extension() == platform_project_path.extension())
                {
                  ++counter;
                }
              }
            }

            if (counter == 0)
            {
              non_existing_sources.push_back(platform_project_path);
            }
          }
          catch (const std::exception& e)
          {
            non_existing_sources.push_back(parent_path);
          }
        }
        // If we have fixed file, just check if not exists.
        else if (!fs::exists(platform_project_path))
        {
          non_existing_sources.push_back(platform_project_path);
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
