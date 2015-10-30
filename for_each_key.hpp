#include <functional>

#include <boost/fusion/include/adapt_struct.hpp>

#include <boost/fusion/sequence/intrinsic/begin.hpp>
#include <boost/fusion/sequence/intrinsic/end.hpp>
#include <boost/fusion/sequence/intrinsic/front.hpp>
#include <boost/fusion/iterator/equal_to.hpp>
#include <boost/fusion/iterator/next.hpp>
#include <boost/fusion/iterator/deref.hpp>
#include <boost/fusion/iterator/distance.hpp>
#include <boost/fusion/support/category_of.hpp>
#include <boost/mpl/bool.hpp>

namespace boost { namespace fusion {

        namespace detail {

            template <typename First, typename Last, typename F>
            inline void
            for_each_key_linear(First const& first,
                                   Last const& last,
                                   F const& f,
                                   boost::mpl::true_) {}

            template <typename First, typename Last, typename F>
            inline void
            for_each_key_linear(First const& first,
                                   Last const& last,
                                   F const& f,
                                   boost::mpl::false_) {
                typename boost::fusion::extension::struct_assoc_key<typename First::seq_type, First::index::value>::type key;
                    f(key,
                      *first
                );

                for_each_key_linear(
                        next(first),
                        last,
                        f,
                        result_of::equal_to< typename result_of::next<First>::type, Last>()
                );
            }

            template <typename Sequence, typename F>
            inline void
            for_each_key(Sequence& seq, F const& f) {

                detail::for_each_key_linear(
                        fusion::begin(seq),
                        fusion::end(seq),
                        f,
                        result_of::equal_to<
                                typename result_of::begin<Sequence>::type,
                                typename result_of::end<Sequence>::type>()
                );
            }

        }

        template <typename Sequence, typename F>
        inline void
        for_each_key(Sequence& seq, F f) {
            detail::for_each_key(seq, f);
        }

    }}
