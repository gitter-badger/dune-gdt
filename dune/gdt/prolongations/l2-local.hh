// This file is part of the dune-gdt project:
//   https://github.com/dune-community/dune-gdt
// Copyright 2010-2016 dune-gdt developers and contributors. All rights reserved.
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2015 - 2016)

#ifndef DUNE_GDT_PROLONGATIONS_L2_LOCAL_HH
#define DUNE_GDT_PROLONGATIONS_L2_LOCAL_HH

#include <dune/stuff/common/memory.hh>

#include <dune/gdt/discretefunction/reinterpret.hh>
#include <dune/gdt/exceptions.hh>
#include <dune/gdt/operators/base.hh>
#include <dune/gdt/projections/l2-local.hh>

namespace Dune {
namespace GDT {


// forward
template <class GridViewImp, class FieldImp = double>
class L2LocalProlongationOperator;


namespace internal {


template <class GridViewImp, class FieldImp>
class L2LocalProlongationOperatorTraits
{
public:
  typedef L2LocalProlongationOperator<GridViewImp, FieldImp> derived_type;
  typedef FieldImp FieldType;
};


} // namespace internal


/**
 * \brief Carries out a prolongation (in a localized manner) using a local L2 projection.
 *
 *        This is done by reinterpreting the source on the range grid view and applying a
 *        L2LocalProjectionLocalizableOperator.
 */
template <class GridViewImp, class SourceImp, class RangeImp>
class L2LocalProlongationLocalizableOperator
    : Stuff::Common::ConstStorageProvider<ReinterpretDiscreteFunction<SourceImp>>,
      public L2LocalProjectionLocalizableOperator<GridViewImp, ReinterpretDiscreteFunction<SourceImp>, RangeImp>
{
  static_assert(is_const_discrete_function<SourceImp>::value, "");
  static_assert(is_discrete_function<RangeImp>::value, "");
  typedef Stuff::Common::ConstStorageProvider<ReinterpretDiscreteFunction<SourceImp>> SourceStorage;
  typedef L2LocalProjectionLocalizableOperator<GridViewImp, ReinterpretDiscreteFunction<SourceImp>, RangeImp>
      BaseOperatorType;

public:
  typedef SourceImp SourceType;
  using typename BaseOperatorType::GridViewType;
  using typename BaseOperatorType::RangeType;

  L2LocalProlongationLocalizableOperator(const size_t over_integrate, GridViewType grd_vw, const SourceType& src,
                                         RangeType& rng)
    : SourceStorage(new ReinterpretDiscreteFunction<SourceImp>(src))
    , BaseOperatorType(over_integrate, grd_vw, SourceStorage::access(), rng)
  {
  }

  L2LocalProlongationLocalizableOperator(GridViewType grd_vw, const SourceType& src, RangeType& rng)
    : L2LocalProlongationLocalizableOperator(0, grd_vw, src, rng)
  {
  }

  ///! Calls L2LocalProjectionLocalizableOperator::apply and gives a meaningful error message.
  void apply()
  {
    try {
      BaseOperatorType::apply();
    } catch (Stuff::Exceptions::reinterpretation_error& ee) {
      DUNE_THROW(prolongation_error,
                 "This prolongation (using a local L2 projection) failed, because the source could not be reinterpreted"
                     << " on the given grid view!\n"
                     << "This was the original error:\n\n"
                     << ee.what());
    }
  } // ... apply(...)
}; // class L2LocalProlongationLocalizableOperator


template <class GridViewType, class SourceSpaceType, class SourceVectorType, class RangeSpaceType,
          class RangeVectorType>
typename std::enable_if<Stuff::Grid::is_grid_layer<GridViewType>::value && is_space<SourceSpaceType>::value
                            && Stuff::LA::is_vector<SourceVectorType>::value && is_space<RangeSpaceType>::value
                            && Stuff::LA::is_vector<RangeVectorType>::value,
                        std::unique_ptr<L2LocalProlongationLocalizableOperator<GridViewType,
                                                                               ConstDiscreteFunction<SourceSpaceType,
                                                                                                     SourceVectorType>,
                                                                               DiscreteFunction<RangeSpaceType,
                                                                                                RangeVectorType>>>>::
    type
    make_local_l2_prolongation_localizable_operator(
        const GridViewType& grid_view, const ConstDiscreteFunction<SourceSpaceType, SourceVectorType>& source,
        DiscreteFunction<RangeSpaceType, RangeVectorType>& range, const size_t over_integrate = 0)
{
  return DSC::
      make_unique<L2LocalProlongationLocalizableOperator<GridViewType,
                                                         ConstDiscreteFunction<SourceSpaceType, SourceVectorType>,
                                                         DiscreteFunction<RangeSpaceType, RangeVectorType>>>(
          over_integrate, grid_view, source, range);
} // ... make_local_l2_prolongation_localizable_operator(...)

template <class SourceSpaceType, class SourceVectorType, class RangeSpaceType, class RangeVectorType>
typename std::enable_if<is_space<SourceSpaceType>::value && Stuff::LA::is_vector<SourceVectorType>::value
                            && is_space<RangeSpaceType>::value && Stuff::LA::is_vector<RangeVectorType>::value,
                        std::unique_ptr<L2LocalProlongationLocalizableOperator<
                            typename RangeSpaceType::GridViewType,
                            ConstDiscreteFunction<SourceSpaceType, SourceVectorType>,
                            DiscreteFunction<RangeSpaceType, RangeVectorType>>>>::type
make_local_l2_prolongation_localizable_operator(const ConstDiscreteFunction<SourceSpaceType, SourceVectorType>& source,
                                                DiscreteFunction<RangeSpaceType, RangeVectorType>& range,
                                                const size_t over_integrate = 0)
{
  return DSC::make_unique<L2LocalProlongationLocalizableOperator<
      typename RangeSpaceType::GridViewType,
      ConstDiscreteFunction<SourceSpaceType, SourceVectorType>,
      DiscreteFunction<RangeSpaceType, RangeVectorType>>>(over_integrate, range.space().grid_view(), source, range);
} // ... make_local_l2_prolongation_localizable_operator(...)


template <class GridViewImp, class FieldImp>
class L2LocalProlongationOperator
    : public OperatorInterface<internal::L2LocalProlongationOperatorTraits<GridViewImp, FieldImp>>
{
  typedef OperatorInterface<internal::L2LocalProlongationOperatorTraits<GridViewImp, FieldImp>> BaseType;

public:
  typedef internal::L2LocalProlongationOperatorTraits<GridViewImp, FieldImp> Traits;
  typedef GridViewImp GridViewType;
  using typename BaseType::FieldType;

private:
  typedef typename Stuff::Grid::Entity<GridViewType>::Type E;
  typedef typename GridViewType::ctype D;
  static const size_t d = GridViewType::dimension;

public:
  L2LocalProlongationOperator(const size_t over_integrate, GridViewType grid_view)
    : grid_view_(grid_view)
    , over_integrate_(over_integrate)
  {
  }

  L2LocalProlongationOperator(GridViewType grid_view)
    : grid_view_(grid_view)
    , over_integrate_(0)
  {
  }

  template <class SS, class SV, class RS, class RV>
  void apply(const ConstDiscreteFunction<SS, SV>& source, DiscreteFunction<RS, RV>& range) const
  {
    L2LocalProlongationLocalizableOperator<GridViewType, ConstDiscreteFunction<SS, SV>, DiscreteFunction<RS, RV>> op(
        over_integrate_, grid_view_, source, range);
    op.apply();
  }

  template <class RangeType, class SourceType>
  FieldType apply2(const RangeType& /*range*/, const SourceType& /*source*/) const
  {
    DUNE_THROW(NotImplemented, "Go ahead if you think this makes sense!");
  }

  template <class RangeType, class SourceType>
  void apply_inverse(const RangeType& /*range*/, SourceType& /*source*/,
                     const Stuff::Common::Configuration& /*opts*/) const
  {
    DUNE_THROW(NotImplemented, "Go ahead if you think this makes sense!");
  }

  std::vector<std::string> invert_options() const
  {
    DUNE_THROW(NotImplemented, "Go ahead if you think this makes sense!");
  }

  Stuff::Common::Configuration invert_options(const std::string& /*type*/) const
  {
    DUNE_THROW(NotImplemented, "Go ahead if you think this makes sense!");
  }

private:
  GridViewType grid_view_;
  const size_t over_integrate_;
}; // class L2LocalProlongationOperator


template <class GridViewType>
typename std::enable_if<Stuff::Grid::is_grid_layer<GridViewType>::value,
                        std::unique_ptr<L2LocalProlongationOperator<GridViewType>>>::type
make_local_l2_prolongation_operator(const GridViewType& grid_view, const size_t over_integrate = 0)
{
  return DSC::make_unique<L2LocalProlongationOperator<GridViewType>>(over_integrate, grid_view);
}


} // namespace GDT
} // namespace Dune

#endif // DUNE_GDT_PROLONGATIONS_L2_LOCAL_HH
