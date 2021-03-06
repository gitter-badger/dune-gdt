// This file is part of the dune-gdt project:
//   https://github.com/dune-community/dune-gdt
// Copyright 2010-2016 dune-gdt developers and contributors. All rights reserved.
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2016)
//   Tobias Leibner  (2016)

#ifndef DUNE_GDT_LOCAL_FLUXES_RHS_HH
#define DUNE_GDT_LOCAL_FLUXES_RHS_HH

#include <dune/stuff/functions/checkerboard.hh>

#include "interfaces.hh"

namespace Dune {
namespace GDT {


/** RHS evaluation for time-independent RHS q(u,x) that is based on Dune::Stuff::Functions::Checkerboard.
 *  TODO: static_assert for CheckerboardFunctionImp
 * */
template <class CheckerboardFunctionImp, class E, class D, size_t d, class R, size_t r, size_t rC = 1>
class CheckerboardBasedRhsEvaluationFluxInterface : public RhsEvaluationFluxInterface<E, D, d, R, r, rC>
{
  typedef RhsEvaluationFluxInterface<E, D, d, R, r, rC> BaseType;
  typedef CheckerboardBasedRhsEvaluationFluxInterface<CheckerboardFunctionImp, E, D, d, R, r, rC> ThisType;

public:
  // function q(u,x) for fixed x, i.e. only dependent on u
  typedef CheckerboardFunctionImp CheckerboardFunctionType;
  using typename BaseType::RangeType;
  using typename BaseType::DomainType;

  CheckerboardBasedRhsEvaluationFluxInterface(const CheckerboardFunctionType& checkerboard_function)
    : checkerboard_function_(checkerboard_function)
  {
  }

  virtual RangeType evaluate(const RangeType& u, const E& entity, const DomainType& /*x_local*/,
                             const double /*t_*/ = 0) const
  {
    return checkerboard_function_.localizable_function(entity).evaluate(u);
  }

  static std::string static_id()
  {
    return "gdt.CheckerboardBasedRhsEvaluationFluxInterface";
  }

  static std::unique_ptr<ThisType> create(const Dune::Stuff::Common::Configuration checkerboard_config,
                                          const std::string sub_name = static_id())
  {
    return DSC::make_unique<ThisType>(*(CheckerboardFunctionType::create(checkerboard_config, sub_name)));
  } // ... create(...)

private:
  const CheckerboardFunctionType checkerboard_function_;
}; // class CheckerboardBasedRhsEvaluationFluxInterface ...


} // namespace GDT
} // namespace Dune

#endif // DUNE_GDT_LOCAL_FLUXES_RHS_HH
