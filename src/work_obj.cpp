//---------------------------------------------------------------------------
#include <string>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/regex.hpp>

#include <iostream>

//#include <iomanip>

#include <iterator>
#include <algorithm>

#include <unordered_map>
#include <unordered_set>
#include <vector>

//---------------------------------------------------------------------------
#include "work_obj.h"
//---------------------------------------------------------------------------
#include "utils.h"
#include "crypt.h"
//---------------------------------------------------------------------------



int t_work_obj::do_search(
  //const boost::filesystem::path& a_path,
  const std::vector<std::string>& avec_dirs,
  const std::unordered_set<std::string>& aset_skip_dirs,
  size_t aun_depth, size_t aun_block_size, size_t un_min_file_size,
  const std::string_view& astr_file_regex)
{

  // Основное хранилище, исходно по размеру файлов
  t_main_storage main_storage;


  for(auto& str_dir: avec_dirs) 
  {

    boost::filesystem::path bpath_curr{ str_dir };


    // Есть ли такой путь?
    if(!boost::filesystem::exists(bpath_curr)) {
      impl::logout("direcotry '" + bpath_curr.string() + "' doesn't exists, so skipping it");
      continue;
    };


    if (aset_skip_dirs.find(bpath_curr.lexically_normal().string()) != aset_skip_dirs.end()) {
      impl::logout("direcotry '" + bpath_curr.string() + "' found in exclusion list, so skipping it");
      continue;
    }

    int n_result = process_dir(bpath_curr, aset_skip_dirs, 
      aun_depth, aun_block_size, un_min_file_size,
      astr_file_regex,
      main_storage);
    if (n_result) {
      impl::logout("error in process_dir()");
      continue;
    }
  }
  impl::logout("processing dirs has finished\r\n");


  impl::logout("\r\nprinting results:\r\n");
  // printing:
  for (auto& entry : main_storage)
  {
    std::cout << "bucket with filesize: " << entry.first << ", files : " << std::endl;

    for (const auto& entry2 : entry.second.m_vec) {

      // промежуточные не выводим:
      if (!entry2.second.m_f_is_final) {
        continue;
      }

      std::cout << "filename: " << entry2.second.m_str_filename << std::endl;

      size_t un_num_doubles = entry2.second.m_str_doubles.size();
      if (un_num_doubles)
      {
        std::cout << "  total num of doubles for it: " << un_num_doubles << ", list: " << std::endl;

        for (const auto& str_doubles : entry2.second.m_str_doubles) {
          std::cout << "    " << str_doubles << std::endl;
        }
      }

      std::cout << std::endl;
    }

  }

  return 0;
}
//---------------------------------------------------------------------------



namespace bfs = boost::filesystem;


int t_work_obj::handle_file(const boost::filesystem::path& astr_file,
  t_size_size_files& a_size_storage, 
  size_t aun_file_size, size_t aun_block_size)
{
  impl::logout(std::string("\r\nstart handling file: ") + astr_file.string());

  // максимальное число блоков, которое нужно читать для данного файла:
  size_t un_max_num_of_blocks = aun_file_size / aun_block_size;
  if (aun_file_size % aun_block_size) {
    un_max_num_of_blocks++;
  }

  impl::logout("total num of blocks: " + std::to_string(un_max_num_of_blocks));


  //size_t un_curr_num_of_block = 0;

  std::string str_curr_hash; // = c_str_hash_zero;

  // читать будем на +1 блоков больше, т.к. первым вычитаем не совсем честный псевдоблок:
  for (size_t un_curr_num_of_block = 0; un_curr_num_of_block < un_max_num_of_blocks + 1; un_curr_num_of_block++)
  {

    if (un_curr_num_of_block == 0) {         // на первой итерации всегда стандартная хеш сумма:
      str_curr_hash = c_str_hash_zero;
    }
    else
    {
      // если идём дальше, то уже читаем дальше куски нашего файла:

      // считаем, сколько нужно читать из файла:
      size_t un_num_to_read = 0;
      if (un_curr_num_of_block == un_max_num_of_blocks) { // на последнем блоке могут быть варианты:
        un_num_to_read = aun_file_size % aun_block_size;
      }
      else {
        un_num_to_read = aun_block_size;
      }

      // читаем, считаем хеш:
      std::string str_next_block_hash = impl::load_hash(astr_file.string(),
        (un_curr_num_of_block - 1) * aun_block_size, // да тут, -1, т.к. мы впереди впихнули один фиктивный
        un_num_to_read,
        aun_block_size);

      // формируем новый, более длинный ключ:
      str_curr_hash += "_" + str_next_block_hash;

      impl::logout("In iteration#: " + std::to_string(un_curr_num_of_block) + ", curr file new hash to check: " + str_curr_hash);
    }


    auto it = a_size_storage.m_vec.find(str_curr_hash);
    if (it == a_size_storage.m_vec.end()) {
      impl::logout("hash not found. We are first");
      a_size_storage.m_vec.emplace(str_curr_hash, t_rec(true, astr_file.string(), un_curr_num_of_block));
      impl::logout("map size after insert: " + std::to_string(a_size_storage.m_vec.size()));

      impl::logout("file handled, exiting\r\n");
      // если были первыми, то вываливаемся. Дальше сравниваться не с кем
      break;
    }
    else 
    {

      impl::logout("such hash already registered");
      // если уже есть такой файл, в котором наш кусок одинаков, то нужно 1) дочитать этот файл дальше
      // 2) дочитать наш блок дальше

      //1) смотрим нужно ли читать что-то для этого файла, или может он уже был прочитан (тогда будет флаг)
      {
        if (!it->second.m_f_is_final) {
          impl::logout("it is old record, this file already read some more blocks");
        }
        else 
        {
          // может быть файл уже дочитан до конца и дальше нельзя?
          if (it->second.m_un_block_num == un_max_num_of_blocks) {  // тут без -1, из-за первого фиктивного
            impl::logout("existing file already fully loaded. file: " + it->second.m_str_filename.string());

            impl::logout("found double. registered");
            it->second.m_str_doubles.push_back(astr_file.string());

            // вываливаемся
            break;
          }
          else
          {
            impl::logout("existing file had no next record, so read it. pre file: " + (it->second.m_str_filename).string());


            // считаем, сколько нужно читать из файла:
            size_t un_num_to_read = 0;
            if (it->second.m_un_block_num + 1 == un_max_num_of_blocks) { // на последнем блоке могут быть варианты:
              un_num_to_read = aun_file_size % aun_block_size;
            }
            else
            {
              un_num_to_read = aun_block_size;
            }

            std::string str_next_block_hash = impl::load_hash(it->second.m_str_filename.string(), 
              it->second.m_un_block_num * aun_block_size,
              un_num_to_read,
              aun_block_size);

            impl::logout("next block hash: " + str_next_block_hash);


            std::string str_next_hash = it->first + "_" + str_next_block_hash;

            impl::logout("exist file new hash to register is: " + str_next_hash);

            // регистрируем новую запись под более длинным хешем:
            a_size_storage.m_vec.emplace(str_next_hash, t_rec(true, it->second.m_str_filename, it->second.m_un_block_num + 1));

            //в старой записи убираем флаг, что она финальная (т.к. зарегистрировали более "длинную"):
            it->second.m_f_is_final = false;
            it->second.m_str_filename = "no_more_valid";

            impl::logout("new record for existing file added, previous record corrected");

            continue; // идём на следующий заход
          }

        }
      }

      impl::logout("hash found. Should do next investigation...");
    }

  }
  

  return 0;
}
//---------------------------------------------------------------------------


