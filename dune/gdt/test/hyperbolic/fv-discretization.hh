// This file is part of the dune-gdt project:
//   https://github.com/dune-community/dune-gdt
// Copyright 2010-2016 dune-gdt developers and contributors. All rights reserved.
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2016)
//   Tobias Leibner  (2016)

#ifndef DUNE_GDT_TEST_HYPERBOLIC_FV_DISCRETIZATION_HH
#define DUNE_GDT_TEST_HYPERBOLIC_FV_DISCRETIZATION_HH

#ifndef THIS_IS_A_BUILDBOT_BUILD
#define THIS_IS_A_BUILDBOT_BUILD 0
#endif

#include <dune/stuff/test/common.hh>

#include <dune/gdt/spaces/interface.hh>
#include <dune/gdt/test/hyperbolic/eocstudy.hh>
#include <dune/gdt/test/hyperbolic/discretizers/fv.hh>

#include "problems.hh"


template <class TestCaseType>
struct hyperbolic_FV_discretization_godunov_euler : public ::testing::Test
{
  static void eoc_study()
  {
    using namespace Dune;
    using namespace Dune::GDT;
#if THIS_IS_A_BUILDBOT_BUILD
    TestCaseType test_case(/*num_refs = */ 1, /*divide_t_end_by = */ 5.0);
#else
    TestCaseType test_case;
#endif
    test_case.print_header(DSC_LOG_INFO);
    DSC_LOG_INFO << std::endl;
    typedef typename Hyperbolic::FVDiscretizer<TestCaseType,
                                               typename TestCaseType::GridType,
                                               double,
                                               TestCaseType::dimRange,
                                               TestCaseType::dimRangeCols,
                                               false,
                                               false,
                                               false>
        Discretizer;
    Tests::HyperbolicEocStudy<TestCaseType, Discretizer> eoc_study(test_case, {});
    Stuff::Test::check_eoc_study_for_success(eoc_study, eoc_study.run(DSC_LOG_INFO));
  } // ... eoc_study()
}; // hyperbolic_FV_discretization_godunov_euler

template <class TestCaseType>
struct hyperbolic_FV_discretization_godunov_adaptiveRK : public ::testing::Test
{
  static void eoc_study()
  {
    using namespace Dune;
    using namespace Dune::GDT;
#if THIS_IS_A_BUILDBOT_BUILD
    TestCaseType test_case(/*num_refs = */ 1, /*divide_t_end_by = */ 5.0);
#else
    TestCaseType test_case;
#endif
    test_case.print_header(DSC_LOG_INFO);
    DSC_LOG_INFO << std::endl;
    typedef typename Hyperbolic::FVDiscretizer<TestCaseType,
                                               typename TestCaseType::GridType,
                                               double,
                                               TestCaseType::dimRange,
                                               TestCaseType::dimRangeCols,
                                               false,
                                               true,
                                               false>
        Discretizer;
    Tests::HyperbolicEocStudy<TestCaseType, Discretizer> eoc_study(test_case, {});
    Stuff::Test::check_eoc_study_for_success(eoc_study, eoc_study.run(DSC_LOG_INFO));
  } // ... eoc_study()
}; // hyperbolic_FV_discretization_godunov_adaptiveRK

template <class TestCaseType>
struct hyperbolic_FV_discretization_laxfriedrichs_euler : public ::testing::Test
{
  static void eoc_study()
  {
    using namespace Dune;
    using namespace Dune::GDT;
#if THIS_IS_A_BUILDBOT_BUILD
    TestCaseType test_case(/*num_refs = */ 1, /*divide_t_end_by = */ 5.0);
#else
    TestCaseType test_case;
#endif
    test_case.print_header(DSC_LOG_INFO);
    DSC_LOG_INFO << std::endl;
    typedef typename Hyperbolic::FVDiscretizer<TestCaseType,
                                               typename TestCaseType::GridType,
                                               double,
                                               TestCaseType::dimRange,
                                               TestCaseType::dimRangeCols,
                                               true,
                                               false,
                                               false>
        Discretizer;
    Tests::HyperbolicEocStudy<TestCaseType, Discretizer> eoc_study(test_case, {});
    Stuff::Test::check_eoc_study_for_success(eoc_study, eoc_study.run(DSC_LOG_INFO));
  } // ... eoc_study()
}; // hyperbolic_FV_discretization_laxfriedrichs_euler

template <class TestCaseType>
struct hyperbolic_FV_discretization_godunovwithreconstruction_euler : public ::testing::Test
{
  static void eoc_study()
  {
    using namespace Dune;
    using namespace Dune::GDT;
#if THIS_IS_A_BUILDBOT_BUILD
    TestCaseType test_case(/*num_refs = */ 1, /*divide_t_end_by = */ 5.0);
#else
    TestCaseType test_case;
#endif
    test_case.print_header(DSC_LOG_INFO);
    DSC_LOG_INFO << std::endl;
    typedef typename Hyperbolic::FVDiscretizer<TestCaseType,
                                               typename TestCaseType::GridType,
                                               double,
                                               TestCaseType::dimRange,
                                               TestCaseType::dimRangeCols,
                                               false,
                                               false,
                                               true>
        Discretizer;
    Tests::HyperbolicEocStudy<TestCaseType, Discretizer> eoc_study(test_case, {});
    Stuff::Test::check_eoc_study_for_success(eoc_study, eoc_study.run(DSC_LOG_INFO));
  } // ... eoc_study()
}; // hyperbolic_FV_discretization_godunov_euler

#endif // DUNE_GDT_TEST_HYPERBOLIC_FV_DISCRETIZATION_HH
