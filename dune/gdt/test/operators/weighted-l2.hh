// This file is part of the dune-gdt project:
//   https://github.com/dune-community/dune-gdt
// Copyright 2010-2016 dune-gdt developers and contributors. All rights reserved.
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2015 - 2016)
//   Tobias Leibner  (2016)

#ifndef DUNE_GDT_TEST_OPERATORS_WEIGHTED_L2_HH
#define DUNE_GDT_TEST_OPERATORS_WEIGHTED_L2_HH

#include <dune/stuff/common/string.hh>
#include <dune/stuff/test/gtest/gtest.h>

#include <dune/gdt/projections.hh>
#include <dune/gdt/operators/weighted-l2.hh>

#include "base.hh"

namespace Dune {
namespace GDT {
namespace Test {


/**
 * \note The values in correct_for_constant_arguments(), etc., are valid for the d-dimendional unit cube.
 */
template <class SpaceType>
struct WeightedL2ProductBase
{
  typedef typename SpaceType::GridViewType GridViewType;
  typedef typename GridViewType::Grid GridType;
  typedef Dune::Stuff::Grid::Providers::Cube<GridType> GridProviderType;
  typedef typename GridViewType::template Codim<0>::Entity EntityType;
  typedef typename SpaceType::DomainFieldType DomainFieldType;
  static const size_t dimDomain = SpaceType::dimDomain;
  typedef typename SpaceType::RangeFieldType RangeFieldType;
  typedef Stuff::Functions::Expression<EntityType, DomainFieldType, dimDomain, RangeFieldType, 1>
      ExpressionFunctionType;

  WeightedL2ProductBase(const double weight_value = 42)
    : weight_value_(weight_value)
    , weight_("x", DSC::to_string(double(weight_value_)), 0) // linker error if double(...) is missing
    , constant_("x", "1.0", 0)
    , linear_("x", "x[0] - 1.0", 1)
    , quadratic_("x", "x[0]*x[0]", 2)
  {
  }

  virtual ~WeightedL2ProductBase() = default;

  virtual RangeFieldType compute(const ExpressionFunctionType& function) const = 0;

  void correct_for_constant_arguments(const RangeFieldType epsilon = 1e-15) const
  {
    check(compute(constant_), weight_value_ * 1.0, epsilon);
  }

  void correct_for_linear_arguments(const RangeFieldType epsilon = 1e-15) const
  {
    check(compute(linear_), weight_value_ * (1.0 / 3.0), epsilon);
  }

  void correct_for_quadratic_arguments(const RangeFieldType epsilon = 1e-15) const
  {
    check(compute(quadratic_), weight_value_ * (1.0 / 5.0), epsilon);
  }

  void check(const RangeFieldType& result, const RangeFieldType& expected, const RangeFieldType epsilon) const
  {
    const auto error = std::abs(expected - result);
    EXPECT_LE(error, epsilon) << "result:     " << result << "\n"
                              << "expected:   " << expected << "\n"
                              << "difference: " << std::scientific << error;
  } // ... check(...)

