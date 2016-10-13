#pragma once

#include "Hash\Hash.hpp"
#include "Limits.hpp"
#include "Containers\Dictionary.hpp"
#include "Containers\Vector.hpp"
#include "Ranges\Xrange.hpp"
#include <utility>
#include <cassert>
#include <iterator>
#include <algorithm>

namespace Yupei
{
    //http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0253r0.pdf

    template<typename ForwardIt1T, typename Pred = std::equal_to<>>
    class default_searcher : Pred
    {
    public:
        default_searcher(ForwardIt1T patFirst, ForwardIt1T patLast, Pred pred = {}) noexcept
            :patFirst_{patFirst}, patLast_{patLast}, Pred{pred}
        {}

        template<typename ForwardIt2T>
        std::pair<ForwardIt2T, ForwardIt2T> 
            operator()(ForwardIt2T first, ForwardIt2T last) const
        {
            if (first == last) return {first, last};
            const auto firstChar = *patFirst_;
            const auto patNext = std::next(patFirst_);
            const auto patLast = patLast_;
            for (;;)
            {
                first = std::find(first, last, firstChar, GetPred());
                if (first == last) return {first, last};
                else
                {
                    const auto searchStart = std::next(first);
                    const auto misMatch = std::mismatch(patNext, patLast, searchStart, last, GetPred());
                    if (misMatch.first == patLast)
                        return {first, misMatch.second};
                }
            }
        }

    private:
        ForwardIt1T patFirst_, patLast_;

        const Pred& GetPred() const noexcept
        {
            return static_cast<const Pred&>(*this);
        }
    };


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
            BMBadSkipTable(size_type, value_type defaultValue, Hash, Pred) noexcept
            {
                std::fill_n(table_, limits_max_v<UnsignedKey>, defaultValue);
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

            //未使用的变量 res，等 [[maybe_unused]]。
            void Insert(const key_type& k, const value_type& v)
            {
#ifdef _DEBUG
                const auto res = 
#endif
                    table_.insert({k, v});
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
    template<typename RandomItT, typename Hash = hash<>, typename Pred = std::equal_to<>>
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
        std::pair<RandomIt2T, RandomIt2T> operator()(RandomIt2T first, RandomIt2T last) const
        {
            assert(first <= last);
            const auto dist = last - first;
            const auto patLen = patLen_;
            if (patLen == 0 || dist < patLen) return {last, last};
            if (patLen_ == 1)
            {
                const auto charToFind = *patStart_;
                const auto it = std::find_if(first, last, [&](const auto c) noexcept
                {
                    return EqualFunc()(c, charToFind);
                });
                if (it == last) return {last, last};
                return {it, it + 1};
            }
            else
            {
                auto strCur = patLen - 1;
                const auto patStart = patStart_; 
                while (strCur < dist)
                {
                    auto patCur = patLen - 1;
                    while (patCur >= 0 && EqualFunc()(patStart[patCur], first[strCur]))
                        --patCur, --strCur;
                    if (patCur < 0) break;          
                    const auto bad = badTable_[first[strCur]] - (patLen - 1 - patCur);
                    const auto good = goodTable_[patCur];
                    strCur += std::max(bad, good);
                }                                
                if (strCur >= dist) return {last, last};
                const auto target = first + strCur + 1;
                return {target, target + patLen};
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

        using ValueType = iterator_value_type_t<RandomItT>;
        using DifferenceType = std::ptrdiff_t;
        using BadTable = Internal::BMBadSkipTable<ValueType, DifferenceType, Hash, Pred, 
            (std::is_same<Pred, std::equal_to<void>>::value || std::is_same<Pred, std::equal_to<ValueType>>::value) &&
                sizeof(ValueType) == 1 && (std::is_integral<ValueType>::value || std::is_enum<ValueType>::value)>;
        
        const RandomItT patStart_, patLast_;
        const DifferenceType patLen_;
        BadTable badTable_;
        vector<DifferenceType> goodTable_;

        void BuildGoodTable()
        {
            const auto suffixTable = BuildSuffix();
            const auto lastIndex = patLen_ - 1;                 
            DifferenceType cur{};
            for (auto i = lastIndex; i >= 0; --i)
            {
                if (suffixTable[i] == i + 1)
                    for (; cur < lastIndex - i; ++cur)  //保证不交叉
                        if(goodTable_[cur] == patLen_)  //遇到前缀以非前缀为准，所以仅当该 cell 没有被更新时更新。
                            goodTable_[cur] = lastIndex - i;
            }
            for (auto i : xrange(goodTable_.size() - 1))
                goodTable_[lastIndex - suffixTable[i]] = lastIndex - i + suffixTable[i];
        }

        vector<DifferenceType> BuildSuffix()
        {
            const auto lastIndex = patLen_ - 1;
            vector<DifferenceType> suffixTable(static_cast<std::size_t>(patLen_));
            suffixTable[lastIndex] = patLen_;
            auto prev = lastIndex;
			DifferenceType last = {};
            for (auto i = lastIndex - 1; i >= 0; --i)
            {
                if (i > prev && suffixTable[i + lastIndex - last] < i - prev)
                    suffixTable[i] = suffixTable[i + lastIndex - last];
                else
                {
                    if (i < prev)
                        prev = i;
                    last = i;
                    while (prev >= 0 && EqualFunc()(patStart_[prev], patStart_[prev + lastIndex - last]))
                        --prev;
                    suffixTable[i] = last - prev;
                }
            }
            return suffixTable;
        }
    };

    template<typename RandomItT, typename Hash = hash<>, typename Pred = std::equal_to<>>
    boyer_moore_searcher<RandomItT, Hash, Pred> 
        make_boyer_moore_searcher(RandomItT patFirst, RandomItT patLast, Hash hf = {}, Pred pred = {})
    {
        return {patFirst, patLast, hf, pred};
    }
}