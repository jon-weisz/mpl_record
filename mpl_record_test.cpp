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


int test_copy()
{
    boost::fusion::Record::MasterRecord mr{5 , "test", 3.0};
    boost::fusion::Record::SubRecord sr{ 2 , "foo"};
    copy_by_key<boost::fusion::Record::SubRecord, boost::fusion::Record::MasterRecord> copier;
    copier(sr, mr);
    boost::fusion::for_each(mr, print_info());
}



int test_shared_keys()
{
    boost::fusion::Record::MasterRecord mr{5 , "test", 3.0};
    boost::fusion::Record::SubRecord sr{ 2 , "foo"};

    typedef typename get_keys<boost::fusion::Record::MasterRecord>::type SourceSequenceKeys;
    typedef typename get_keys<boost::fusion::Record::SubRecord>::type TargetSequenceKeys;
    typedef typename boost::fusion::result_of::filter_if<SourceSequenceKeys,boost::mpl::contains<TargetSequenceKeys, boost::mpl::_1 > >::type SharedKeys;
    typedef typename boost::fusion::result_of::transform<SourceSequenceKeys, boost::mpl::contains<SharedKeys, boost::mpl::_1 > >::type contained_keys;
    typedef boost::mpl::range_c<int, 0, boost::fusion::result_of::size<SourceSequenceKeys>::value>::type range;
    copier_if_pattern<contained_keys, boost::fusion::Record::MasterRecord, boost::fusion::Record::SubRecord> copier_(mr, sr);
    //boost::fusion::for_each(range(), copier_);
}

int main(int argc, char ** argv)
{
    test_copy();
    return 0;
}
