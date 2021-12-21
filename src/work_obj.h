//---------------------------------------------------------------------------
#ifndef __work_obj_h__
#define __work_obj_h__
//---------------------------------------------------------------------------

// "Псевдо" хеш для того, чтобы организовать хранение файла в хеше, но ещё без вычитывания его данных
// т.к. имеет разные буквы, то гарантировано не совпадёт ни с одним реальным хешем
const std::string c_str_hash_zero = "somezero";



// запись о файле. Для одного файла таких может быть много.
// такие записи храняться под разной длины хешами, в зависимости от того,
// какую общую длину нам пришлось вычитать, пока не нашли несовпадения
struct t_rec
{

  t_rec(bool af_is_final, const boost::filesystem::path& astr_filename, size_t aun_block_num)
    :m_f_is_final(af_is_final),
    m_str_filename(astr_filename),
    m_un_block_num(aun_block_num)
  {}

  // флаг является ли эта запись финальная, или же файл был прочитан где-то дальше
  bool m_f_is_final = false;

  // имя файла
  boost::filesystem::path m_str_filename;

  // номер блока, для которого здесь запись
  // 0 - ничего не читалось, 1 - это запись для 1-го блока и т.д.
  size_t m_un_block_num = 0;

  std::vector<std::string> m_str_doubles;
};
//---------------------------------------------------------------------------



// запись о разделе файлов, с одинаковыми размерами. Можно было и не делать, по идее
// но думал что вдруг появятся ещё данные
struct t_size_size_files
{
  std::unordered_map<std::string, t_rec> m_vec;
};
//---------------------------------------------------------------------------

// Тип хранилища файлов "по размеру"
using t_main_storage = std::unordered_map<size_t, t_size_size_files>;





class t_work_obj
{
  public:


    int do_search(
      const std::vector<std::string>& avec_dirs,
      const std::unordered_set<std::string>& aset_skip_dirs,
      size_t aun_depth, size_t aun_block_size, size_t un_min_file_size,
      const std::string_view& astr_file_regex);



  private:


    // для простоты crc будем хранить в строках
    std::string calc_crc(uint8_t* ap_bin_buf, size_t aun_size);

    // вычисляем хеш для указанного блока файла
    std::string load_hash(const std::string& astr_filename, 
      size_t aun_start_pos, size_t aun_length, size_t aun_block_length);

    // тест
    void test_calc_md5();


    // обрабатываем указанный файл
    int handle_file(const boost::filesystem::path& astr_file,
      t_size_size_files& a_size_storage,
      size_t aun_file_size, size_t aun_block_size);

    // рекурсивная функция обходов каталогов:
    int process_dir(const boost::filesystem::path& astr_dir, 
      const std::unordered_set<std::string>& aset_skip_dirs,
      size_t aun_max_depth, size_t aun_block_size, size_t un_min_file_size,
      const std::string_view& astr_file_regex,
      t_main_storage& a_main_storage);




    static void logout(const std::string_view& astr_view);
    static void logerr(const std::string_view& astr_view);

};
//---------------------------------------------------------------------------

#endif
