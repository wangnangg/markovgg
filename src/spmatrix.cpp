#include "spmatrix.hpp"
#include <algorithm>
#include <utility>
#include "debug_utils.hpp"
namespace markovgg
{
spmatrix_mutable_view::spmatrix_mutable_view(spmatrix& mat)
    : spmatrix_base(mat.m(), mat.n(), mat.is_compressed_row()),
      _ptr(&mat._ptr[0]),
      _data(&mat._data[0])
{
}

spmatrix_const_view::spmatrix_const_view(const spmatrix& mat)
    : spmatrix_base(mat.m(), mat.n(), mat.is_compressed_row()),
      _ptr(&mat._ptr[0]),
      _data(&mat._data[0])
{
}

bool cmp_spsmat_cs_entry(const spmat_cs_entry& e1, const spmat_cs_entry& e2)
{
    return e1.idx < e2.idx;
}
const spmat_cs_entry& bin_search(const spmat_cs_entry* first,
                                 const spmat_cs_entry* last,
                                 const spmat_cs_entry& value,
                                 bool (*cmp)(const spmat_cs_entry&,
                                             const spmat_cs_entry&))
{
    first = std::lower_bound(first, last, value, cmp);
    if (first != last && !cmp(value, *first))
    {
        return *first;
    }
    else
    {
        return value;
    }
}

real_t spmatrix_get_entry(spmatrix_const_view mat, size_t row, size_t col)
{
    if (mat.is_compressed_row())
    {
        auto target = spmat_cs_entry{col, 0.0};
        auto view = mat[row];
        return bin_search(view.begin(), view.end(), target, cmp_spsmat_cs_entry)
            .val;
    }
    else
    {
        auto target = spmat_cs_entry{row, 0.0};
        auto view = mat[col];
        return bin_search(view.begin(), view.end(), target, cmp_spsmat_cs_entry)
            .val;
    }
}

real_t spmatrix::operator()(size_t row, size_t col) const
{
    return spmatrix_get_entry(*this, row, col);
}
real_t spmatrix_mutable_view::operator()(size_t row, size_t col) const
{
    return spmatrix_get_entry(*this, row, col);
}
real_t spmatrix_const_view::operator()(size_t row, size_t col) const
{
    return spmatrix_get_entry(*this, row, col);
}

static bool cmp_CPR_ROW(const spmat_triplet_entry& e1,
                        const spmat_triplet_entry& e2)
{
    if (e1.row < e2.row)
    {
        return true;
    }
    else if (e1.row > e2.row)
    {
        return false;
    }
    else if (e1.col < e2.col)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static bool cmp_CPR_COL(const spmat_triplet_entry& e1,
                        const spmat_triplet_entry& e2)
{
    if (e1.col < e2.col)
    {
        return true;
    }
    else if (e1.col > e2.col)
    {
        return false;
    }
    else if (e1.row < e2.row)
    {
        return true;
    }
    else
    {
        return false;
    }
}

spmatrix spmatrix_creator::create(bool is_row_compressed)
{
    if (is_row_compressed)
    {
        std::sort(_data.begin(), _data.end(), cmp_CPR_ROW);
        std::vector<size_t> idx;
        idx.reserve(_m + 1);
        std::vector<spmat_cs_entry> val;
        val.reserve(_data.size());
        idx.push_back(0);
        auto itor = _data.begin();
        for (size_t i = 0; i < _m; i++)
        {
            size_t counter = 0;
            while (itor != _data.end() && itor->row == i)
            {
                spmat_cs_entry entry{itor->col, itor->val};
                itor += 1;
                while (itor != _data.end() && itor->row == i &&
                       itor->col == entry.idx)
                {
                    entry.val += itor->val;
                    itor += 1;
                }
                if (entry.val != 0.0)
                {
                    val.push_back(entry);
                    counter += 1;
                }
            }
            idx.push_back(counter + idx[i]);
        }
        assert(_m + 1 >= idx.size());
        return spmatrix(_m, _n, is_row_compressed, std::move(idx),
                        std::move(val));
    }
    else
    {
        std::sort(_data.begin(), _data.end(), cmp_CPR_COL);
        std::vector<size_t> idx;
        idx.reserve(_n + 1);
        std::vector<spmat_cs_entry> val;
        val.reserve(_data.size());
        idx.push_back(0);
        auto itor = _data.begin();
        for (size_t i = 0; i < _n; i++)
        {
            size_t counter = 0;
            while (itor != _data.end() && itor->col == i)
            {
                spmat_cs_entry entry{itor->row, itor->val};
                itor += 1;
                while (itor != _data.end() && itor->col == i &&
                       itor->row == entry.idx)
                {
                    entry.val += itor->val;
                    itor += 1;
                }
                if (entry.val != 0.0)
                {
                    val.push_back(entry);
                    counter += 1;
                }
            }
            idx.push_back(counter + idx[i]);
        }
        assert(_n + 1 >= idx.size());
        return spmatrix(_m, _n, is_row_compressed, std::move(idx),
                        std::move(val));
    }
}
}
