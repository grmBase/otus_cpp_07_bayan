//---------------------------------------------------------------------------
#include <string>
#include <iostream>
//---------------------------------------------------------------------------
#include "utils.h"
//---------------------------------------------------------------------------




// пока включим отладку:
//#define DBG_LOGGING
#ifdef DBG_LOGGING
void impl::logout(const std::string_view& astr_view)
{
  std::cout << astr_view << std::endl;
}
#else
void impl::logout(const std::string_view&)
{
}
#endif
//---------------------------------------------------------------------------

void impl::logerr(const std::string_view& astr_view)
{
  std::cerr << astr_view << std::endl;
}
//---------------------------------------------------------------------------
