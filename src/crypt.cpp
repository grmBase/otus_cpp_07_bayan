//---------------------------------------------------------------------------
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <iomanip>  //setfill
//---------------------------------------------------------------------------
#include <boost/crc.hpp>
//---------------------------------------------------------------------------
#include "crypt.h"
//---------------------------------------------------------------------------
#include "md5.h"
//---------------------------------------------------------------------------




std::string impl::calc_crc(uint8_t* ap_bin_buf, size_t aun_size)
{
  boost::crc_32_type result;

  result.process_bytes(ap_bin_buf, aun_size);

  uint32_t un_crc = result.checksum();

  std::ostringstream stream;

  stream << std::hex << std::setfill('0');
  stream << un_crc;

  return stream.str();
}
//---------------------------------------------------------------------------


// зачитываем блок из файла, рассчитываем для него hash
std::string impl::load_hash(const std::string& astr_filename, size_t aun_start_pos, size_t aun_length, size_t aun_block_length)
{

  std::ifstream file(astr_filename, std::ios::binary);

  // всегда используем вектор длинной блока, обнуляем:
  std::vector<uint8_t> vec_buf(aun_block_length, 0);


  file.seekg(aun_start_pos);

  file.read((char*)(vec_buf.data()), aun_length); // читаем, сколько сказали:

  return calc_crc(vec_buf.data(), vec_buf.size());
}
//---------------------------------------------------------------------------



void impl::test_calc_md5()
{
  MD5 md5;

  std::string myHash = md5("Hello World");     // std::string
  std::string myHash2 = md5("How are you", 11); // arbitrary data, 11 bytes

  std::cout << "md5, first: " << myHash << ", second: " << myHash2 << std::endl;
}
//---------------------------------------------------------------------------
