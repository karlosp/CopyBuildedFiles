// clang-format off
#include "StdAfx.h"
// clang-format on
#include "CopyProgramFiles/CopyProgramFiles.hpp"

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

bool safe_exists(std::filesystem::path const& p)
{
  try
  {
    return fs::exists(p);
  }
  catch (const std::exception&)
  {
    return false;
  }

  return false;
}

// E.g:  "UrbanoInterface/arx/VS2015u3_64/*.arx"
// If path contains no wild cards returns unmodified path.
std::vector<fs::path> list_wildcard_files(fs::path const& path)
{
  std::vector<fs::path> files;

  if (path.stem() == "*")
  {
    // at least one file with extension must exits
    const auto parent_path = fs::path(path).parent_path();

    if (safe_exists(parent_path))
    {
      for (auto& file : fs::directory_iterator(parent_path))
      {
        if (file.is_regular_file())
        {
          if (file.path().extension() == path.extension())
          {
            files.push_back(file);
          }
        }
      }
    }
  }
  else
  {
    files.push_back(path);
  }

  return files;
}

cbf::CopyReport cbf::copy_check_sources(
  const cbf::CopyProgramFiles& cbf, std::shared_ptr<spdlog::logger> logger, cbf::CopyMode copy_mode)
{
  CopyReport cr;

  for (const auto& product : cbf.products)
  {
    for (const auto& platform : product.second.platforms)
    {
      std::vector<fs::path> files_to_be_copied;
      // Check for platform root
      if (!safe_exists(platform.src_root_path))
      {
        cr.non_existing_sources.push_back(platform.src_root_path);
        continue;
      }

      // Check for specific files
      for (const auto& plaform_specific_file : platform.platform_specific_files)
      {
        const auto absolute_path = platform.src_root_path / plaform_specific_file;
        const auto full_path_wildcard_files = list_wildcard_files(absolute_path);

        if (full_path_wildcard_files.empty())
        {
          cr.non_existing_sources.push_back(absolute_path);
        }
        else
        {
          for (auto const& full_path_wildcard_file : full_path_wildcard_files)
          {
            if (!safe_exists(full_path_wildcard_file))
            {
              cr.non_existing_sources.push_back(full_path_wildcard_file);
            }
          }
          if (copy_mode == CopyMode::COPY_AND_CHECK_COURCES)
          {
            std::copy(
              full_path_wildcard_files.begin(), full_path_wildcard_files.end(),
              std::back_inserter(files_to_be_copied));
          }
        }
      }

      // Check default projects, but only for defined products
      if (cbf.default_projects.count(product.first))
        for (const auto& project : cbf.default_projects.at(product.first).projects)
        {
          const auto platform_project_paths = get_platform_project_path(project, platform);

          std::vector<fs::path> wildcard_files;
          // In one of [Rel15, Rel015] find at least one file
          for (auto const& platform_project_path : platform_project_paths)
          {
            const auto tmp_files =
              list_wildcard_files(platform.src_root_path / platform_project_path);
            std::copy(tmp_files.begin(), tmp_files.end(), std::back_inserter(wildcard_files));

          }  // END In one of [Rel15, Rel015]

          if (wildcard_files.empty() && !platform_project_paths.empty())
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
            cr.non_existing_sources.push_back(platform_project_path.append(build_folder_names));
          }
          else
          {
            if (copy_mode == CopyMode::COPY_AND_CHECK_COURCES)
            {
              std::copy(
                wildcard_files.begin(), wildcard_files.end(),
                std::back_inserter(files_to_be_copied));
            }
          }
        }

      if (copy_mode == CopyMode::COPY_AND_CHECK_COURCES)
      {
        logger->info(
          "Files to be copied ({}) [Product: '{}',\t Src: '{}']", files_to_be_copied.size(),
          product.first, platform.src_root_path.string());
        for (auto const& file : files_to_be_copied)
        {
          try
          {
            fs::create_directories(platform.dst_folder_path);
          }
          catch (const std::exception&)
          {
            logger->error("Can not create folder: {}", platform.dst_folder_path.string());
          }
          try
          {
            fs::copy(
              file, platform.dst_folder_path, std::filesystem::copy_options::overwrite_existing);
            cr.succeeded.emplace_back(CopyReport::FromTo{file, platform.dst_folder_path});
          }
          catch (const std::exception& e)
          {
            logger->error("{}", e.what());
            cr.failed.push_back(file);
          }
        }
      }
    }  // End for platform
  }    // End for project

  return cr;
}