  const double weight_value_;
  const ExpressionFunctionType weight_;
  const ExpressionFunctionType constant_;
  const ExpressionFunctionType linear_;
  const ExpressionFunctionType quadratic_;
}; // struct WeightedL2ProductBase


template <class SpaceType>
struct WeightedL2LocalizableProductTest : public WeightedL2ProductBase<SpaceType>,
                                          public LocalizableProductBase<SpaceType>
{
  typedef WeightedL2ProductBase<SpaceType> WeightedL2BaseType;
  typedef LocalizableProductBase<SpaceType> LocalizableBaseType;
  using typename LocalizableBaseType::GridViewType;
  using typename WeightedL2BaseType::ExpressionFunctionType;
  using typename LocalizableBaseType::ScalarFunctionType;
  using typename LocalizableBaseType::RangeFieldType;
  using WeightedL2BaseType::dimDomain;

  void constructible_by_ctor()
  {
    const auto& weight    = this->weight_;
    const auto& grid_view = this->space_.grid_view();
    const auto& source    = this->scalar_function_;
    const auto& range     = this->scalar_function_;

    typedef WeightedL2LocalizableProduct<ExpressionFunctionType,
                                         GridViewType,
                                         ScalarFunctionType,
                                         ScalarFunctionType,
                                         double>
        CtorTestProductType;
    CtorTestProductType DUNE_UNUSED(wo_over_integrate)(weight, grid_view, range, source);
    CtorTestProductType DUNE_UNUSED(with_over_integrate)(1, weight, grid_view, range, source);
  } // ... constructible_by_ctor(...)

  void constructible_by_factory()
  {
    const auto& weight    = this->weight_;
    const auto& grid_view = this->space_.grid_view();
    const auto& source    = this->scalar_function_;
    const auto& range     = this->scalar_function_;

    auto DUNE_UNUSED(wo_over_integrate)   = make_weighted_l2_localizable_product(weight, grid_view, range, source);
    auto DUNE_UNUSED(with_over_integrate) = make_weighted_l2_localizable_product(weight, grid_view, range, source, 1);
  } // ... constructible_by_factory()

  virtual RangeFieldType compute(const ExpressionFunctionType& function) const override final
  {
    const auto& weight    = this->weight_;
    const auto& grid_view = this->space_.grid_view();

    auto product      = make_weighted_l2_localizable_product(weight, grid_view, function, function);
    const auto result = product->apply2();

    auto product_tbb = make_weighted_l2_localizable_product(weight, grid_view, function, function);
    product_tbb->walk(true);
    const auto result_tbb = product_tbb->apply2();

    EXPECT_EQ(result_tbb, result);
    return result;
  } // ... compute(...)

  void is_localizable_product()
  {
    const auto& weight    = this->weight_;
    const auto& grid_view = this->space_.grid_view();
    const auto& source    = this->scalar_function_;
    const auto& range     = this->scalar_function_;

    auto product = make_weighted_l2_localizable_product(weight, grid_view, range, source);
    this->localizable_product_test(*product);
  } // ... is_localizable_product(...)
}; // struct WeightedL2LocalizableProductTest


/**
 * \note Assumes that Operators::Projection does the right thing!
 */
template <class SpaceType>
struct WeightedL2MatrixOperatorTest : public WeightedL2ProductBase<SpaceType>, public MatrixOperatorBase<SpaceType>
{
  typedef WeightedL2ProductBase<SpaceType> WeightedL2BaseType;
  typedef MatrixOperatorBase<SpaceType> MatrixBaseType;
  using typename MatrixBaseType::GridViewType;
  using typename WeightedL2BaseType::ExpressionFunctionType;
  using typename MatrixBaseType::DiscreteFunctionType;
  using typename MatrixBaseType::ScalarFunctionType;
  using typename MatrixBaseType::RangeFieldType;
  using typename MatrixBaseType::MatrixType;
  using WeightedL2BaseType::dimDomain;

