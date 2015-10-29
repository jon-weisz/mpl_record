//
// Created by jweisz on 10/21/15.
//

#include <record_macros.hpp>
#include <iostream>

#define TEST_TUPLE  (int, a)
#define TEST_TUPLE_SEQUENCE ((int, a)) \
                    ((float, b)) \
                    ((char, c))



int main(int argc, char ** argv)
{
    //Records::SubRecord sr{1};
    std::cout << __cplusplus << "boost variadics" << BOOST_PP_VARIADICS <<"\n";
    std::cout << BOOST_PP_STRINGIZE( BOOST_PP_TUPLE_PUSH_BACK(TEST_TUPLE, GENERATE_KEY_TYPE_NAME_NS(test, BOOST_PP_TUPLE_ELEM(2, 0, TEST_TUPLE), BOOST_PP_TUPLE_ELEM(2, 1, TEST_TUPLE)))) <<"\n";
    std::cout << BOOST_PP_STRINGIZE(AUGMENT_TUPLE_TYPEDEF(test, TEST_TUPLE_SEQUENCE)) <<"\n";
    std::cout << BOOST_PP_STRINGIZE(GENERATE_KEY_STRUCT('_', test, TEST_TUPLE)) <<"\n";
    std::cout << BOOST_PP_STRINGIZE(ADD_TUPLE_TYPEDEF(test,TEST_TUPLE_SEQUENCE)) <<"\n";
    std::cout << BOOST_PP_STRINGIZE(  ) << "\n";
    return 0;
}
