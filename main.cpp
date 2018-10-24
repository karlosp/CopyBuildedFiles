#include "StdAfx.h"

#include "CopyBuildedFiles/CopyBuildedFiles.hpp"

// for convenience
using json = nlohmann::json;
namespace fs = std::filesystem;

// time find . -name "*.dll" | sed -r "s/\.\/(.*)\/Rel.*dll/\"\1\/*\/*\.dll\",/p" | sort | uniq
// time find . -name "*.arx" | sed -r "s/\.\/(.*)\/Rel.*arx/\"\1\/*\/*\.arx\",/p" | sort | uniq
int main(int argc, char* argv[])
{
  cxxopts::Options options(argv[0], " - example command line options");

  fs::path json_file;
  try
  {
    options.positional_help("[optional args]").show_positional_help();

    options.add_options()("j,json", "Path to .json file", cxxopts::value<std::string>())(
      "h,help", "Print help");

    auto result = options.parse(argc, argv);

    if (result.count("help") || result.arguments().empty())
    {
      std::cout << options.help({}) << std::endl;
      exit(0);
    }
    if (result.count("json"))
    {
      json_file = result["json"].as<std::string>();
      std::cout << "Selected json file: " << json_file << "\n";

      if (!fs::exists(json_file))
      {
        spdlog::error("Json file '{}' does not exists.", json_file.string());
        return 1;
      }
    }
  }
  catch (const cxxopts::OptionException& e)
  {
    std::cout << "error parsing options: " << e.what() << std::endl;
    std::cout << "\n" << std::endl;
    std::cout << options.help() << std::endl;
    exit(1);
  }

  std::ifstream json_file_stream(json_file.c_str());
  json settings;
  try
  {
    json_file_stream >> settings;
  }
  catch (const std::exception& e)
  {
    spdlog::critical(
      "Error when parsing file '{}':\n'{}'! \nAborting!", json_file.string(), e.what());
    return 1;
  }

  cbf::CopyBuildedFiles copy_build_files;
  cbf::from_json(settings, copy_build_files);

  auto check_sources_result = cbf::check_sources(copy_build_files);
  if (!check_sources_result)
  {
    std::string error_msg = fmt::format("Following source files foes not exist ({}):\n", check_sources_result.error().size());
    for (const auto& file : check_sources_result.error())
    {
      error_msg.append(file.string()).append("\n");
    }
    spdlog::critical("Error: {}\n\nAborting!", error_msg);
    return 0;
  }
  else
  {
    spdlog::info("All source files exists!");
  }

  /*  auto console = spdlog::stdout_color_mt("console");
    spdlog::info("Welcome to spdlog version {}.{}.{} !", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR,
    SPDLOG_VER_PATCH); console->info("Welcome to spdlog!"); std::string s = fmt::format("{0}{1}{0}",
    "abra", "cad"); console->error("Some error message with arg: {}", s);

    auto err_logger = spdlog::stderr_color_mt("stderr");
    err_logger->error("Some error message");
    std::ifstream i("../copy.json");
    json copy_json;
    i >> copy_json;


    std::cout << copy_json.dump(2) << "\n\n\n";

    for (auto& j : copy_json["products"])
    {
      std::cout << j.dump(2) << "\n";
    }

    for (auto& j : copy_json)
    {
      std::cout << j << "\n";
    }*/
  return 0;
}