  void constructible_by_ctor()
  {
    const auto& weight    = this->weight_;
    const auto& space     = this->space_;
    const auto& grid_view = this->space_.grid_view();

    // without matrix
    //   without over_integrate
    //     simplified argument list
    WeightedL2MatrixOperator<ExpressionFunctionType, SpaceType> DUNE_UNUSED(no_matrix_1)(weight, space);
    WeightedL2MatrixOperator<ExpressionFunctionType, SpaceType> DUNE_UNUSED(no_matrix_2)(weight, space, grid_view);
    WeightedL2MatrixOperator<ExpressionFunctionType, SpaceType> DUNE_UNUSED(no_matrix_3)(
        weight, space, space, grid_view);
    //     full argument list
    WeightedL2MatrixOperator<ExpressionFunctionType, SpaceType, MatrixType, GridViewType, SpaceType, double>
        DUNE_UNUSED(no_matrix_4)(weight, space);
    WeightedL2MatrixOperator<ExpressionFunctionType, SpaceType, MatrixType, GridViewType, SpaceType, double>
        DUNE_UNUSED(no_matrix_5)(weight, space, grid_view);
    WeightedL2MatrixOperator<ExpressionFunctionType, SpaceType, MatrixType, GridViewType, SpaceType, double>
        DUNE_UNUSED(no_matrix_6)(weight, space, space, grid_view);
    //   with over_integrate
    //     simplified argument list
    WeightedL2MatrixOperator<ExpressionFunctionType, SpaceType> DUNE_UNUSED(no_matrix_7)(1, weight, space);
    WeightedL2MatrixOperator<ExpressionFunctionType, SpaceType> DUNE_UNUSED(no_matrix_8)(1, weight, space, grid_view);
    WeightedL2MatrixOperator<ExpressionFunctionType, SpaceType> DUNE_UNUSED(no_matrix_9)(
        1, weight, space, space, grid_view);
    //     full argument list
    WeightedL2MatrixOperator<ExpressionFunctionType, SpaceType, MatrixType, GridViewType, SpaceType, double>
        DUNE_UNUSED(no_matrix_10)(1, weight, space);
    WeightedL2MatrixOperator<ExpressionFunctionType, SpaceType, MatrixType, GridViewType, SpaceType, double>
        DUNE_UNUSED(no_matrix_11)(1, weight, space, grid_view);
    WeightedL2MatrixOperator<ExpressionFunctionType, SpaceType, MatrixType, GridViewType, SpaceType, double>
        DUNE_UNUSED(no_matrix_12)(1, weight, space, space, grid_view);
    // with matrix
    MatrixType matrix(space.mapper().size(), space.mapper().size(), space.compute_volume_pattern());
    //   without over_integrate
    //     simplified argument list
    WeightedL2MatrixOperator<ExpressionFunctionType, SpaceType> DUNE_UNUSED(matrix_1)(weight, matrix, space);
    WeightedL2MatrixOperator<ExpressionFunctionType, SpaceType> DUNE_UNUSED(matrix_2)(weight, matrix, space, grid_view);
    WeightedL2MatrixOperator<ExpressionFunctionType, SpaceType> DUNE_UNUSED(matrix_3)(
        weight, matrix, space, space, grid_view);
    //     full argument list
    WeightedL2MatrixOperator<ExpressionFunctionType, SpaceType, MatrixType, GridViewType, SpaceType, double>
        DUNE_UNUSED(matrix_4)(weight, matrix, space);
    WeightedL2MatrixOperator<ExpressionFunctionType, SpaceType, MatrixType, GridViewType, SpaceType, double>
        DUNE_UNUSED(matrix_5)(weight, matrix, space, grid_view);
    WeightedL2MatrixOperator<ExpressionFunctionType, SpaceType, MatrixType, GridViewType, SpaceType, double>
        DUNE_UNUSED(matrix_6)(weight, matrix, space, space, grid_view);
    //   with over_integrate
    //     simplified argument list
    WeightedL2MatrixOperator<ExpressionFunctionType, SpaceType> DUNE_UNUSED(matrix_7)(1, weight, matrix, space);
    WeightedL2MatrixOperator<ExpressionFunctionType, SpaceType> DUNE_UNUSED(matrix_8)(
        1, weight, matrix, space, grid_view);
    WeightedL2MatrixOperator<ExpressionFunctionType, SpaceType> DUNE_UNUSED(matrix_9)(
        1, weight, matrix, space, space, grid_view);
    //     full argument list
    WeightedL2MatrixOperator<ExpressionFunctionType, SpaceType, MatrixType, GridViewType, SpaceType, double>
        DUNE_UNUSED(matrix_10)(1, weight, matrix, space);
    WeightedL2MatrixOperator<ExpressionFunctionType, SpaceType, MatrixType, GridViewType, SpaceType, double>
        DUNE_UNUSED(matrix_11)(1, weight, matrix, space, grid_view);
    WeightedL2MatrixOperator<ExpressionFunctionType, SpaceType, MatrixType, GridViewType, SpaceType, double>
        DUNE_UNUSED(matrix_12)(1, weight, matrix, space, space, grid_view);
  } // ... constructible_by_ctor(...)

  void constructible_by_factory()
  {
    const auto& weight    = this->weight_;
    const auto& space     = this->space_;
    const auto& grid_view = this->space_.grid_view();
    MatrixType matrix(space.mapper().size(), space.mapper().size(), space.compute_volume_pattern());

    // without matrix
    auto DUNE_UNUSED(op_01) = make_weighted_l2_matrix_operator<MatrixType>(weight, space);
    auto DUNE_UNUSED(op_02) = make_weighted_l2_matrix_operator<MatrixType>(weight, space, 1);
    auto DUNE_UNUSED(op_03) = make_weighted_l2_matrix_operator<MatrixType>(weight, space, grid_view);
    auto DUNE_UNUSED(op_04) = make_weighted_l2_matrix_operator<MatrixType>(weight, space, grid_view, 1);
    auto DUNE_UNUSED(op_05) = make_weighted_l2_matrix_operator<MatrixType>(weight, space, space, grid_view);
    auto DUNE_UNUSED(op_06) = make_weighted_l2_matrix_operator<MatrixType>(weight, space, space, grid_view, 1);
    // with matrix
    auto DUNE_UNUSED(op_07) = make_weighted_l2_matrix_operator(weight, matrix, space);
    auto DUNE_UNUSED(op_08) = make_weighted_l2_matrix_operator(weight, matrix, space, 1);
    auto DUNE_UNUSED(op_09) = make_weighted_l2_matrix_operator(weight, matrix, space, grid_view);
    auto DUNE_UNUSED(op_10) = make_weighted_l2_matrix_operator(weight, matrix, space, grid_view, 1);
    auto DUNE_UNUSED(op_11) = make_weighted_l2_matrix_operator(weight, matrix, space, space, grid_view);
    auto DUNE_UNUSED(op_12) = make_weighted_l2_matrix_operator(weight, matrix, space, space, grid_view, 1);
  } // ... constructible_by_factory()

