// This file is part of the dune-gdt project:
//   http://users.dune-project.org/projects/dune-gdt
// Copyright holders: Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "config.h"

#include <dune/grid/yaspgrid.hh>

#include <dune/gdt/test/hyperbolic/discretizers/fv.hh>

#include "problems/burgers.hh"
#include "eocexpectations.hh"


namespace Dune {
namespace GDT {
namespace Tests {


template <bool anything>
class HyperbolicEocExpectations<Hyperbolic::BurgersTestCase<Dune::YaspGrid<1>, double, 1>,
                                Hyperbolic::ChooseDiscretizer::fv, 1, anything>
    : public internal::HyperbolicEocExpectationsBase<1>
{
  typedef Hyperbolic::BurgersTestCase<Dune::YaspGrid<1>, double, 1> TestCaseType;

public:
  static std::vector<double> results(const TestCaseType& test_case, const std::string type)
  {
    if (type == "L1") {
      if (test_case.num_refinements() == 1)
        return {8.13e-02, 3.44e-02};
      else
        return {1.01e-01, 5.43e-02, 2.69e-02, 1.22e-02, 4.75e-03};
    } else
      EXPECT_TRUE(false) << "test results missing for type: " << type;
    return {};
  } // ... results(...)
}; // HyperbolicEocExpectations

template class HyperbolicEocExpectations<Hyperbolic::BurgersTestCase<Dune::YaspGrid<1>, double, 1>,
                                         Hyperbolic::ChooseDiscretizer::fv, 1>;


} // namespace Tests
} // namespace GDT
} // namespace Dune
