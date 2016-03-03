#pragma once

#include "Hash\Hash.hpp"
#include "Comparators.hpp"
#include "Limits.hpp"
#include "Containers\Dictionary.hpp"
#include "Containers\Vector.hpp"
#include "Ranges\Xrange.hpp"
#include <iterator>
#include <algorithm>

namespace Yupei
{
    //https://github.com/mclow/search-library/blob/master/searching.hpp
    namespace Internal
    {      
        template<typename KeyT, typename ValueT, typename Hash, typename Pred, bool Trivial>
        class BMBadSkipTable;

        template<typename KeyT, typename ValueT, typename Hash, typename Pred>
        class BMBadSkipTable<KeyT, ValueT, Hash, Pred, true>
        {
        private:
            using value_type = ValueT;
            using key_type = KeyT;
            using size_type = std::size_t;
            using UnsignedKey = std::make_unsigned_t<key_type>;

        public:
            BMBadSkipTable(size_type n, value_type defaultValue, Hash hf, Pred pred) noexcept
            {
                std::fill_n(table_, static_cast<std::ptrdiff_t>(n), defaultValue);
            }

            void Insert(key_type k, value_type v) noexcept
            {
                table_[static_cast<UnsignedKey>(k)] = v;
            }

            value_type operator[](key_type k) const noexcept
            {
                return table_[static_cast<UnsignedKey>(k)];
            }

        private:
            value_type table_[limits_max_v<UnsignedKey>];
        };

        template<typename KeyT, typename ValueT, typename Hash, typename Pred>
        class BMBadSkipTable<KeyT, ValueT, Hash, Pred, false>
        {
        private:
            using key_type = KeyT;
            using value_type = ValueT;
            using size_type = std::size_t;

            dictionary<KeyT, ValueT, Hash, Pred> table_;
            const value_type defaultValue_;

        public:
            BMBadSkipTable(size_type n, value_type defaultValue, Hash hf, Pred pred)
                :table_(n, hf, pred), defaultValue_{std::move(defaultValue)}
            {}

            void Insert(const key_type& k, const value_type& v)
            {
#ifdef _DEBUG
                const auto res = 
#endif
                    table_.insert(k, v);
                YPASSERT(res.second == true, "Insert failed!");
            }

            value_type operator[](const key_type& k) const
            {
                const auto it = table_.find(k);
                return it == end(table_) ? defaultValue_ : it->first;
            }
        };
    }
    
    //http://igm.univ-mlv.fr/~lecroq/string/node14.html#SECTION00140
    template<typename RandomItT, typename Hash = hash<>, typename Pred = equal_to<>>
    class boyer_moore_searcher: private Hash, private Pred
    {
    public:
        boyer_moore_searcher(RandomItT patStart, RandomItT patLast, Hash hf = {}, Pred pred = {})
            :Hash{std::move(hf)},
            Pred{std::move(pred)},
            patStart_{patStart},
            patLast_{patLast},
            patLen_{patLast - patStart},
            badTable_{static_cast<std::size_t>(patLen_), patLen_, hf, pred},
            goodTable_(static_cast<std::size_t>(patLen_), patLen_)
        {
            if (patLen_ > 1)
            {
                BuildBadTable();
                BuildGoodTable();
            }         
        }

        template<typename RandomIt2T>
        RandomIt2T operator()(RandomIt2T first, RandomIt2T last) const
        {
            const auto dist = std::distance(first, last);
            const auto patLen = patLen_;
            if (patLen == 0 || dist < patLen) return last;
            if (patLen_ == 1)
                return first;//TODO: Naive way to search.
            else
            {
                auto strCur = first + patLen;
                const auto patStart = patStart_; 
                while (strCur != last)
                {
                    auto patCur = patLast_;
                    //Why not std::tie? I don't want to include <tuple>...
                    const auto p = std::mismatch(std::make_reverse_iterator(patCur), std::make_reverse_iterator(patStart_),
                        std::make_reverse_iterator(strCur), std::make_reverse_iterator(std::prev(strCur, patLen)), EqualFunc());
                    patCur = p.first.base();
                    strCur = p.second.base();
                    if (patCur == patStart) break;
                    const auto cur = patCur - patStart;
                    strCur += std::max(badTable_[cur], goodTable_[cur]);
                }
                return strCur;
            }
        }
        
    private:
        void BuildBadTable()
        {
            //Easy one!
            for (auto i : xrange(patLen_))
                badTable_.Insert(patStart_[i], patLen_ - i - 1);
        }

        Hash HashFunc() noexcept
        {
            return static_cast<Hash&>(*this);
        }

        Pred EqualFunc() const noexcept
        {
            return static_cast<const Pred&>(*this);
        }

        using ValueType = value_type_t<RandomItT>;
        using DifferenceType = std::ptrdiff_t;
        using BadTable = Internal::BMBadSkipTable<ValueType, DifferenceType, Hash, Pred, 
            (std::is_same<Pred, equal_to<void>>::value || std::is_same<Pred, equal_to<ValueType>>::value) &&
                sizeof(ValueType) == 1 && (std::is_integral<ValueType>::value || std::is_enum<ValueType>::value)>;
        
        const RandomItT patStart_, patLast_;
        const DifferenceType patLen_;
        BadTable badTable_;
        vector<DifferenceType> goodTable_;

        void BuildGoodTable()
        {
            const auto suffixTable = BuildSuffix();
            const auto patLen = patLen_;
            //保证不交叉
            DifferenceType cur{};
            for (auto i = patLen - 1; i >= 0; --i)
            {
                if (suffixTable[i] == i + 1)
                    for (; cur < patLen - i - 1; ++cur)
                        goodTable_[i] = i + 1;
            }
            for (auto i : xrange(goodTable_.size()))
                goodTable_[patLen - suffixTable[i] - 1] = patLen - 1 - i;
        }

        vector<DifferenceType> BuildSuffix()
        {
            const auto patLen = patLen_;
            vector<DifferenceType> suffixTable(static_cast<std::size_t>(patLen));
            suffixTable[patLen - 1] = patLen;
            DifferenceType prev = patLen - 1;
            auto last = prev;
            for (auto i = patLen - 2; i >= 0; --i)
            {
                const auto& c = suffixTable[i + patLen - 1 - last];
                if (prev < last && c < i - prev)
                    suffixTable[i] = c;
                else
                {
                    prev = last = i;
                    const auto p = std::mismatch(std::make_reverse_iterator(patStart_ + i),
                        std::make_reverse_iterator(patStart_), 
                        std::make_reverse_iterator(patLast_), 
                        std::make_reverse_iterator(patLast_ - i), EqualFunc());
                    suffixTable[i] = (patStart_ + i - p.first.base()) + i;
                }
            }
            return suffixTable;
        }
    };

    template<typename RandomItT, typename Hash = hash<>, typename Pred = equal_to<>>
    boyer_moore_searcher<RandomItT, Hash, Pred> 
        make_boyer_moore_searcher(RandomItT patFirst, RandomItT patLast, Hash hf = {}, Pred pred = {})
    {
        return {patFirst, patLast, hf, pred};
    }
}