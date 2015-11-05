#include <string>
#include <iostream>
#include "record_macros.hpp"
#include "mpl_record_copy.hpp"
#include <boost/fusion/include/filter_if.hpp>
#include <boost/fusion/include/deduce.hpp>
#include <boost/fusion/include/transform_view.hpp>
#include <boost/fusion/include/algorithm.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl_graph/mpl_graph.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl_graph/incidence_list_graph.hpp>
#include <boost/mpl_graph/breadth_first_search.hpp>
#include <boost/mpl_graph/depth_first_search.hpp>
#include <boost/mpl/assert.hpp>

#define MASTER_RECORD_MEMBERS ((int, a)) ((std::string, b)) ((float, c)) ((std::string, d))
#define SUBRECORD_MEMBERS ((int, a)) ((std::string, b)) ((float, e))
DEFINE_ASSOC_STRUCT(Record, MasterRecord, MASTER_RECORD_MEMBERS)
DEFINE_ASSOC_STRUCT(Record, SubRecord, SUBRECORD_MEMBERS)


/*Given a set of processors, create the graph of all Dependencies
 */

struct edge_tag {};
struct half_edge_tag : edge_tag {};
struct full_edge_tag : edge_tag {};


struct vertex_tag {};
struct state_variable_vertex_tag : vertex_tag {};
struct controller_vertex_tag : vertex_tag {};


template <class vertex_type_tag>
struct graph_vertex
{
        typedef vertex_type_tag vertex_tag ;
};


template <class V>
struct vertex_tag_of {
        typedef typename V::vertex_tag type;
};




template <class Source, class Dest, class DataType>
struct half_edge
{
   typedef half_edge_tag tag;
   typedef Source source;
   typedef Dest dest;
   typedef DataType type;
};

template<class Controller, class DataType>
struct output_edge {
        typedef half_edge<Controller, typename DataType::type, DataType> type;
};

template<class Controller, class DataType>
struct input_edge
{
        typedef half_edge<typename boost::fusion::traits::deduce<DataType>::type::type, Controller, DataType> type;
};


template <class edge_maker>
struct make_est
{
        template<class DataType>
        struct apply {
                typedef typename boost::mpl::apply<edge_maker, DataType>::type edge_type;
          typedef typename boost::mpl::vector3<edge_type,
                                             typename edge_type::source,
                                               typename edge_type::dest> type;
        };
};

template <class E>
struct add_edge_to_sequence
{
        template <class S, class D>
                struct apply{
                typedef typename boost::mpl::push_back<S, typename boost::mpl::apply<E, D>::type>::type type;
        };
};

template <class edge_maker, class OutSeq = boost::mpl::vector<>>
struct make_edges
{
        template<class Seq>
        struct apply {
                typedef make_est<edge_maker> est_maker;
                typedef add_edge_to_sequence<est_maker> adder;
                typedef typename boost::mpl::fold<typename get_keys<Seq>::type, OutSeq, adder>::type type;

        };
};

template<class Controller>
struct in_edge_maker {
        template <class DataType>
        struct apply {
                typedef typename input_edge<Controller, DataType>::type type;
        };
};

template<class Controller>
struct out_edge_maker {
        template <class DataType>
        struct apply {
                typedef typename output_edge<Controller, DataType>::type type;
        };
};


struct preordering_visitor : boost::msm::mpl_graph::bfs_default_visitor_operations {
        template<typename Vertex, typename Graph, typename State>
        struct discover_vertex :
                boost::mpl::push_back<State, Vertex>
        {};
};

template <class find_target>
struct preordering_find_state_visitor : boost::msm::mpl_graph::dfs_default_visitor_operations {
    template<class Vertex, class Graph, class State>
    struct discover_vertex : boost::mpl::if_<boost::mpl::contains<State, find_target>,
            State,
            boost::mpl::push_back<State, Vertex>>::type {};


    template <class Vertex, class Graph, class State>
    struct finish_vertex : boost::mpl::if_<boost::mpl::contains<State, find_target>,
                State,
                boost::mpl::vector<>>::type {};

};

struct active_path{};
struct finished_path_list{};


template <class Map, class Key, class Val>
struct replace_key{
    typedef typename boost::mpl::insert<typename boost::mpl::erase_key<Map, Key>::type, boost::mpl::pair<Key, Val>>::type type;

    BOOST_MPL_ASSERT(( boost::is_same<boost::mpl::map<>::tag, typename type::tag> ));
};

