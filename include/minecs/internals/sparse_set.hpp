#pragma once

#include <limits>
#include <type_traits>
#include <vector>

namespace minecs
{
    template <typename T, typename U>
    requires std::is_unsigned_v<U>
    class sparse_set
    {
    public:
        using type = T;
        using size_type = U;

        using dense_type = std::vector<type>;
        using sparse_type = std::vector<size_type>;

        using iterator = typename dense_type::iterator;
        using const_iterator = typename dense_type::const_iterator;

        inline sparse_set() = default;
        inline ~sparse_set() = default;

        inline sparse_set(const sparse_set&) = default;
        inline sparse_set& operator=(const sparse_set&) = default;

        inline sparse_set(sparse_set&&) noexcept = default;
        inline sparse_set& operator=(sparse_set&&) noexcept = default;

        static constexpr size_type dead_index = std::numeric_limits<size_type>::max();

        [[nodiscard]] inline bool insert(size_type index, const T& element)
        {
            if (index >= m_sparse.size())
            {
                m_sparse.resize(index + 1, dead_index);
            }

            if (m_sparse[index] != dead_index)
            {
                return false;
            }

            m_sparse[index] = m_dense.size();

            m_dense.push_back(element);
            m_dense_to_index.push_back(index);

            return true;
        }

        template <typename... Args>
        [[nodiscard]] inline bool insert(size_type index, Args&&... args)
        {
            if (index >= m_sparse.size())
            {
                m_sparse.resize(index + 1, dead_index);
            }

            if (m_sparse[index] != dead_index)
            {
                return false;
            }

            m_sparse[index] = m_dense.size();

            m_dense.emplace_back(std::forward<Args>(args)...);
            m_dense_to_index.push_back(index);

            return true;
        }

        [[nodiscard]] inline bool remove(size_type index)
        {
            if (index >= m_sparse.size())
            {
                return false;
            }

            if (m_sparse[index] == dead_index)
            {
                return false;
            }

            size_type dense_index = m_sparse[index];
            size_type last_index = static_cast<size_type>(m_dense.size() - 1);

            if (dense_index == last_index)
            {
                m_sparse[index] = dead_index;

                m_dense.pop_back();
                m_dense_to_index.pop_back();

                return true;
            }

            size_type last_entity = m_dense_to_index[last_index];

            m_dense[dense_index] = std::move(m_dense[last_index]);
            m_dense_to_index[dense_index] = last_entity;
            m_sparse[last_entity] = dense_index;

            m_sparse[index] = dead_index;

            m_dense.pop_back();
            m_dense_to_index.pop_back();

            return true;
        }

        [[nodiscard]] inline type* get(size_type index)
        {
            if (index >= m_sparse.size())
            {
                return nullptr;
            }

            if (m_sparse[index] == dead_index)
            {
                return nullptr;
            }

            return &m_dense[m_sparse[index]];
        }

        [[nodiscard]] inline const type* get(size_type index) const
        {
            if (index >= m_sparse.size())
            {
                return nullptr;
            }

            if (m_sparse[index] == dead_index)
            {
                return nullptr;
            }

            return &m_dense[m_sparse[index]];
        }

        [[nodiscard]] inline bool contains(size_type index) const
        {
            if (index >= m_sparse.size())
            {
                return false;
            }

            if (m_sparse[index] == dead_index)
            {
                return false;
            }

            return true;
        }

        [[nodiscard]] inline iterator begin() noexcept
        {
            return m_dense.begin();
        }

        [[nodiscard]] inline const_iterator begin() const noexcept
        {
            return m_dense.begin();
        }

        [[nodiscard]] inline iterator end() noexcept
        {
            return m_dense.end();
        }

        [[nodiscard]] inline const_iterator end() const noexcept
        {
            return m_dense.end();
        }

        [[nodiscard]] inline dense_type& get_dense()
        {
            return m_dense;
        }

        [[nodiscard]] inline const dense_type& get_dense() const
        {
            return m_dense;
        }

        [[nodiscard]] inline sparse_type& get_sparse()
        {
            return m_sparse;
        }

        [[nodiscard]] inline const sparse_type& get_sparse() const
        {
            return m_sparse;
        }

    private:
        dense_type m_dense;
        sparse_type m_sparse;

        std::vector<size_type> m_dense_to_index;
    };
}