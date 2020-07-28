#pragma once
#include "types.hpp"
#include "span.hpp"
#include "string.hpp"

namespace fancy_status
{
    inline namespace prelude
    {
        struct Block_List;
        struct Single_Block;

        // Compared to Vectors, a Block_List is better in
        // - Appending data will not create spikes (real-time friendlier).
        // - Can be optimized to an infinitely large vector via mmap.
        struct Block_List final
        {
            ~Block_List() noexcept;
            Block_List(Block_List&&) noexcept;
            auto operator = (Block_List&&) noexcept -> Block_List&;

            Block_List() noexcept {}
            // block_byte_size: Must be a power-of-two. Must be multiples of 4 KiB.
            // The alignment of new blocks will also be block_byte_size.
            Block_List(size_t block_byte_size) noexcept;

            auto block_byte_size() const noexcept -> size_t;
            auto empty() const noexcept -> bool;            // There are no blocks.
            auto solid() const noexcept -> bool;            // There is exactly 1 block.
            auto empty_or_solid() const noexcept -> bool;   // There are less than 2 blocks.

            // Merge all blocks into 1 single big block that is exactly multiples of block_byte_size
            // that can just fit all the data in.
            // When solid() returns true, it is guaranteed that pointers into
            // the allocated memory won't be invalidated.
            auto consolidate() && noexcept -> Single_Block;

            // Equivalent to:
            // > auto block = std::move(*this).consolidate();
            // > auto result = block.data();
            // > *this = std::move(block).disperse();
            // > return result;
            auto consolidate_for_data() const& noexcept -> Buffer_Slice;
            auto consolidate_for_data() && noexcept -> Buffer_Slice = delete;

            // Allocate a continuous memory inside blocks.
            // When the `size` is larger than a block, a block of
            // least multiples of block_byte_size will be allocated instead.
            // `alignment` MUST divide block_byte_size cleanly.
            auto alloc_bytes_back(size_t size, size_t alignment=8) const noexcept -> Buffer_Slice;

            template <class T>
            auto alloc_back(size_t n=1) const noexcept -> Span<T>
            {
                return alloc_bytes_back(sizeof(T) * n, alignof(T)).cast<T>();
            }

            // Move blocks from the `source` into this.
            // Blocks are put to the back.
            // Block size and block alignment information are lost.
            // After splicing, new allocation may use the free space
            // of the last block from the `source`.
            auto splice_back(Block_List&& source) const noexcept -> void;

        private:
            struct Impl;
            mutable Impl* impl{};   // owner

            // impl shall transfer ownership.
            Block_List(Impl* impl) noexcept: impl{impl} {}

            friend Single_Block;
        };

        struct Single_Block final
        {
            ~Single_Block() noexcept;
            Single_Block(Single_Block&&) noexcept;
            auto operator = (Single_Block&&) noexcept -> Single_Block&;

            Single_Block() noexcept {}

            auto block_byte_size() const noexcept -> size_t;

            // Move the single block into a block list.
            // It is guaranteed that pointers into data() won't be invalidated.
            // `disperse().solid()` guarantees to return true.
            auto disperse() && noexcept -> Block_List;

            auto data() const noexcept -> Buffer_Slice;
            auto clone() const noexcept -> Single_Block;

        private:
            mutable Block_List::Impl* impl{};   // owner

            // impl shall transfer ownership.
            Single_Block(Block_List::Impl* impl) noexcept: impl{impl} {}

            friend Block_List;
        };
    }
}

