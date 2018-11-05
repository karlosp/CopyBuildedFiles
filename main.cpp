#include "StdAfx.h"

#include "CopyProgramFiles/CopyProgramFiles.hpp"

// for convenience
using json = nlohmann::json;
namespace fs = std::filesystem;

// time find . -name "*.dll" | sed -r "s/\.\/(.*)\/Rel.*dll/\"\1\/*\/*\.dll\",/p" | sort | uniq
// time find . -name "*.arx" | sed -r "s/\.\/(.*)\/Rel.*arx/\"\1\/*\/*\.arx\",/p" | sort | uniq
int main(int argc, char* argv[])
{
  // Create logger
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
  auto file_sink =
    std::make_shared<spdlog::sinks::basic_file_sink_st>("CopyProgramFiles_log.txt", true);
  file_sink->set_level(spdlog::level::trace);

  std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
  auto multi_sink = std::make_shared<spdlog::logger>("multi_sink", begin(sinks), end(sinks));
  multi_sink->set_level(spdlog::level::debug);

  // register it if you need to access it globally
  // spdlog::register_logger(multi_sink);

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
      std::cout << "Selected json file: '" << json_file.string() << "'\n";

      if (!fs::exists(json_file))
      {
        multi_sink->error("Json file '{}' does not exists.", json_file.string());
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
    multi_sink->critical(
      "Error when parsing file '{}':\n'{}'! \nAborting!", json_file.string(), e.what());
    return 1;
  }

  cbf::CopyProgramFiles copy_program_files;
  cbf::from_json(settings, copy_program_files);

  auto copy_report =
    cbf::copy_check_sources(copy_program_files, multi_sink, cbf::CopyMode::ONLY_CHECK_SOURCES);
  if (!copy_report.non_existing_sources.empty())
  {
    std::string error_msg = fmt::format(
      "Following source files foes not exist ({}):\n", copy_report.non_existing_sources.size());
    for (const auto& file : copy_report.non_existing_sources)
    {
      error_msg.append(file.string()).append("\n");
    }
    multi_sink->critical("Error: {}\n\nAborting!", error_msg);
    return 0;
  }
  else
  {
    multi_sink->info("All source files exists!");
    multi_sink->info("Start copying, this can take awhile ... ");
    const auto copy_report = cbf::copy_check_sources(copy_program_files, multi_sink);

#ifdef _DEBUG
    multi_sink->debug("In debug mode!");
#else
    // Do not write debug information in console if not in DEBUG mode
    console_sink->set_level(spdlog::level::info);
#endif

    const auto not_copied_txt =
      fmt::format("Files that were not copied ({}):", copy_report.failed.size());
    if (copy_report.failed.empty())
    {
      // We just want to inform, that there were no errors
      multi_sink->info(not_copied_txt);
    }
    else
    {
      multi_sink->error(not_copied_txt);
    }
    for (auto const& file : copy_report.failed)
    {
      multi_sink->debug("{}", file.string());
    }

    multi_sink->info("Copy succeeded ({})!", copy_report.succeeded.size());
    multi_sink->debug("Succeeded list");
    int counter = 0;
    for (auto const& from_to : copy_report.succeeded)
    {
      multi_sink->debug("{:>3} {} => {}", ++counter, from_to.from.string(), from_to.to.string());
    }
  }

  return 0;
}