  virtual RangeFieldType compute(const ExpressionFunctionType& function) const override final
  {
    const auto& weight = this->weight_;
    const auto& space  = this->space_;
    // project the function
    DiscreteFunctionType discrete_function(space);
    project(function, discrete_function);
    // compute product
    auto product      = make_weighted_l2_matrix_operator<MatrixType>(weight, space);
    const auto result = product->apply2(discrete_function, discrete_function);

    auto product_tbb = make_weighted_l2_matrix_operator<MatrixType>(weight, space);
    product_tbb->assemble(true);
    const auto result_tbb = product_tbb->apply2(discrete_function, discrete_function);
    EXPECT_DOUBLE_EQ(result_tbb, result);
    return result;
  } // ... compute(...)

  void is_matrix_operator()
  {
    const auto& weight = this->weight_;
    const auto& space  = this->space_;

    auto op = make_weighted_l2_matrix_operator<MatrixType>(weight, space);
    this->matrix_operator_test(*op);
  } // ... is_matrix_operator(...)
}; // struct WeightedL2MatrixOperatorTest


template <class SpaceType>
struct WeightedL2OperatorTest : public WeightedL2ProductBase<SpaceType>, public OperatorBase<SpaceType>
{
  typedef WeightedL2ProductBase<SpaceType> WeightedL2BaseType;
  typedef OperatorBase<SpaceType> OperatorBaseType;
  using typename OperatorBaseType::GridViewType;
  using typename WeightedL2BaseType::ExpressionFunctionType;
  using typename OperatorBaseType::DiscreteFunctionType;
  using typename OperatorBaseType::ScalarFunctionType;
  using typename OperatorBaseType::RangeFieldType;
  using typename OperatorBaseType::MatrixType;
  using typename OperatorBaseType::VectorType;
  using WeightedL2BaseType::dimDomain;

  void constructible_by_ctor()
  {
    const auto& weight    = this->weight_;
    const auto& grid_view = this->space_.grid_view();

    WeightedL2Operator<ExpressionFunctionType, GridViewType> DUNE_UNUSED(wo_over_integrate)(weight, grid_view);
    WeightedL2Operator<ExpressionFunctionType, GridViewType> DUNE_UNUSED(with_over_integrate)(weight, grid_view, 1);
  } // ... constructible_by_ctor(...)

  void constructible_by_factory()
  {
    const auto& weight    = this->weight_;
    const auto& grid_view = this->space_.grid_view();

    auto DUNE_UNUSED(wo_over_integrate)   = make_weighted_l2_operator(grid_view, weight);
    auto DUNE_UNUSED(with_over_integrate) = make_weighted_l2_operator(grid_view, weight, 1);
  } // ... constructible_by_factory()

  virtual RangeFieldType compute(const ExpressionFunctionType& function) const override final
  {
    const auto& weight    = this->weight_;
    const auto& grid_view = this->space_.grid_view();

    return make_weighted_l2_operator(grid_view, weight)->apply2(function, function);
  }

  void apply_is_callable()
  {
    const auto& weight    = this->weight_;
    const auto& grid_view = this->space_.grid_view();
    auto& source          = this->discrete_function_;
    auto range            = make_discrete_function<VectorType>(this->space_);

    auto op = make_weighted_l2_operator(grid_view, weight);
    op->apply(source, range);
  } // ... apply_is_callable(...)
}; // struct WeightedL2OperatorTest


} // namespace Test
} // namespace GDT
} // namespace Dune

#endif // DUNE_GDT_TEST_OPERATORS_WEIGHTED_L2_HH