int t_work_obj::process_dir(const boost::filesystem::path& astr_dir, 
  const std::unordered_set<std::string>& aset_skip_dirs,
  size_t aun_max_depth, size_t aun_block_size, size_t un_min_file_size, 
  const std::string_view& astr_file_regex,
  t_main_storage& a_main_storage)
{
  for (auto& entry : boost::make_iterator_range(bfs::directory_iterator(astr_dir), {}))
  {

    if (bfs::is_directory(entry.path())) {

      impl::logout("found subdir: " + entry.path().string());

      if (aset_skip_dirs.find(entry.path().lexically_normal().string()) != aset_skip_dirs.end()) {
        impl::logout("dir is in exclusion list, so skipping it");
        continue;
      }
      
      // ниже нуля нельзя. Всё, выходим
      if (aun_max_depth == 0) {
        impl::logout("dir level is 0, so not going to recoursive");
        continue;
      }

      impl::logout("levels is: " + std::to_string(aun_max_depth) + ", so going to recourcive...");

      int n_result = process_dir(entry.path(), aset_skip_dirs, 
        aun_max_depth - 1, aun_block_size, un_min_file_size, astr_file_regex, 
        a_main_storage);
      if (n_result) {
        impl::logerr("error in process_dir()");
        return n_result;
      }
      continue;
    }

    if (!bfs::is_regular_file(entry.path())) {
      impl::logout("it is some not regular file, so skipped. entry: " + entry.path().string());
      continue;
    }

    // ----------- проверяем подходит ли: -----------------
    size_t file_size = bfs::file_size(entry.path());

    if (file_size == 0) {
      impl::logout("file size is zero, so we skipping this file");
      continue;
    }

    if (file_size < un_min_file_size) {
      impl::logout("file size is less than minimum specified, so skipped. file: " + entry.path().string() + 
        ", size: " + std::to_string(file_size) + 
        ", minsize: " + std::to_string(un_min_file_size));
      continue;
    }


    // проверяем на regexp
    if(astr_file_regex.size()) {
      boost::regex reg(std::string(astr_file_regex).c_str());
      boost::smatch what;

      if (!boost::regex_search(entry.path().string(), what, reg)) {
        impl::logout("file doesnt' match regex. so skipping it. file: " + entry.path().string());
        continue;
      }
    }

    impl::logout("it is a regular file, so going to handle it... name: " + entry.path().string() + 
      ", size: " + std::to_string(file_size));


    // ищем, если надо создаём хранилище по размеру файла:
    auto it = a_main_storage.find(file_size);
    if (it == a_main_storage.end())  {
      a_main_storage.emplace(file_size, t_size_size_files{});

      it = a_main_storage.find(file_size);
    }
    
    auto& size_storage = it->second;


    int n_result = handle_file(entry, size_storage, file_size, aun_block_size);
    if (n_result) {
      impl::logerr("error in handle_file()");
      return n_result;
    }

  }

  return 0;
}
//---------------------------------------------------------------------------
