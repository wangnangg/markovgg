#include <cassert>
#include <sstream>
#include "blas.hpp"
#include "linalg.hpp"
#include "matvec_oper.hpp"
#include "spblas.hpp"
#include "splinalg.hpp"
#include "spmatrix_oper.hpp"
#include "vector.hpp"

namespace markovgg
{
real_t max_diff(vector_const_view v1, vector_const_view v2)
{
    assert(v1.dim() == v2.dim());
    real_t max = 0;
    for (size_t i = 0; i < v1.dim(); i++)
    {
        real_t diff = abs(v1[i] - v2[i]);
        if (diff > max)
        {
            max = diff;
        }
    }
    return max;
}

real_t eigen_power_method(spmatrix_const_view A, vector_mutable_view x,
                          real_t tol, uint_t max_iter, uint_t check_interval)
{
    set_norm1(x, 1.0);
    vector x_next_(x.dim(), 0.0);
    vector_mutable_view x_next(x_next_);
    real_t prec = 0.0;
    for (uint_t ii = 0; ii < max_iter / check_interval; ii++)
    {
        for (uint_t jj = 0; jj < check_interval; jj++)
        {
            dot(x_next, A, x);
            set_norm1(x_next, 1.0);
            std::swap(x_next, x);
        }
        prec = max_diff(x_next, x);
        if (prec < tol)
        {
            return prec;
        }
    }
    return prec;
}
}