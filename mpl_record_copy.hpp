#ifndef __MPL_RECORD_COPY__
#define __MPL_RECORD_COPY__

#include <boost/fusion/algorithm.hpp>
#include <boost/fusion/mpl.hpp>
#include <boost/fusion/adapted/mpl.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/define_struct.hpp>
#include <boost/fusion/include/define_assoc_struct.hpp>
#include <boost/fusion/include/filter_if.hpp>
#include <boost/fusion/include/filter_view.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/has_key.hpp>
#include <boost/fusion/include/iterator_range.hpp>
#include <boost/mpl/arg.hpp>
#include <boost/mpl/contains.hpp>
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
    typedef typename boost::mpl::range_c<int, 0, boost::fusion::result_of::size<SourceSequence>::value> range;
    struct index_copier
    {
    SourceSequence &  from_;
    TargetSequence & to_;
    index_copier(SourceSequence & from, TargetSequence & to) : from_(from), to_(to){};

        struct key_copier
        {
           SourceSequence &  from_;
           TargetSequence & to_;
           key_copier(SourceSequence & from, TargetSequence & to) : from_(from), to_(to){}

                template<class T, typename boost::enable_if<typename boost::mpl::has_key<TargetSequence, T>::type,int>::type = 0>
                void operator() (const T &t )const
                {
                        boost::fusion::at_key<T>(to_) = boost::fusion::at_key<T>(from_);
                }
                template<class T, typename boost::enable_if<typename boost::mpl::not_<typename boost::mpl::has_key<TargetSequence, T>::type>::type,int>::type = 0>
                void operator() (const T &t )const{}
        };

        template< class I>
        inline void operator() (const I & t) const
        {
            typename boost::fusion::extension::struct_assoc_key<SourceSequence, I::value>::type key;
            key_copier copier_(from_, to_);
            copier_(key);
        }

    };

    inline void operator() (SourceSequence & from, TargetSequence & to)
    {
            index_copier copier_(from, to);
            boost::fusion::for_each(range(), copier_);
    }
};






/* @brief - Second attempt to enable copies.
 *
 *
 */
template <class contained_keys, class SourceSequence, class TargetSequence>
struct copier_if_pattern
{

    SourceSequence & sk_;
    TargetSequence & tk_;
    copier_if_pattern(SourceSequence sk, TargetSequence tk) : sk_(sk), tk_(tk){}

    template< class key>
    struct actual_copier
    {
        void operator()(SourceSequence & sk, TargetSequence & tk)
            {
                boost::fusion::at_key<TargetSequence, key>(tk) = boost::fusion::at_key<SourceSequence, key>(sk);
            }
    };


    template<class key>
    struct do_nothing
    {
        void operator()(SourceSequence & sk, TargetSequence tk)
       {
       }
    };

    template <class I>
    void operator()(const I & i ) const
    {
        typedef typename boost::fusion::extension::struct_assoc_key<SourceSequence, I::value>::type key;
        typedef typename boost::mpl::at<contained_keys, I>::type has_key;
        typedef actual_copier<key> actual_copier_;
        typedef do_nothing<key> do_nothing_;

        typedef typename boost::mpl::if_<
            has_key, actual_copier_, do_nothing_>::type copy_impl;
        copy_impl(sk_, tk_);
    }
};


#endif
