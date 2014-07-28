// This file is part of the dune-gdt project:
//   http://users.dune-project.org/projects/dune-gdt
// Copyright holders: Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_GDT_SPACES_BLOCK_HH
#define DUNE_GDT_SPACES_BLOCK_HH

#if HAVE_DUNE_GRID_MULTISCALE

#include <dune/stuff/common/exceptions.hh>

#include <dune/grid/multiscale/default.hh>

#include <dune/gdt/playground/mapper/block.hh>

#include "../../spaces/interface.hh"

namespace Dune {
namespace GDT {
namespace Spaces {


template <class LocalSpaceImp>
class Block;


namespace internal {


template <class LocalSpaceType>
class BlockTraits
{
  static_assert(std::is_base_of<SpaceInterface<typename LocalSpaceType::Traits>, LocalSpaceType>::value,
                "LocalSpaceType has to be derived from SpaceInterface!");
  typedef grid::Multiscale::Default<typename LocalSpaceType::GridViewType::Grid> MsGridType;

public:
  typedef Block<LocalSpaceType> derived_type;
  static const int polOrder = LocalSpaceType::polOrder;
  typedef typename LocalSpaceType::BackendType BackendType;
  typedef Mapper::Block<LocalSpaceType> MapperType;
  typedef typename LocalSpaceType::BaseFunctionSetType BaseFunctionSetType;
  typedef typename LocalSpaceType::CommunicatorType CommunicatorType;
  typedef typename MsGridType::GlobalGridViewType GridViewType;
  typedef typename LocalSpaceType::RangeFieldType RangeFieldType;
  static const unsigned int dimRange     = LocalSpaceType::dimRange;
  static const unsigned int dimRangeCols = LocalSpaceType::dimRangeCols;

  static const Stuff::Grid::ChoosePartView part_view_type = LocalSpaceType::part_view_type;

  static const bool needs_grid_view = LocalSpaceType::needs_grid_view;
}; // class BlockTraits


} // namespace internal


template <class LocalSpaceImp>
class Block : public SpaceInterface<internal::BlockTraits<LocalSpaceImp>>
{
  typedef SpaceInterface<internal::BlockTraits<LocalSpaceImp>> BaseType;

public:
  typedef internal::BlockTraits<LocalSpaceImp> Traits;
  typedef typename Traits::BackendType BackendType;
  typedef typename Traits::MapperType MapperType;
  typedef typename Traits::BaseFunctionSetType BaseFunctionSetType;
  typedef LocalSpaceImp LocalSpaceType;

  typedef typename BaseType::PatternType PatternType;
  typedef typename BaseType::GridViewType GridViewType;
  typedef typename BaseType::EntityType EntityType;

  typedef grid::Multiscale::Default<typename GridViewType::Grid> MsGridType;

  Block(const std::shared_ptr<const MsGridType> ms_grid,
        const std::vector<std::shared_ptr<const LocalSpaceType>> local_spaces)
    : ms_grid_(ms_grid)
    , local_spaces_(local_spaces)
    , mapper_(std::make_shared<MapperType>(ms_grid_, local_spaces_))
  {
    if (local_spaces_.size() != ms_grid_->size())
      DUNE_THROW_COLORFULLY(Stuff::Exceptions::shapes_do_not_match,
                            "You have to provide a local space for each subdomain of the multiscale grid!\n"
                                << "  Size of the given multiscale grid: "
                                << ms_grid_->size()
                                << "\n"
                                << "  Number of local spaces given: "
                                << local_spaces_.size());
  } // Block(...)

  const std::shared_ptr<const MsGridType>& ms_grid() const
  {
    return ms_grid_;
  }

  const std::vector<std::shared_ptr<const LocalSpaceType>>& local_spaces() const
  {
    return local_spaces_;
  }

  const std::shared_ptr<const GridViewType>& grid_view() const
  {
    return ms_grid_->globalGridView();
  }

  const BackendType& backend() const
  {
    return local_spaces_[0]->backend();
  }

  const MapperType& mapper() const
  {
    return *mapper_;
  }

  BaseFunctionSetType base_function_set(const EntityType& entity) const
  {
    const size_t block = find_block_of_(entity);
    return local_spaces_[block]->base_function_set(entity);
  }

  template <class ConstraintsType>
  void local_constraints(const EntityType& /*entity*/, ConstraintsType& /*ret*/) const
  {
    DUNE_THROW_COLORFULLY(NotImplemented, "I am not sure yet how to implement this!");
  }

  template <class G, class S>
  PatternType compute_pattern(const GridView<G>& /*local_grid_view*/, const SpaceInterface<S>& /*ansatz_space*/) const
  {
    DUNE_THROW_COLORFULLY(NotImplemented, "I am not sure yet how to implement this!");
  }

private:
  template <class EntityType>
  size_t find_block_of_(const EntityType& entity) const
  {
    const auto global_entity_index = ms_grid_->globalGridView()->indexSet().index(entity);
    const auto result              = ms_grid_->entityToSubdomainMap()->find(global_entity_index);
#ifndef NDEBUG
    if (result == ms_grid_->entityToSubdomainMap()->end())
      DUNE_THROW_COLORFULLY(Stuff::Exceptions::internal_error,
                            "Entity " << global_entity_index
                                      << " of the global grid view was not found in the multiscale grid!");
#endif // NDEBUG
    const size_t subdomain = result->second;
#ifndef NDEBUG
    if (subdomain >= ms_grid_->size())
      DUNE_THROW_COLORFULLY(Stuff::Exceptions::internal_error,
                            "The multiscale grid is corrupted!\nIt reports Entity " << global_entity_index
                                                                                    << " to be in subdomain "
                                                                                    << subdomain
                                                                                    << " while only having "
                                                                                    << ms_grid_->size()
                                                                                    << " subdomains!");
#endif // NDEBUG
    assert(subdomain < local_spaces_.size());
    return subdomain;
  } // ... find_block_of_(...)

  std::shared_ptr<const MsGridType> ms_grid_;
  std::vector<std::shared_ptr<const LocalSpaceType>> local_spaces_;
  std::shared_ptr<const MapperType> mapper_;
}; // class Block


} // namespace Spaces
} // namespace GDT
} // namespace Dune

#endif
#endif // DUNE_GDT_SPACES_BLOCK_HH
