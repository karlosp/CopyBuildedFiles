#pragma once
#include "StdAfx.h"

namespace cbf
{
using nlohmann::json;
namespace fs = std::filesystem;

inline json get_untyped(const json& j, const char* property)
{
  if (j.find(property) != j.end())
  {
    return j.at(property).get<json>();
  }
  return json();
}

struct DefaultProjects
{
  std::vector<std::string> projects;
};

struct Platform
{
  fs::path src_root_path;
  std::vector<fs::path> build_folder_names;
  std::vector<fs::path> platform_specific_files;
  fs::path dst_folder_path;
  bool use_default_projects;
};

struct Product
{
  std::vector<Platform> platforms;
};

struct CopyBuildedFiles
{
  std::unordered_map<std::string, DefaultProjects> default_projects;
  std::unordered_map<std::string, Product> products;
};

inline void from_json(const json& j, cbf::Platform& platform)
{
  platform.src_root_path = j.at("src_root_path").get<std::string>();
  platform.use_default_projects = j.at("use_default_projects").get<bool>();

  for (const auto& build_folder_name : j.at("build_folder_names").get<std::vector<std::string>>())
  {
    platform.build_folder_names.push_back(build_folder_name);
  }

  for (auto const& platform_specific_file :
       j.at("platform_specific_files").get<std::vector<std::string>>())
  {
    platform.platform_specific_files.emplace_back(platform_specific_file);
  }
  platform.dst_folder_path = j.at("dst_folder_path").get<std::string>();
}

inline void from_json(const json& j, cbf::DefaultProjects& default_project)
{
  default_project.projects.assign(j.begin(), j.end());
}

inline void from_json(const json& j, cbf::Product& product) { int a = 0; }

inline void from_json(const json& root, cbf::CopyBuildedFiles& cbf)
{
  const auto default_projects = root["default_projects"];
  for (auto default_project = default_projects.cbegin(); default_project != default_projects.cend();
       ++default_project)
  {
    // std::cout << default_project.key() << "\n";
    from_json(default_project.value(), cbf.default_projects[default_project.key()]);
  }

  const auto products = root["products"];
  for (auto product = products.cbegin(); product != products.cend(); ++product)
  {
    std::cout << product.key() << "\n";
    const auto product_platforms = product.value().at("platforms");

    for (auto product_platform = product_platforms.cbegin();
         product_platform != product_platforms.cend(); ++product_platform)
    {
      cbf::Platform platform;
      from_json(product_platform.value(), platform);
      cbf.products[product.key()].platforms.emplace_back(std::move(platform));
      // from_json(product.value(), cbf.products[product.key()]);
      // cbf.products[product.key()].platforms.push_back(&cbf.platforms.at(product_platform.key()));
    }
  }
}

struct CopyReport
{
  struct FromTo
  {
    fs::path from;
    fs::path to;
  };
  std::vector<FromTo> succeeded;
  std::vector<fs::path> failed;
  std::vector<fs::path> non_existing_sources;
};

enum class CopyMode
{
  ONLY_CHECK_SOURCES,
  COPY_AND_CHECK_COURCES
};

// Check if all source files exists, if not, returns paths of those who do not exists
CopyReport copy_check_sources(
  const cbf::CopyBuildedFiles& cbf, std::shared_ptr<spdlog::logger> logger,
  cbf::CopyMode copy_mode = CopyMode::COPY_AND_CHECK_COURCES);

}  // namespace cbf
