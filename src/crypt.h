//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------

#include <string>


namespace impl
{

  // для простоты crc будем хранить в строках
  std::string calc_crc(uint8_t* ap_bin_buf, size_t aun_size);

  // вычисляем хеш для указанного блока файла
  std::string load_hash(const std::string& astr_filename, 
    size_t aun_start_pos, size_t aun_length, size_t aun_block_length);

  // тест
  void test_calc_md5();

}
//---------------------------------------------------------------------------
