// This file is part of the dune-gdt project:
//   http://users.dune-project.org/projects/dune-gdt
// Copyright holders: Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_TEST_MAIN_ENABLE_INFO_LOGGING
#define DUNE_STUFF_TEST_MAIN_ENABLE_INFO_LOGGING 1
#endif

#include <dune/stuff/test/main.hxx> // <- This one has to come first (includes the config.h)!

#include "linearelliptic/swipdg-estimators.hh"
#include "linearelliptic/swipdg-estimator-testcases.hh"

using namespace Dune;
using namespace Dune::GDT;


#if HAVE_DUNE_FEM && HAVE_EIGEN && HAVE_ALUGRID

TYPED_TEST_CASE(linearelliptic_SWIPDG_estimators, AluGridTestCases);
TYPED_TEST(linearelliptic_SWIPDG_estimators, eoc_study_using_fem_and_eigen_and_alugrid_order_1)
{
  this->template eoc_study<ChooseSpaceBackend::fem, Stuff::LA::ChooseBackend::eigen_sparse, 1>();
}

#else

TEST(DISABLED_linearelliptic_SWIPDG_estimators, eoc_study_using_fem_and_eigen_and_alugrid_order_1)
{
}
TEST(DISABLED_linearelliptic_SWIPDG_estimators, eoc_study_using_fem_and_eigen_and_alugrid_order_2)
{
}

#endif