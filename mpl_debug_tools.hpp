#ifndef __MPL_DEBUG_TOOLS__
#define __MPL_DEBUG_TOOLS__
#include <iostream>
#include <cxxabi.h>
#include <string>
#include <regex>
#include <boost/xpressive/xpressive.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/is_sequence.hpp>
#include <boost/mpl/print.hpp>
#include <boost/mpl/not.hpp>
#include <boost/utility/enable_if.hpp>

class indent_guard {
        public:
        indent_guard(int level, std::ostream& stream, int index)
        : level(level),
        stream(stream),
        index(index)
        {
            stream.iword(index) += level;
        }

        ~indent_guard() {
            stream.iword(index) -= level;
        }

        private:
        int level;
        std::ostream& stream;
        int index;
};

struct indent {
    indent(int index_ = 0) : index(index_) {}
private:
    friend std::ostream& operator<<(std::ostream& stream, const indent& val);

    int index;
};

std::ostream& operator<<(std::ostream& stream, const indent& val) {
    for(int i = 0; i < stream.iword(val.index); i++) {
        stream << " ";
    }
    return stream;
}


struct print{
    template <typename T>
    void operator()(const T &t) const
    {
      std::cout << indent() << std::boolalpha << t << '\n';
    }
};

struct print_name
{

        template <class T, typename boost::enable_if<typename boost::is_same<typename T::tag, key_type_tag>::type, int>::type = 0>
        void operator()(const T & t)
        {
            std::cout <<indent() << "key name:" <<t.key_name() << std::endl;
        }

        template <typename T>
        void operator()(const T &t) const
                {
                        int status = -4;
                        std::cout << indent() << abi::__cxa_demangle(typeid(t).name(), NULL, NULL, &status) << " " <<std::endl;
                }
};



struct print_info
{
     template <typename T>
     void operator()(const T &t) const
     {
         int status = -4;
         std::cout << indent() << abi::__cxa_demangle(typeid(t).name(), NULL, NULL, &status) << " " << t <<std::endl;
     }
};

template <class T>
struct non_struct_sequence {
    //typedef typename boost::mpl::and_<boost::mpl::is_sequence<T>, boost::is_same<typename boost::fusion::traits::tag_of<T>::type, boost::fusion::assoc_struct_tag>>::type type;
    typedef typename boost::mpl::and_<typename boost::mpl::is_sequence<T>::type,  typename boost::mpl::not_<typename boost::is_same<typename boost::fusion::traits::tag_of<T>::type, boost::fusion::assoc_struct_tag>::type>::type>::type type;
};

template<class Printer, unsigned Level = 0>
struct print_nested_for_each {
    template<class T, typename boost::enable_if<typename non_struct_sequence<T>::type , int>::type=0>
    void operator()(const T &t) const{
        std::cout << indent() << "Embedded Level: " << Level << std::endl;
        indent_guard ig(2, std::cout, 0);
        print_nested_for_each<Printer, Level + 1> printer;
        boost::fusion::for_each(t, printer);
    }
    template<class T, typename boost::enable_if<typename  boost::mpl::not_<typename non_struct_sequence<T>::type>::type, int>::type=0>
    void operator()(const T &t) const{
        Printer()(t);
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
