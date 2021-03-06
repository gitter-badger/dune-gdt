// This file is part of the dune-gdt project:
//   https://github.com/dune-community/dune-gdt
// Copyright 2010-2016 dune-gdt developers and contributors. All rights reserved.
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2013 - 2016)
//   Rene Milk       (2014)
//   Tobias Leibner  (2014, 2016)

#ifndef DUNE_GDT_PROLONGATIONS_HH
#define DUNE_GDT_PROLONGATIONS_HH

#include <dune/stuff/grid/layers.hh>

#include <dune/gdt/discretefunction/default.hh>
#include <dune/gdt/spaces/interface.hh>
#include <dune/gdt/spaces/cg/interface.hh>

#include "prolongations/l2.hh"
#include "prolongations/lagrange.hh"

namespace Dune {
namespace GDT {


template <class GridViewType, class SourceSpaceType, class SourceVectorType, class RangeSpaceType,
          class RangeVectorType>
typename std::enable_if<Stuff::Grid::is_grid_layer<GridViewType>::value && is_cg_space<RangeSpaceType>::value,
                        void>::type
prolong(const GridViewType& grid_view, const ConstDiscreteFunction<SourceSpaceType, SourceVectorType>& source,
        DiscreteFunction<RangeSpaceType, RangeVectorType>& range, const size_t /*over_integrate*/ = 0)
{
  prolong_lagrange(grid_view, source, range);
}

template <class GridViewType, class SourceSpaceType, class SourceVectorType, class RangeSpaceType,
          class RangeVectorType>
typename std::enable_if<Stuff::Grid::is_grid_layer<GridViewType>::value && !is_cg_space<RangeSpaceType>::value,
                        void>::type
prolong(const GridViewType& grid_view, const ConstDiscreteFunction<SourceSpaceType, SourceVectorType>& source,
        DiscreteFunction<RangeSpaceType, RangeVectorType>& range, const size_t over_integrate = 0)
{
  prolong_l2(grid_view, source, range, over_integrate);
}


template <class SourceSpaceType, class SourceVectorType, class RangeSpaceType, class RangeVectorType>
typename std::enable_if<is_cg_space<RangeSpaceType>::value, void>::type
prolong(const ConstDiscreteFunction<SourceSpaceType, SourceVectorType>& source,
        DiscreteFunction<RangeSpaceType, RangeVectorType>& range, const size_t /*over_integrate*/ = 0)
{
  prolong_lagrange(source, range);
}

template <class SourceSpaceType, class SourceVectorType, class RangeSpaceType, class RangeVectorType>
typename std::enable_if<!is_cg_space<RangeSpaceType>::value, void>::type
prolong(const ConstDiscreteFunction<SourceSpaceType, SourceVectorType>& source,
        DiscreteFunction<RangeSpaceType, RangeVectorType>& range, const size_t over_integrate = 0)
{
  prolong_l2(source, range, over_integrate);
}


} // namespace GDT
} // namespace Dune

#endif // DUNE_GDT_PROLONGATIONS_HH
