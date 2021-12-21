//-----------------------------------------------------------------------------
#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
//-----------------------------------------------------------------------------
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
//-----------------------------------------------------------------------------
#include "work_obj.h"
//-----------------------------------------------------------------------------


namespace bpo = boost::program_options;

int main(int argc, const char* argv[])
{

  size_t un_depth = 2; // на сколько глубоко искать:
  size_t un_block_size = 3; // размера блока для рассчёта хеша
  size_t un_min_file_size = 1; // минимальный размер файла, который будем обрабатывать
  std::string str_regex;

  bpo::options_description desc{ "Options" };
  desc.add_options()
    ("help,h", "Help screen")
    //("pi", value<float>()->implicit_value(3.14f), "Pi")

    ("depth", bpo::value<size_t>(&un_depth), "Depth")
    ("block_size", bpo::value<size_t>(&un_block_size), "BlockSize")
    ("min_file_size", bpo::value<size_t>(&un_min_file_size), "MinFileSize")

    ("dirs", bpo::value<std::vector<std::string>>()->multitoken()->zero_tokens()->composing(), "Dirs")
    ("skip_dirs", bpo::value<std::vector<std::string>>()->multitoken()->zero_tokens()->composing(), "Skip dirs")
    ("regex", bpo::value<std::string>(&str_regex), "Skip dirs")
    ;


  bpo::command_line_parser parser{ argc, argv };
  parser.options(desc).allow_unregistered().style(
    bpo::command_line_style::default_style | bpo::command_line_style::allow_slash_for_short);
  bpo::parsed_options parsed_options = parser.run();

  bpo::variables_map vm;
  bpo::store(parsed_options, vm);
  bpo::notify(vm);


  if (vm.count("help")) {
    std::cout << desc << '\n';
  }

  /*
  if (vm.count("depth")) {
    std::cout << "depth: " << un_depth << std::endl;
  }
  if (vm.count("block_size")) {
    std::cout << "block_size: " << un_block_size << std::endl;
  }
  if (vm.count("min_file_size")) {
    std::cout << "min_file_size: " << un_min_file_size << std::endl;
  }
  if (vm.count("regex")) {
    std::cout << "regex: " << str_regex << std::endl;
  }
  */

  if(vm.count("dirs")) 
  {
    /*
    std::cout << "list of dirs to handle: " << std::endl;
    for (auto& curr : vm["dirs"].as<std::vector<std::string>>())
    {
      std::cout << curr << std::endl;
    }
    */
  }


  // перебросим в сет, чтобы быстрее искать по нему:
  std::unordered_set<std::string> set_skip_dirs;
  if(vm.count("skip_dirs"))
  {
  
    //std::cout << "list of dirs to skip: " << std::endl;
    for(auto& curr : vm["skip_dirs"].as<std::vector<std::string>>())
    {
      boost::filesystem::path bpath{curr};

      // сохраним нормализованные пути
      set_skip_dirs.insert(bpath.lexically_normal().string());

      //std::cout << curr << std::endl;
    }
  }


  std::cout << "going to start with following params:\r\n" << 
   "sub dir depth: " << un_depth << ", read block size: " << un_block_size << 
   ", min file size: " << un_min_file_size << ", regex: " << str_regex;

  // выводить все каталоги и списки исключений наверно будет уже болтливо

  t_work_obj workObj;

  try
  {
    int nResult = workObj.do_search(vm["dirs"].as<std::vector<std::string>>(), set_skip_dirs, 
      un_depth, un_block_size, un_min_file_size, str_regex);
    if (nResult) {
      std::cout << "Error in do_search(), code: " << nResult << std::endl;
      return nResult;
    }

  }
  catch (const std::exception& exc)
  {
    std::cout << "Exception caught. Info: " << exc.what() << std::endl;
    return -33;
  }

  return 0;
}
//-----------------------------------------------------------------------------

