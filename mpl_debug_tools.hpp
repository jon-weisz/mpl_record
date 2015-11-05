#ifndef __MPL_DEBUG_TOOLS__
#define __MPL_DEBUG_TOOLS__
#include <iostream>
#include <cxxabi.h>
#include <string>
#include <regex>
#include <boost/xpressive/xpressive.hpp>
#include <boost/fusion/include/for_each.hpp>

struct print{
    template <typename T>
    void operator()(const T &t) const
    {
      std::cout << std::boolalpha << t << '\n';
    }
};

struct print_name
{
        template <typename T>
        void operator()(const T &t) const
                {
                        int status = -4;
                        std::cout << abi::__cxa_demangle(typeid(t).name(), NULL, NULL, &status) << " " <<std::endl;
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

struct print_name_for_each
{
    template <typename T>
    void operator()(const T &t) const
        {
            boost::fusion::for_each(t, print_name());
                }
};

int fast_atoi( const char * str )
{
    int val = 0;
    while( *str ) {
        val = val*10 + (*str++ - '0');
    }
    return val;
}

struct print_keyname
{
    template <typename T>
    void operator()(const T & t) const {
        int status = -4;
        std::string name = abi::__cxa_demangle(typeid(t).name(), NULL, NULL, &status);

        boost::xpressive::sregex expr = boost::xpressive::sregex::compile("char.(\\d+)");
        int const subs[] = {1};
        boost::xpressive::sregex_token_iterator cur(name.begin(), name.end(), expr, subs);
        boost::xpressive::sregex_token_iterator end;

        std::cout << name.substr(30,60) << " ";

        for(;cur != end; ++cur)
            std::cout << char(fast_atoi(std::string(*cur).c_str()));
        std::cout << "\n";
    }
};

#endif //__MPL_DEBUG_TOOLS__
