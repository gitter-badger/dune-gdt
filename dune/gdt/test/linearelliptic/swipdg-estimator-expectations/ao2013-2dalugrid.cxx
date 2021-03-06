// This file is part of the dune-gdt project:
//   https://github.com/dune-community/dune-gdt
// Copyright 2010-2016 dune-gdt developers and contributors. All rights reserved.
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2016)

#include "config.h"

#if HAVE_ALUGRID

#include <dune/grid/alugrid.hh>

#include "../problems/AO2013.hh"
#include "../swipdg-estimator-expectations.hh"

namespace Dune {
namespace GDT {
namespace Test {
namespace internal {


// polorder 1, conforming

std::vector<double>
results_LinearEllipticSwipdgEstimatorExpectationsAO2013TestCaseALUGrid22simplexconformingdouble1swipdg1(
    const LinearElliptic::AO2013TestCase<ALUGrid<2, 2, simplex, conforming>, double, 1>& /*test_case*/,
    const std::string type)
{
  if (type == "energy")
    return {9.10e-02, 5.23e-02, 2.68e-02, 1.20e-02};
  else if (type == LinearElliptic::SwipdgFluxreconstrutionEstimators::local_nonconformity_ESV2007_id())
    return {9.57e-02, 1.10e-01, 5.12e-02, 2.17e-02};
  else if (type == LinearElliptic::SwipdgFluxreconstrutionEstimators::local_residual_ESV2007_id())
    return {1.03e-15, 5.20e-13, 4.68e-13, 1.19e-12};
  else if (type == LinearElliptic::SwipdgFluxreconstrutionEstimators::local_diffusive_flux_ESV2007_id())
    return {1.12e-01, 6.54e-02, 3.54e-02, 1.90e-02};
  else if (type == LinearElliptic::SwipdgFluxreconstrutionEstimators::ESV2007_id())
    return {1.48e-01, 1.28e-01, 6.22e-02, 2.89e-02};
  else if (type == "efficiency_" + LinearElliptic::SwipdgFluxreconstrutionEstimators::ESV2007_id())
    return {1.62e+00, 2.45e+00, 2.32e+00, 2.40e+00};
  else if (type == LinearElliptic::SwipdgFluxreconstrutionEstimators::ESV2007_alternative_summation_id())
    return {4.56e-01, 4.19e-01, 2.94e-01, 2.02e-01};
  else if (type
           == "efficiency_" + LinearElliptic::SwipdgFluxreconstrutionEstimators::ESV2007_alternative_summation_id())
    return {5.01e+00, 8.01e+00, 1.10e+01, 1.68e+01};
  else
    EXPECT_TRUE(false) << "test results missing for type: " << type;
  return {};
} // ... results(...)


} // namespace internal


// polorder 1, conforming

#if HAVE_EIGEN

template <bool anything>
class LinearEllipticSwipdgEstimatorExpectations<LinearElliptic::AO2013TestCase<ALUGrid<2, 2, simplex, conforming>,
                                                                               double, 1>,
                                                LinearElliptic::ChooseDiscretizer::swipdg, 1,
                                                Stuff::LA::ChooseBackend::eigen_sparse, anything>
    : public internal::LinearEllipticSwipdgEstimatorExpectationsBase<1>
{
  typedef LinearElliptic::AO2013TestCase<ALUGrid<2, 2, simplex, conforming>, double, 1> TestCaseType;

public:
  static std::vector<double> results(const TestCaseType& test_case, const std::string type)
  {
#ifndef NDEBUG
    if (type == LinearElliptic::SwipdgFluxreconstrutionEstimators::local_residual_ESV2007_id())
      return {7.62e-16, 8.48e-13, 1.18e-13, 2.47e-13};
    else
#endif
      return internal::
          results_LinearEllipticSwipdgEstimatorExpectationsAO2013TestCaseALUGrid22simplexconformingdouble1swipdg1(
              test_case, type);
  }
}; // LinearEllipticSwipdgEstimatorExpectations


template class LinearEllipticSwipdgEstimatorExpectations<LinearElliptic::AO2013TestCase<ALUGrid<2, 2, simplex,
                                                                                                conforming>,
                                                                                        double, 1>,
                                                         LinearElliptic::ChooseDiscretizer::swipdg, 1,
                                                         Stuff::LA::ChooseBackend::eigen_sparse>;


#endif // HAVE_EIGEN
#if HAVE_DUNE_ISTL


template <bool anything>
class LinearEllipticSwipdgEstimatorExpectations<LinearElliptic::AO2013TestCase<ALUGrid<2, 2, simplex, conforming>,
                                                                               double, 1>,
                                                LinearElliptic::ChooseDiscretizer::swipdg, 1,
                                                Stuff::LA::ChooseBackend::istl_sparse, anything>
    : public internal::LinearEllipticSwipdgEstimatorExpectationsBase<1>
{
  typedef LinearElliptic::AO2013TestCase<ALUGrid<2, 2, simplex, conforming>, double, 1> TestCaseType;

public:
  static std::vector<double> results(const TestCaseType& test_case, const std::string type)
  {
    return internal::
        results_LinearEllipticSwipdgEstimatorExpectationsAO2013TestCaseALUGrid22simplexconformingdouble1swipdg1(
            test_case, type);
  }
}; // LinearEllipticSwipdgEstimatorExpectations


template class LinearEllipticSwipdgEstimatorExpectations<LinearElliptic::AO2013TestCase<ALUGrid<2, 2, simplex,
                                                                                                conforming>,
                                                                                        double, 1>,
                                                         LinearElliptic::ChooseDiscretizer::swipdg, 1,
                                                         Stuff::LA::ChooseBackend::istl_sparse>;


#endif // HAVE_DUNE_ISTL

} // namespace Test
} // namespace GDT
} // namespace Dune

#endif // HAVE_ALUGRID
