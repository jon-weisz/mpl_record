#ifndef __MPL_RECORD_COPY__
#define __MPL_RECORD_COPY__

#include <boost/fusion/algorithm.hpp>
#include <boost/fusion/mpl.hpp>
#include <boost/fusion/adapted/mpl.hpp>
#include <boost/fusion/include/define_struct.hpp>
#include <boost/fusion/include/define_assoc_struct.hpp>
#include <boost/fusion/include/filter_if.hpp>
#include <boost/fusion/include/filter_view.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/has_key.hpp>
#include <boost/fusion/include/iterator_range.hpp>
#include <boost/mpl/arg.hpp>
#include <boost/mpl/has_key.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/utility/enable_if.hpp>

#include "mpl_debug_tools.hpp"



/* @brief - Template metafunction to get the key at an index of an associative struct sequence
 * FIXME - This could probably be done by using boost::fusion::applywrap1 .
 *
 * @var S - The associative struct sequence
 * @var I - The index (boost::mpl::integral_c<S, integer>)
 *
 * @returns - The type of the key at that index
 */
template <typename S>
struct key_of_index
{
    template <typename I>
    struct apply { ;
        typedef boost::fusion::extension::struct_assoc_key<S, I::value> type;
    };
};



/*@brief - Template metafunction to transform an associative struct sequence to its corresponding
 * key sequence.
 *
 *@var S - The associative struct sequence
 *
 * Returns the sequence of keys
 */
template<typename S>
struct get_keys
{
     typedef boost::mpl::range_c<int, 0, boost::fusion::result_of::size<S>::value> range;
     typedef boost::mpl::transform_view<range, key_of_index<S> > type;
};


/*@brief  - A template metafunction to determine if two associative struct sequences
 *             share a key by index into the first sequence.
 *
 * @var SourceSequence - The associative struct sequence into which the index is applied
 * @var TargetSequence - The associative struct sequence which is tested for the target key.
 * Returns boost::mpl::bool_
 */
template <class SourceSequence, class TargetSequence, class I>
struct shares_key_at_index
{
    typedef typename boost::fusion::extension::struct_assoc_key<SourceSequence, I::value> from_type;
    typedef typename boost::mpl::has_key<TargetSequence, from_type>::type has_type;
    typedef typename has_type::type type;
};


template <typename SourceSequence, typename TargetSequence>
struct copy_by_key
{
    typedef typename get_keys<SourceSequence>::type SourceSequenceKeys;
    typedef typename get_keys<TargetSequence>::type TargetSequenceKeys;
        //typedef typename boost::fusion::result_of::filter_if<SourceSequenceKeys, boost::mpl::contains<TargetSequenceKeys, boost::mpl::_1 > >::type SharedKeys;
    typedef typename boost::mpl::range_c<int, 0, boost::fusion::result_of::size<SourceSequence>::value> range;
        typedef typename get_keys<SourceSequence>::type keys;

    struct index_copier
    {
    SourceSequence &  from_;
    TargetSequence & to_;
    index_copier(SourceSequence & from, TargetSequence & to) : from_(from), to_(to){};

        template <class T, class Enable=void>
        struct key_copier{
                SourceSequence &  from_;
                TargetSequence & to_;
                typedef typename boost::mpl::has_key<TargetSequence, T> has_key_t ;
                key_copier(SourceSequence & from, TargetSequence & to) : from_(from), to_(to){}
                inline void operator() (const T & t) const
                        {
                                has_key_t tt;
                                print_info()(t);
                        }

        };

        template<class T>
        struct key_copier<T, typename boost::enable_if<typename boost::mpl::has_key<TargetSequence, T>, int>::type>
        {
            SourceSequence &  from_;
            TargetSequence & to_;
            key_copier(SourceSequence & from, TargetSequence & to) : from_(from), to_(to){};
            inline void operator() (const T & t) const
            {
              boost::fusion::at_key<TargetSequence, T>(to_) = boost::fusion::at_key<T>(from_);
            }
        };


        template< class I>
        inline void operator() (const I & t) const
        {
            typename boost::fusion::extension::struct_assoc_key<SourceSequence, I::value>::type key;
            key_copier<typename boost::fusion::extension::struct_assoc_key<SourceSequence, I::value>::type> copier_(from_, to_);
            copier_(key);
        }

    };

    void operator() (SourceSequence & from, TargetSequence & to)
    {
            index_copier copier_(from, to);
            boost::fusion::for_each(range(), copier_);
    }
};

#endif