template <class Map, class Key, class Val>
struct append_to_key {
    typedef typename replace_key<Map, Key, typename boost::mpl::push_back<typename boost::mpl::at<Map, Key>::type, Val>::type>::type type;
    BOOST_MPL_ASSERT(( boost::is_same<boost::mpl::map<>::tag, typename type::tag> ));
};

template <class Map, class Key>
struct pop_from_key {
    typedef typename replace_key<Map, Key, typename boost::mpl::pop_back<typename boost::mpl::at<Map, Key>::type>::type>::type type;
    BOOST_MPL_ASSERT(( boost::is_same<boost::mpl::map<>::tag, typename type::tag> ));
};

template <class find_target>
struct preordering_find_state_visitor_all_paths : boost::msm::mpl_graph::dfs_default_visitor_operations {

    template<class Vertex, class State>
    struct add_vertex_to_active : boost::mpl::push_back<typename boost::mpl::at<State, active_path>::type,
           Vertex>
    {
        BOOST_MPL_ASSERT(( boost::is_same<boost::mpl::map<>::tag, typename State::tag> ));
    };

    template<class Edge, class Graph, class State>
    struct tree_edge {
        BOOST_MPL_ASSERT(( boost::is_same<boost::mpl::map<>::tag, typename State::tag> ));

        typedef typename boost::msm::mpl_graph::target<Edge, Graph>::type Vertex;
        typedef typename add_vertex_to_active<Vertex, State>::type extended_active;
        typedef typename boost::mpl::if_<boost::is_same<Vertex , find_target>,
            typename append_to_key<State, finished_path_list, extended_active>::type,
           State
    >::type type;

    };


    template<typename Edge, typename Graph, typename State>
    struct forward_or_cross_edge : tree_edge<Edge, Graph, State> {
        BOOST_MPL_ASSERT(( boost::is_same<boost::mpl::map<>::tag, typename State::tag> ));
    };

    template<typename Vertex, typename Graph, typename State>
    struct discover_vertex {
        typedef typename append_to_key<State, active_path, Vertex>::type type;
        BOOST_MPL_ASSERT(( boost::is_same<boost::mpl::map<>::tag, typename State::tag> ));
    };

    template<typename Vertex, typename Graph, typename State>
    struct finish_vertex {
        typedef typename pop_from_key<State, active_path>::type type;
        BOOST_MPL_ASSERT(( boost::is_same<boost::mpl::map<>::tag, typename State::tag> ));
    };
};

int main()
{
        boost::fusion::Record::MasterRecord mr{5 , "test", 3.0, "test2"};
        boost::fusion::Record::SubRecord sr{ 2 , "foo", 5.0};
        typedef in_edge_maker<boost::fusion::Record::SubRecord> build_in_edges;
        typedef out_edge_maker<boost::fusion::Record::MasterRecord> build_out_edges;
        typedef boost::mpl::apply<make_edges<build_in_edges>, boost::fusion::Record::SubRecord>::type input_edges;
        typedef boost::mpl::apply<make_edges<build_out_edges, input_edges>, boost::fusion::Record::MasterRecord>::type output_edges;

        typedef boost::msm::mpl_graph::incidence_list_graph<input_edges> input_edge_graph;
        typedef boost::msm::mpl_graph::incidence_list_graph<output_edges> output_edge_graph;

        typedef boost::mpl::map<boost::mpl::pair<active_path, boost::mpl::vector<>> ,
                boost::mpl::pair<finished_path_list,boost::mpl::vector<>>> initial_map;
        typedef boost::mpl::first<typename boost::msm::mpl_graph::
                depth_first_search<output_edge_graph,
                preordering_find_state_visitor_all_paths<boost::fusion::Record::SubRecord>,
                                     initial_map,
                                     boost::fusion::Record::MasterRecord>::type>::type preorder_inc;


       //typedef typename append_to_key<initial_map, finished_path_list, int>::type test_add_int;
       //print_name()(test_add_int());
        //typedef typename pop_from_key<test_add_int, finished_path_list>::type test_remove_int;
        //print_name()(boost::mpl::size<boost::mpl::at<test_add_int,finished_path_list>::type>::type());
        std::cout << "num_input_edges: " << boost::msm::mpl_graph::num_edges<input_edge_graph>::value << "\n" ;
        std::cout << "num_output_edges: " << boost::msm::mpl_graph::num_edges<output_edge_graph>::value << "\n" ;
        boost::fusion::for_each(boost::mpl::at<preorder_inc,finished_path_list>::type(),print_name_for_each()) ;

        //boost::fusion::for_each(typename output_edge_graph::data(), print_keyname());

        return 0;
}
