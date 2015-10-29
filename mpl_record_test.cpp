#include <string>
#include <iostream>
#include "record_macros.hpp"
#include "mpl_record_copy.hpp"

#define MASTER_RECORD_MEMBERS ((int, a)) ((std::string, b)) ((float, c))
#define SUBRECORD_MEMBERS ((int, a)) ((std::string, b))

DEFINE_ASSOC_STRUCT(Record, MasterRecord, MASTER_RECORD_MEMBERS)
DEFINE_ASSOC_STRUCT(Record, SubRecord, SUBRECORD_MEMBERS)


int main(int argc, char ** argv)
{
    boost::fusion::Record::MasterRecord mr{5 , "test", 3.0};
    boost::fusion::Record::SubRecord sr{ 2 , "foo"};
    copy_by_key<boost::fusion::Record::MasterRecord, boost::fusion::Record::SubRecord> copier;
    copier(mr, sr);

    return 0;
}
