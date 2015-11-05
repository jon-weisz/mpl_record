//
// Created by jweisz on 10/21/15.
//

#ifndef MPL_TEST_RECORD_MACROS_HPP
#define MPL_TEST_RECORD_MACROS_HPP
#include <boost/preprocessor/seq.hpp>
#include <boost/preprocessor/tuple/to_seq.hpp>
#include <boost/preprocessor/tuple.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/tuple/size.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/facilities/expand.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/fusion/include/define_assoc_struct.hpp>
#include <boost/fusion/adapted/struct/detail/namespace.hpp>


#include <boost/preprocessor/repetition/repeat.hpp>
#define GET_STR_AUX(_, i, str) (sizeof(str) > (i) ? str[(i)] : 0),
#define GET_STR(str) BOOST_PP_REPEAT(64,GET_STR_AUX,str) 0

template <class t, char... Name> struct key_type
{
        typedef t type;
        // template<char first, char...Rest>
        // static std::string key_name_impl(){
        //         return std::string(first) + key_name_impl<Rest...>();
        // }

        // template<char last>
        // static std::string key_name_impl(){
        //         return std::string(last);
        // }
        // static std::string key_name(){
        //         return key_name_impl<Name...>();
        // }
};


#define SUBRECORD_MEMBERS_FAIL_NAME                     \
        (int, d)

#define SUBRECORD_MEMBERS_FAIL_TYPE                     \
        (std::string, a)

#define GENERATE_KEY_TYPE_NAME(key_type, key_name) BOOST_PP_CAT(keytype, key_name)
#define GENERATE_KEY_TYPE_NAME_NS(ns, key_type, key_name) ns::BOOST_PP_CAT(keytype, key_name)
#define GENERATE_TUPLE_KEY_NAME_NS(ns, tuple) GENERATE_KEY_TYPE_NAME_NS(ns, BOOST_PP_TUPLE_ELEM(2, 0, tuple), BOOST_PP_TUPLE_ELEM(2, 1, tuple))
#define GENERATE_TUPLE_KEY_NAME(tuple) GENERATE_KEY_TYPE_NAME( BOOST_PP_TUPLE_ELEM(2, 0, tuple), BOOST_PP_TUPLE_ELEM(2, 1, tuple))

//#define GENERATE_KEY_STRUCT(r, ns, key_tuple)  \
//namespace ns{\
//  struct GENERATE_TUPLE_KEY_NAME(key_tuple) \
//        { \
//    typedef BOOST_PP_TUPLE_ELEM(0, key_tuple ) type; \
//  };\
//};

#define GENERATE_KEY_STRUCT(r, ns, key_tuple)\
namespace boost{\
  namespace fusion{\
   namespace ns{\
           typedef key_type< BOOST_PP_TUPLE_ELEM(2, 0, key_tuple), GET_STR(BOOST_PP_STRINGIZE(GENERATE_TUPLE_KEY_NAME(key_tuple)))> GENERATE_TUPLE_KEY_NAME(key_tuple); \
    };\
  };\
};\


//BOOST_FUSION_ADAPT_STRUCT_NAMESPACE_DECLARATION((boost) (fusion) (ns)) \
//BOOST_FUSION_ADAPT_STRUCT_NAMESPACE_DEFINITION_BEGIN((boost) (fusion) (ns)) \
//BOOST_FUSION_ADAPT_STRUCT_NAMESPACE_DEFINITION((boost) (fusion) (ns))




#ifndef BOOST_PP_TUPLE_PUSH_BACK
#define BOOST_PP_TUPLE_PUSH_BACK(tuple, item) (BOOST_PP_TUPLE_REM_CTOR(2, tuple), item)
#endif



#define ADD_TUPLE_TYPE(r, ns, tuple) BOOST_PP_TUPLE_PUSH_BACK(tuple, GENERATE_TUPLE_KEY_NAME_NS(ns, tuple))

#define ADD_TUPLE_TYPEDEF(ns, seq) BOOST_PP_SEQ_FOR_EACH(GENERATE_KEY_STRUCT, ns, seq)

#define AUGMENT_TUPLE_TYPEDEF(ns, seq) BOOST_PP_SEQ_FOR_EACH(ADD_TUPLE_TYPE, ns, seq)


#define DEFINE_ASSOC_STRUCT(ns, classname, member_seq) \
ADD_TUPLE_TYPEDEF(ns, member_seq) \
 BOOST_FUSION_DEFINE_ASSOC_STRUCT \
 ( (boost) (fusion) (ns),      \
  classname, \
  AUGMENT_TUPLE_TYPEDEF(ns, member_seq) \
 )

#endif //MPL_TEST_RECORD_MACROS_HPP
