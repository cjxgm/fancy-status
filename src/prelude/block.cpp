#include "block.hpp"
#include "utility.hpp"
#include <new>

namespace fancy_status
{
    inline namespace prelude
    {
        namespace
        {
            struct Block_Head final: Pinned
            {
                Block_Head* next{};     // owned by Block_List::Impl
                byte* block{};          // owned by Block_Head
                Buffer_Slice free_space{};
                size_t first_object_alignment{};

                ~Block_Head() noexcept
                {
                    delete[] block;
                }

                Block_Head(size_t block_size, size_t block_alignment, size_t content_alignment) noexcept
                    : block{new (std::align_val_t(block_alignment)) byte[block_size]{}}
                    , free_space{block, block_size}
                    , first_object_alignment{content_alignment}
                {}

                auto used_space()       noexcept -> Buffer_Slice { return {block, free_space.begin()}; }
                auto used_space() const noexcept -> Buffer_View  { return {block, free_space.begin()}; }

                auto used_size() const noexcept -> size_t { return used_space().size(); }
            };

            inline constexpr auto default_block_byte_size = 4 * 1024;
        }

        struct Block_List::Impl final: Pinned
        {
            size_t block_byte_size{};
            Block_Head* first_block{};   // owner
            Block_Head** next_new_block{&first_block};

            ~Impl() noexcept
            {
                while (first_block) delete exchange(first_block, first_block->next);
            }

            Impl(size_t block_byte_size=default_block_byte_size, Block_Head* first_block=nullptr) noexcept
                : block_byte_size{block_byte_size}
                , first_block{first_block}
            {}

            // Assumptions:
            // - size > 0.
            // - alignment is a power-of-two.
            // - block_byte_size >= alignment.
            auto alloc_bytes_back(size_t size, size_t alignment) noexcept -> Buffer_Slice
            {
                auto free_block = *next_new_block;
                auto requires_new_block = (
                    false
                    || free_block == nullptr
                    || free_block->free_space.align_first(alignment).size() < size
                );

                if (requires_new_block) {
                    auto new_block_size = align_uint(size, block_byte_size);
                    free_block = new Block_Head{new_block_size, block_byte_size, alignment};
                    append_solid_block(free_block);
                }

                auto aligned_free_space = free_block->free_space.align_first(alignment);
                free_block->free_space = aligned_free_space.remove_prefix(size);
                auto allocated_space = aligned_free_space.prefix(size);

                return allocated_space;
            }

            // Assumes source->first_block != nullptr.
            auto splice_back(Impl* source) noexcept -> void
            {
                auto head = exchange(source->first_block, nullptr);
                source->next_new_block = &source->first_block;

                append_blocks(head);
            }

            // head shall transfer ownership.
            // head->next should be nullptr, otherwise next_new_block will point to a wrong place.
            auto append_solid_block(Block_Head* head) noexcept -> void
            {
                while (*next_new_block) next_new_block = &next_new_block[0]->next;
                *next_new_block = head;
            }

            // head shall transfer ownership.
            auto append_blocks(Block_Head* head) noexcept -> void
            {
                append_solid_block(head);
                // The above is OK even if head->next is not nullptr (not solid),
                // because we will fix next_new_block below.
                while (*next_new_block && next_new_block[0]->next) next_new_block = &next_new_block[0]->next;
            }

            // Assumes first_block != nullptr.
            auto merge_blocks() const noexcept -> Block_Head*
            {
                auto new_block_size = align_uint(content_size(), block_byte_size);
                auto merged_block = new Block_Head{new_block_size, block_byte_size, first_block->first_object_alignment};
                auto& free_space = merged_block->free_space;

                for (auto block=first_block; block; block=block->next) {
                    free_space = free_space.align_first(block->first_object_alignment);
                    free_space = free_space.remove_prefix(copy(block->used_space(), free_space));
                }

                return merged_block;
            }

            auto content_size() const noexcept -> size_t
            {
                auto size = size_t(0);
                for (auto block=first_block; block; block=block->next) {
                    size = align_uint(size, block->first_object_alignment);
                    size += block->used_size();
                }
                return size;
            }

            static auto empty(Impl const* x) noexcept -> bool
            {
                return (x == nullptr || x->first_block == nullptr);
            }

            static auto solid(Impl const* x) noexcept -> bool
            {
                return (x && x->first_block && x->first_block->next == nullptr);
            }

            static auto empty_or_solid(Impl const* x) noexcept -> bool
            {
                return (x == nullptr || x->first_block == nullptr || x->first_block->next == nullptr);
            }
        };

        Block_List::~Block_List() noexcept
        {
            delete impl;
        }

        Block_List::Block_List(Block_List&& x) noexcept
            : impl{exchange(x.impl, nullptr)}
        {}

        auto Block_List::operator = (Block_List&& x) noexcept -> Block_List&
        {
            if (this != &x) delete exchange(impl, exchange(x.impl, nullptr));
            return *this;
        }

        Block_List::Block_List(size_t block_byte_size) noexcept
            : impl{new Impl{block_byte_size}}
        {}

        auto Block_List::block_byte_size() const noexcept -> size_t
        {
            return (impl == nullptr ? default_block_byte_size : impl->block_byte_size);
        }

        auto Block_List::empty() const noexcept -> bool
        {
            return Impl::empty(impl);
        }

        auto Block_List::solid() const noexcept -> bool
        {
            return Impl::solid(impl);
        }

        auto Block_List::empty_or_solid() const noexcept -> bool
        {
            return Impl::empty_or_solid(impl);
        }

        auto Block_List::consolidate() && noexcept -> Single_Block
        {
            if (empty_or_solid())
                return exchange(impl, nullptr);

            return new Impl{
                impl->block_byte_size,
                impl->merge_blocks(),
            };
        }

        auto Block_List::consolidate_for_data() const& noexcept -> Buffer_Slice
        {
            if (empty())
                return {};

            if (!solid())
                delete exchange(impl, new Impl{impl->block_byte_size, impl->merge_blocks()});

            return impl->first_block->used_space();
        }

        auto Block_List::alloc_bytes_back(size_t size, size_t alignment) const noexcept -> Buffer_Slice
        {
            if (size == 0)
                return {};

            if (impl == nullptr)
                impl = new Impl;

            return impl->alloc_bytes_back(size, alignment);
        }

        auto Block_List::splice_back(Block_List&& source) const noexcept -> void
        {
            if (source.empty())
                return;

            if (impl == nullptr)
                impl = new Impl;

            impl->splice_back(source.impl);
        }

        Single_Block::~Single_Block() noexcept
        {
            delete impl;
        }

        Single_Block::Single_Block(Single_Block&& x) noexcept
            : impl{exchange(x.impl, nullptr)}
        {}

        auto Single_Block::operator = (Single_Block&& x) noexcept -> Single_Block&
        {
            if (this != &x) delete exchange(impl, exchange(x.impl, nullptr));
            return *this;
        }

        auto Single_Block::block_byte_size() const noexcept -> size_t
        {
            return (impl == nullptr ? default_block_byte_size : impl->block_byte_size);
        }

        auto Single_Block::disperse() && noexcept -> Block_List
        {
            return exchange(impl, nullptr);
        }

        auto Single_Block::data() const noexcept -> Buffer_Slice
        {
            if (Block_List::Impl::empty(impl))
                return {};

            return {
                impl->first_block->block,
                impl->first_block->free_space.begin(),
            };
        }

        auto Single_Block::clone() const noexcept -> Single_Block
        {
            if (impl == nullptr)
                return {};

            if (Block_List::Impl::empty(impl))
                return new Block_List::Impl{impl->block_byte_size};

            return new Block_List::Impl{
                impl->block_byte_size,
                impl->merge_blocks(),
            };
        }
    }
}

