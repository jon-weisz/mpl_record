#ifndef __MPL_DEBUG_TOOLS__
#define __MPL_DEBUG_TOOLS__
#include <iostream>
#include <cxxabi.h>



struct print{
    template <typename T>
    void operator()(const T &t) const
    {
      std::cout << std::boolalpha << t << '\n';
    }
};

struct print_info
{
     template <typename T>
     void operator()(const T &t) const
     {
         int status = -4;
         std::cout << abi::__cxa_demangle(typeid(t).name(), NULL, NULL, &status) << " " << t <<std::endl;
     }
};

#endif //__MPL_DEBUG_TOOLS__
