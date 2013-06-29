// This file is part of the dune-gdt project:
//   http://users.dune-project.org/projects/dune-gdt
// Copyright holders: Felix Albrecht
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_GDT_LOCALFUNCTIONAL_CODIM1_HH
#define DUNE_GDT_LOCALFUNCTIONAL_CODIM1_HH

#include <utility>
#include <vector>

#include <dune/common/densevector.hh>

#include <dune/geometry/quadraturerules.hh>

#include <dune/stuff/common/matrix.hh>

#include "../basefunctionset/interface.hh"
#include "../localevaluation/interface.hh"
#include "interface.hh"

namespace Dune {
namespace GDT {
namespace LocalFunctional {


// forward, to be used in the traits
template <class UnaryEvaluationImp>
class Codim1Integral;


template <class UnaryEvaluationImp>
class Codim1IntegralTraits
{
public:
  typedef Codim1Integral<UnaryEvaluationImp> derived_type;
  typedef LocalEvaluation::Codim1Interface<typename UnaryEvaluationImp::Traits, 1> UnaryEvaluationType;
};


template <class UnaryEvaluationImp>
class Codim1Integral : public LocalFunctional::Codim1Interface<Codim1IntegralTraits<UnaryEvaluationImp>>
{
public:
  typedef Codim1IntegralTraits<UnaryEvaluationImp> Traits;
  typedef typename Traits::UnaryEvaluationType UnaryEvaluationType;

private:
  static const size_t numTmpObjectsRequired_ = 1;

public:
  Codim1Integral(const UnaryEvaluationImp evaluation)
    : evaluation_(evaluation)
  {
  }

  template <class... Args>
  Codim1Integral(Args&&... args)
    : evaluation_(std::forward<Args>(args)...)
  {
  }

  size_t numTmpObjectsRequired() const
  {
    return numTmpObjectsRequired_;
  }

  template <class T, class IntersectionType, class D, int d, class R, int r, int rC>
  void apply(const BaseFunctionSetInterface<T, D, d, R, r, rC>& testBase, const IntersectionType& intersection,
             Dune::DynamicVector<R>& ret, std::vector<Dune::DynamicVector<R>>& tmpLocalVectors) const
  {
    // local inducing function
    const auto& entity        = testBase.entity();
    const auto localFunctions = evaluation_.localFunctions(entity);
    // quadrature
    typedef Dune::QuadratureRules<D, d - 1> FaceQuadratureRules;
    typedef Dune::QuadratureRule<D, d - 1> FaceQuadratureType;
    const int quadratureOrder = evaluation().order(localFunctions, testBase);
    assert(quadratureOrder >= 0 && "Not implemented for negative integration orders!");
    const FaceQuadratureType& faceQuadrature = FaceQuadratureRules::rule(intersection.type(), 2 * quadratureOrder + 1);
    // check vector and tmp storage
    const size_t size = testBase.size();
    assert(ret.size() >= size);
    assert(tmpLocalVectors.size() >= numTmpObjectsRequired_);
    Dune::DynamicVector<R>& localVector = tmpLocalVectors[0];
    // loop over all quadrature points
    for (auto quadPoint = faceQuadrature.begin(); quadPoint != faceQuadrature.end(); ++quadPoint) {
      const Dune::FieldVector<D, d - 1> localPoint = quadPoint->position();
      const R integrationFactor                    = intersection.geometry().integrationElement(localPoint);
      const R quadratureWeight                     = quadPoint->weight();
      // clear target vector
      Dune::Stuff::Common::clear(localVector);
      // evaluate local
      evaluation().evaluate(localFunctions, testBase, intersection, localPoint, localVector);
      // compute integral
      assert(localVector.size() >= size);
      // loop over all test basis functions
      for (size_t ii = 0; ii < size; ++ii) {
        ret[ii] += localVector[ii] * integrationFactor * quadratureWeight;
      } // loop over all test basis functions
    } // loop over all quadrature points
  } // void apply(...) const

private:
  const UnaryEvaluationType& evaluation() const
  {
    return static_cast<const UnaryEvaluationType&>(evaluation_);
  }

  const UnaryEvaluationImp evaluation_;
}; // class Codim1Integral


} // namespace LocalFunctional
} // namespace GDT
} // namespace Dune

#endif // DUNE_GDT_LOCALFUNCTIONAL_CODIM1_HH