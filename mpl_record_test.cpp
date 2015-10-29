#include <string>
#include <iostream>
#include "record_macros.hpp"
#include "mpl_record_copy.hpp"
#include <boost/fusion/include/filter_if.hpp>
#include <boost/fusion/include/transform_view.hpp>
#include <boost/fusion/include/algorithm.hpp>
#include <boost/mpl/at.hpp>


#define MASTER_RECORD_MEMBERS ((int, a)) ((std::string, b)) ((float, c))
#define SUBRECORD_MEMBERS ((int, a)) ((std::string, b))

DEFINE_ASSOC_STRUCT(Record, MasterRecord, MASTER_RECORD_MEMBERS)
DEFINE_ASSOC_STRUCT(Record, SubRecord, SUBRECORD_MEMBERS)

int test_hand_copy()
{
    boost::fusion::Record::MasterRecord mr{5 , "test", 3.0};
    boost::fusion::Record::SubRecord sr{ 2 , "foo"};
    sr.a = mr.a;
    sr.b = mr.b;
}

int test_copy()
{
    boost::fusion::Record::MasterRecord mr{5 , "test", 3.0};
    boost::fusion::Record::SubRecord sr{ 2 , "foo"};
    copy_by_key<boost::fusion::Record::SubRecord, boost::fusion::Record::MasterRecord> copier;
    copier(sr, mr);
    boost::fusion::for_each(mr, print_info());
}



int main(int argc, char ** argv)
{
    test_copy();
    return 0;
}
