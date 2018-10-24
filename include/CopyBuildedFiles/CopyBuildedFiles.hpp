#pragma once
#include "StdAfx.h"

namespace cbf {
using nlohmann::json;
namespace fs = std::filesystem;
// Generated from https://app.quicktype.io/

inline json get_untyped(const json& j, const char* property) {
  if (j.find(property) != j.end()) {
    return j.at(property).get<json>();
  }
  return json();
}

struct Platform {
  std::string name;
  fs::path src_root_path;
  std::string build_folder_name;
  bool use_default_projects;
  std::vector<fs::path> platform_specific_files;
  fs::path dst_folder_path;
};

struct Product {
  std::string name;
  std::vector<Platform> platforms;
};

struct CopyBuildedFiles {
  std::vector<Product> products;
  std::vector<std::string> default_projects;
};

inline void from_json(const json& _j, cbf::Platform& _x) {
  _x.name = _j.at("name").get<std::string>();
  _x.src_root_path = _j.at("src_root_path").get<std::string>();
  _x.build_folder_name = _j.at("build_folder_name").get<std::string>();
  _x.use_default_projects = _j.at("use_default_projects").get<bool>();
  for (auto const& p : _j.at("platform_specific_files").get<std::vector<std::string>>())
  {
    _x.platform_specific_files.emplace_back(p);
  }
  _x.dst_folder_path = _j.at("dst_folder_path").get<std::string>();
}

//inline void to_json(json& _j, const cbf::Platform& _x) {
//  _j = json::object();
//  _j["name"] = _x.name;
//  _j["src_root_path"] = _x.src_root_path;
//  _j["build_folder_name"] = _x.build_folder_name;
//  _j["use_default_projects"] = _x.use_default_projects;
//  _j["platform_specific_files"] = _x.platform_specific_files;
//  _j["dst_folder_path"] = _x.dst_folder_path;
//}

inline void from_json(const json& _j, cbf::Product& _x) {
  _x.name = _j.at("name").get<std::string>();
  _x.platforms = _j.at("platforms").get<std::vector<cbf::Platform>>();
}

//inline void to_json(json& _j, const cbf::Product& _x) {
//  _j = json::object();
//  _j["name"] = _x.name;
//  _j["platforms"] = _x.platforms;
//}

inline void from_json(const json& _j, cbf::CopyBuildedFiles& _x) {
  _x.products = _j.at("products").get<std::vector<cbf::Product>>();
  _x.default_projects =
      _j.at("default_projects").get<std::vector<std::string>>();
}

//inline void to_json(json& _j, const cbf::CopyBuildedFiles& _x) {
//  _j = json::object();
//  _j["products"] = _x.products;
//  _j["default_projects"] = _x.default_projects;
//}

// Check if all source files exists
tl::expected<bool, std::vector<std::filesystem::path>> check_sources(const cbf::CopyBuildedFiles& cbf);

inline void copy(const cbf::CopyBuildedFiles& cbf);
}  // namespace cbf
