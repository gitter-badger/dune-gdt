// This file is part of the dune-gdt project:
//   https://github.com/dune-community/dune-gdt
// Copyright 2010-2016 dune-gdt developers and contributors. All rights reserved.
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2015 - 2016)

#include "config.h"

#if HAVE_ALUGRID

#include <dune/grid/alugrid.hh>

#include "../problems/ESV2007.hh"
#include "../eocexpectations.hh"


namespace Dune {
namespace GDT {
namespace Test {


template <bool anything>
class LinearEllipticEocExpectations<LinearElliptic::ESV2007TestCase<ALUGrid<2, 2, simplex, conforming>, double, 1>,
                                    LinearElliptic::ChooseDiscretizer::cg, 1, anything>
    : public internal::LinearEllipticEocExpectationsBase<1>
{
  typedef LinearElliptic::ESV2007TestCase<ALUGrid<2, 2, simplex, conforming>, double, 1> TestCaseType;

public:
  static std::vector<double> results(const TestCaseType& /*test_case*/, const std::string type)
  {
    if (type == "L2")
      return {3.82e-02, 9.64e-03, 2.42e-03, 6.04e-04};
    else if (type == "H1_semi" || type == "energy")
      return {1.84e-01, 9.24e-02, 4.63e-02, 2.31e-02};
    else
      EXPECT_TRUE(false) << "test results missing for type: " << type;
    return {};
  } // ... results(...)
}; // LinearEllipticEocExpectations

template <bool anything>
class LinearEllipticEocExpectations<LinearElliptic::ESV2007TestCase<ALUGrid<2, 2, simplex, nonconforming>, double, 1>,
                                    LinearElliptic::ChooseDiscretizer::cg, 1, anything>
    : public internal::LinearEllipticEocExpectationsBase<1>
{
  typedef LinearElliptic::ESV2007TestCase<ALUGrid<2, 2, simplex, nonconforming>, double, 1> TestCaseType;

public:
  static std::vector<double> results(const TestCaseType& /*test_case*/, const std::string type)
  {
    if (type == "L2")
      return {4.22e-02, 1.08e-02, 2.70e-03, 6.76e-04};
    else if (type == "H1_semi" || type == "energy")
      return {1.94e-01, 9.79e-02, 4.91e-02, 2.45e-02};
    else
      EXPECT_TRUE(false) << "test results missing for type: " << type;
    return {};
  } // ... results(...)
}; // LinearEllipticEocExpectations


template class LinearEllipticEocExpectations<LinearElliptic::ESV2007TestCase<ALUGrid<2, 2, simplex, conforming>, double,
                                                                             1>,
                                             LinearElliptic::ChooseDiscretizer::cg, 1>;
template class LinearEllipticEocExpectations<LinearElliptic::ESV2007TestCase<ALUGrid<2, 2, simplex, nonconforming>,
                                                                             double, 1>,
                                             LinearElliptic::ChooseDiscretizer::cg, 1>;


} // namespace Test
} // namespace GDT
} // namespace Dune

#endif // HAVE_ALUGRID
