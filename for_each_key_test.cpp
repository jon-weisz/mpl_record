#include <string>
#include <iostream>
#include "record_macros.hpp"
#include "mpl_record_copy.hpp"
#include <boost/fusion/include/filter_if.hpp>
#include <boost/fusion/include/transform_view.hpp>
#include <boost/fusion/include/algorithm.hpp>
#include <boost/mpl/at.hpp>
#include "for_each_key.hpp"

#define MASTER_RECORD_MEMBERS ((int, a)) ((std::string, b)) ((float, c))
#define SUBRECORD_MEMBERS ((int, a)) ((std::string, b))

DEFINE_ASSOC_STRUCT(Record, MasterRecord, MASTER_RECORD_MEMBERS)
DEFINE_ASSOC_STRUCT(Record, SubRecord, SUBRECORD_MEMBERS)

template<class S>
struct print_has_key
{
    S & test_struct_;

    print_has_key(S & test_struct) : test_struct_(test_struct) {}
    template <class T, class U>
    void operator () (const T & t, const U & u) const
        {
            std::cout << "has_key?" << boost::fusion::has_key<T>(test_struct_) <<  " value:" << u << "\n";
        }
};

int test_for_each_key(){
    boost::fusion::Record::MasterRecord mr{5 , "test", 3.0};
    boost::fusion::Record::SubRecord sr{ 2 , "foo"};
    boost::fusion::for_each_key(mr, print_has_key<boost::fusion::Record::SubRecord>(sr));
}



int main(int argc, char ** argv)
{
    test_for_each_key();
    return 0;
}
