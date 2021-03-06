#include "common.hpp"
#include "debug.hpp"
#include "graph.hpp"
#include "gtest/gtest.h"

size_t bottom_count(const std::vector<scc_component>& scc_list)
{
    size_t counter = 0;
    for (const auto& scc : scc_list)
    {
        if (scc.is_bottom)
        {
            counter += 1;
        }
    }
    return counter;
}

TEST(test_digraph, strongly_connected_components)
{
    spmatrix_creator crtor(10, 10);
    crtor.add_entry(0, 1, 1);
    crtor.add_entry(1, 0, 1);
    crtor.add_entry(1, 3, 1);
    crtor.add_entry(3, 6, 1);
    crtor.add_entry(3, 4, 1);
    crtor.add_entry(4, 5, 1);
    crtor.add_entry(4, 9, 1);
    crtor.add_entry(5, 3, 1);
    crtor.add_entry(5, 8, 1);
    crtor.add_entry(6, 7, 1);
    crtor.add_entry(7, 6, 1);
    crtor.add_entry(8, 9, 1);
    crtor.add_entry(9, 8, 1);
    const auto scc_list = strongly_connected(crtor.create(true));
    ASSERT_EQ(bottom_count(scc_list), 3);
}

struct vector_comp
{
    bool operator()(const vector& v1, const vector& v2) const
    {
        for (size_t i = 0; i < v1.dim(); i++)
        {
            if (v1[i] < v2[i])
            {
                return true;
            }
            else if (v1[i] > v2[i])
            {
                return false;
            }
        }
        return false;
    }
};

TEST(test_digraph, digraph_create_1)
{
    auto g = ordered_digraph<vector, double, vector_comp>(vector_comp());
    auto n1 = g.add_node(create_vector(2, {1.0, 1.0})).idx();
    auto n2 = g.add_node(create_vector(2, {2.0, 2.0})).idx();
    auto n3 = g.add_node(create_vector(2, {3, 3}))
                  .add_arc(n1, 1.0)
                  .add_arc(n2, 2.0)
                  .idx();
    ASSERT_EQ(g.node_count(), 3);
    ASSERT_EQ(g[n1].data(), create_vector(2, {1.0, 1.0}));
    ASSERT_EQ(g[n2].data(), create_vector(2, {2.0, 2.0}));
    ASSERT_EQ(g[n3].data(), create_vector(2, {3.0, 3.0}));
    ASSERT_EQ(g[n3].out_arc().size(), 2);
    ASSERT_EQ(g[n3].out_arc()[0].data(), 1.0);
    ASSERT_EQ(g[n3].out_arc()[1].data(), 2.0);
}

struct ptr_comp
{
    bool operator()(const std::unique_ptr<double>& p1,
                    const std::unique_ptr<double>& p2) const
    {
        return *p1 < *p2;
    }
};

TEST(test_digraph, digraph_create_2)
{
    auto g = ordered_digraph<std::unique_ptr<double>, std::unique_ptr<int>,
                             ptr_comp>(ptr_comp());
    auto n1 = g.add_node(std::make_unique<double>(1)).idx();
    auto n2 = g.add_node(std::make_unique<double>(2)).idx();
    auto n3 = g.add_node(std::make_unique<double>(3))
                  .add_arc(n1, std::make_unique<int>(1))
                  .add_arc(n2, std::make_unique<int>(2))
                  .idx();
    ASSERT_EQ(g.node_count(), 3);
    ASSERT_EQ(*(g[n1].data()), 1.0);
    ASSERT_EQ(*(g[n2].data()), 2.0);
    ASSERT_EQ(*(g[n3].data()), 3.0);
    ASSERT_EQ(g[n3].out_arc().size(), 2);
    ASSERT_EQ(*(g[n3].out_arc()[0].data()), 1);
    ASSERT_EQ(*(g[n3].out_arc()[1].data()), 2);
}

TEST(test_digraph, digraph_create_3)
{
    auto g = ordered_digraph<int, double>();
    auto n1 = g.add_node(1).idx();
    auto n2 = g.add_node(2).idx();
    auto n3 = g.add_node(3).add_arc(n1, 1.0).add_arc(n2, 2.0).idx();
    ASSERT_EQ(g.node_count(), 3);
    ASSERT_EQ(g[n1].data(), 1);
    ASSERT_EQ(g[n2].data(), 2);
    ASSERT_EQ(g[n3].data(), 3);
    ASSERT_EQ(g[n3].out_arc().size(), 2);
    ASSERT_EQ(g[n3].out_arc()[0].data(), 1.0);
    ASSERT_EQ(g[n3].out_arc()[1].data(), 2.0);
}
