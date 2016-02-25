#pragma once

#include "..\Hash\Hash.hpp"
#include "..\Comparators.hpp"
#include "..\Iterator.hpp"
#include "..\SmartPointers\UniquePtr.hpp"
#include "..\MemoryResource\MemoryResource.hpp"
#include "..\Assert.hpp"
#include "..\Utility.hpp"
#include "..\Hash\HashHelpers.hpp"
#include "..\ConstructDestruct.hpp"
#include "..\Algorithm\ForEach.hpp"
#include <cstdint>
#include <algorithm>

namespace Yupei
{
    template<typename KeyT, typename ValueT, typename HashFun, typename KeyEqualT>
    class dictionary;

    namespace Internal
    {
        template<typename DictionaryT>
        class DictionaryIterator
        {
            template<typename, typename, typename, typename>
            friend class Yupei::dictionary;

            template<typename>
            friend class DictionaryConstIterator;

            DictionaryIterator(DictionaryT* dict, size_type_t<DictionaryT> index) noexcept
                :dict_{dict}, index_{index}
            {}

        public:
            using difference_type = std::ptrdiff_t;
            using value_type = value_type_t<DictionaryT>;
            using iterator_category = std::forward_iterator_tag;
            using pointer = value_type*;
            using reference = value_type&;

            constexpr DictionaryIterator() noexcept
                :dict_{}, index_{}
            {}

            DictionaryIterator& operator++() noexcept;

            DictionaryIterator operator++(int) noexcept
            {
                auto tmp = *this;
                ++*this;
                return tmp;
            }

            value_type& operator*() noexcept
            {
                return dict_->entries_[index_].KeyValue_;
            }

            pointer operator->() noexcept
            {
                return std::addressof(this->operator*());
            }

            bool operator==(const DictionaryIterator& other) const noexcept
            {
                YPASSERT(dict_ == other.dict_, "Two iterators are not compatible!");
                return index_ == other.index_;
            }

            bool operator!=(const DictionaryIterator& other) const noexcept
            {
                return !(*this == other);
            }
            
        private:
            DictionaryT* dict_;
            size_type_t<DictionaryT> index_;
        };

        template<typename DictionaryT>
        class DictionaryConstIterator
        {
            template<typename, typename, typename, typename>
            friend class Yupei::dictionary;

            DictionaryConstIterator(DictionaryT* dict, size_type_t<DictionaryT> index) noexcept
                :dict_{dict}, index_{index}
            {}
          
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = value_type_t<DictionaryT>;
            using iterator_category = std::forward_iterator_tag;
            using pointer = const value_type*;
            using reference = const value_type&;

            constexpr DictionaryConstIterator() noexcept
                :dict_{}, index_{}
            {}

            DictionaryConstIterator(const DictionaryIterator<DictionaryT>& it) noexcept
                : dict_{it.dict_}, index_{it.index_}
            {}

            DictionaryConstIterator& operator++() noexcept;

            DictionaryConstIterator operator++(int) noexcept
            {
                auto tmp = *this;
                ++*this;
                return tmp;
            }

            reference operator*() noexcept
            {
                return dict_->entries_[index_].KeyValue_;
            }

            pointer operator->() noexcept
            {
                return std::addressof(this->operator*());
            }

            bool operator==(const DictionaryConstIterator& other) const noexcept
            {
                YPASSERT(dict_ == other.dict_, "Two iterators are not compatible!");
                return index_ == other.index_;
            }

            bool operator!=(const DictionaryConstIterator& other) const noexcept
            {
                return !(*this == other);
            }

        private:
            DictionaryT* dict_;
            size_type_t<DictionaryT> index_;
        };

        template<typename DictionaryT>
        class DictionaryLocalIterator
        {
            template<typename, typename, typename, typename>
            friend class Yupei::dictionary;

            DictionaryLocalIterator(DictionaryT* dict, size_type_t<DictionaryT> index) noexcept
                :dict_{dict}, index_{index}
            {}

        public:
            using difference_type = std::ptrdiff_t;
            using value_type = value_type_t<DictionaryT>;
            using iterator_category = std::forward_iterator_tag;
            using pointer = value_type*;
            using reference = value_type&;

            constexpr DictionaryLocalIterator() noexcept
                :dict_{}, index_{}
            {}

            DictionaryLocalIterator& operator++() noexcept;

            DictionaryLocalIterator operator++(int) noexcept
            {
                auto tmp = *this;
                ++*this;
                return tmp;
            }

            reference operator*() noexcept
            {
                return dict_->entries_[index_].KeyValue_;
            }

            pointer operator->() noexcept
            {
                return std::addressof(this->operator*());
            }

            bool operator==(const DictionaryLocalIterator& other) const noexcept
            {
                YPASSERT(dict_ == other.dict_, "Two iterators are not compatible!");
                return index_ == other.index_;
            }

            bool operator!=(const DictionaryLocalIterator& other) const noexcept
            {
                return !(*this == other);
            }

        private:
            DictionaryT* dict_;
            size_type_t<DictionaryT> index_;
        };

        template<typename DictionaryT>
        class DictionaryConstLocalIterator
        {
            template<typename, typename, typename, typename>
            friend class Yupei::dictionary;

            DictionaryConstLocalIterator(DictionaryT* dict, size_type_t<DictionaryT> index) noexcept
                :dict_{dict}, index_{index}
            {}

        public:
            using difference_type = std::ptrdiff_t;
            using value_type = value_type_t<DictionaryT>;
            using iterator_category = std::forward_iterator_tag;
            using pointer = const value_type*;
            using reference = const value_type&;

            constexpr DictionaryConstLocalIterator() noexcept
                :dict_{}, index_{}
            {}

            DictionaryConstLocalIterator(const DictionaryLocalIterator<DictionaryT>& it) noexcept
                : dict_{it.dict_}, index_{it.index_}
            {}

            DictionaryConstLocalIterator& operator++() noexcept;

            DictionaryConstLocalIterator operator++(int) noexcept
            {
                auto tmp = *this;
                ++*this;
                return tmp;
            }

            reference operator*() noexcept
            {
                return dict_->entries_[index_].KeyValue_;
            }

            pointer operator->() noexcept
            {
                return std::addressof(this->operator*());
            }

            bool operator==(const DictionaryConstLocalIterator& other) const noexcept
            {
                YPASSERT(dict_ == other.dict_, "Two iterators are not compatible!");
                return index_ == other.index_;
            }

            bool operator!=(const DictionaryConstLocalIterator& other) const noexcept
            {
                return !(*this == other);
            }

        private:
            DictionaryT* dict_;
            size_type_t<DictionaryT> index_;
        };
    }

    template<typename KeyT, typename ValueT, typename HashFun, typename KeyEqualT>
    class dictionary : KeyEqualT, HashFun
    {
    public:
        using key_type = KeyT;
        using mapped_type = ValueT;
        using value_type = Yupei::pair<key_type, mapped_type>;
        using size_type = std::size_t;
        using allocator_type = polymorphic_allocator<value_type>;
        using key_equal = KeyEqualT;
        using hasher = HashFun;
        using iterator = Internal::DictionaryIterator<dictionary>;
        using const_iterator = Internal::DictionaryConstIterator<dictionary>;
        using local_iterator = Internal::DictionaryLocalIterator<dictionary>;
        using const_local_iterator = Internal::DictionaryConstLocalIterator<dictionary>;

    private:
        template<typename>
        friend class Internal::DictionaryIterator;

        static constexpr size_type kNothing = static_cast<size_type>(-1);

        struct Entry
        {
            size_type HashCode_ = kNothing;
            size_type NextEntryIndex_;
            value_type KeyValue_;
        };

    public:
        dictionary()
            :dictionary{0}
        {}

        explicit dictionary(size_type bucketCount, hasher hash = {}, key_equal keyEqual = {}, memory_resource_ptr pmr = {})
            :hasher{hash},
            key_equal{keyEqual},
            allocator_{pmr}
        {
            Initialize(bucketCount);
        }

        dictionary(const dictionary& other)
            :key_equal{other.key_eq()},
            hasher{other.hash_function()}
        {
            Initialize(other.bucketCount_);
            for (const auto& v : other)
                insert(v);
        }

        dictionary(dictionary&& other) noexcept
            :key_equal{other.key_eq()},
            hasher{other.hash_function()},
            freeList_{other.freeList_},
            freeCount_{other.freeCount_},
            count_{other.count_},
            bucketCount_{other.bucketCount_},
            entries_{std::move(other.entries_)},
            buckets_{std::move(other.buckets_)}
        {
            other.freeList_ = kNothing;
            other.freeCount_ = {};
            other.count_ = {};
            other.bucketCount_ = {};
        }

        dictionary& operator=(const dictionary& other)
        {
            dictionary(other).swap(*this);
            return *this;
        }

        dictionary& operator=(dictionary&& other) noexcept
        {
            dictionary(std::move(other)).swap(*this);
            return *this;
        }

        ~dictionary()
        {
            clear();
        }

        void swap(dictionary& other) noexcept
        {
            using std::swap;
            swap(freeList_, other.freeList_);
            swap(freeCount_, other.freeCount_);
            swap(count_, other.count_);
            swap(bucketCount_, other.bucketCount_);
            swap(entries_, other.entries_);
            swap(buckets_, other.buckets_);
        }

        allocator_type get_allocator() const noexcept
        {
            return static_cast<allocator_type>(*this);
        }

        size_type bucket_count() const noexcept
        {
            return bucketCount_;
        }

        float load_factor() const noexcept
        {
            return static_cast<float>(size()) /
                static_cast<float>(bucket_count());
        }

        hasher hash_function() const
        {
            return static_cast<hasher>(*this);
        }

        key_equal key_eq() const
        {
            return static_cast<key_equal>(*this);
        }

        size_type size() const noexcept
        {
            return count_ - freeCount_;
        }

        float max_load_factor() const noexcept
        {
            return 1.f;
        }

        //WARNING: This is not O(1).
        iterator begin() noexcept
        {            
            return {this, FindFirstNonEmptyEntry()};
        }

        //WARNING: This is not O(1).
        const_iterator begin() const noexcept
        {
            return {this, FindFirstNonEmptyEntry()};
        }

        //WARNING: This is not O(1).
        const_iterator cbegin() const noexcept
        {
            return begin();
        }
        
        iterator end() noexcept
        {
            return {this, count_};
        }

        const_iterator end() const noexcept
        {
            return {this, count_};
        }

        const_iterator cend() const noexcept
        {
            return end();
        }

        local_iterator begin(size_type n) noexcept
        {
            return {this, buckets_[n]};
        }

        const_local_iterator begin(size_type n) const noexcept
        {
            return {this, buckets_[n]};
        }

        const_local_iterator cbegin(size_type n) const noexcept
        {
            return begin(n);
        }

        local_iterator end(size_type n) noexcept
        {
            return {this, kNothing};
        }

        const_local_iterator end(size_type n) const noexcept
        {
            return {this, kNothing};
        }

        const_local_iterator cend(size_type n) const noexcept
        {
            return end(n);
        }

        iterator erase(const_iterator pos)
        {
            YPASSERT(pos.index_ != kNothing, "Erase an end iterator!");
            const auto ret = std::next(pos);
            const auto index = pos.index_;
            auto& entry = entries_[index];
            const auto targetBucket = ConstrainHash(entry.HashCode_);
            auto last = kNothing;
            for (size_type i = buckets_[targetBucket]; i != index; last = i, i = entries_[i].NextEntryIndex_)
                ;
            if (last == kNothing)
                buckets_[targetBucket] = entry.NextEntryIndex_;
            else
                entries_[last].NextEntryIndex_ = entry.NextEntryIndex_;
            entry.HashCode_ = kNothing;
            entry.NextEntryIndex_ = freeList_;
            freeList_ = index;
            ++freeCount_;
            destroy(std::addressof(entry.KeyValue_));
            return {ret.dict_, ret.index_};
        }

        bool erase(const key_type& key)
        {
            const auto hashCode = hash_function()(key);
            const auto targetBucket = ConstrainHash(hashCode);
            auto last = kNothing;
            if (buckets_)
            {
                for (size_type i = buckets_[targetBucket]; i != kNothing; last = i, i = entries_[i].NextEntryIndex_)
                {
                    auto& entry = entries_[i];
                    if (entry.HashCode_ == hashCode && key_eq()(key, entry.KeyValue_.first))
                    {
                        if (last == kNothing)
                            buckets_[targetBucket] = entry.NextEntryIndex_;
                        else
                            entries_[last].NextEntryIndex_ = entry.NextEntryIndex_;

                        entry.HashCode_ = kNothing;
                        entry.NextEntryIndex_ = freeList_;
                        freeList_ = index;
                        ++freeCount_;
                        destroy(std::addressof(entry.KeyValue_));
                        return true;
                    }
                }
            }
            return false;
        }

        mapped_type& at(const key_type& key)
        {
            const auto i = FindEntry(key);
            if (i == kNothing) throw std::out_of_range("Key doesn't exist!");
            return entries_[i].KeyValue_.second;
        }

        const mapped_type& at(const key_type& key) const
        {
            const auto i = FindEntry(key);
            if (i == kNothing) throw std::out_of_range("Key doesn't exist!");
            return entries_[i].KeyValue_.second;
        }

        mapped_type& operator[](const key_type& key)
        {
            return try_emplace(key).first->second;
        }

        mapped_type& operator[](key_type&& key)
        {
            return try_emplace(std::move(key)).first->second;
        }

        pair<iterator, bool> insert(const value_type& value)
        {
            return Insert(true, value.first, value.second);
        }

        pair<iterator, bool> insert(value_type&& value)
        {
            return Insert(true, std::move(value.first), std::move(value.second));
        }

        template<typename M>
        pair<iterator, bool> insert_or_assign(const key_type& k, M&& obj)
        {
            return Insert(false, k, std::forward<M>(obj));
        }

        template<typename M>
        pair<iterator, bool> insert_or_assign(key_type&& k, M&& obj)
        {
            return Insert(false, std::move(k), std::forward<M>(obj));
        }

        template<typename... Args>
        pair<iterator, bool> try_emplace(const key_type& k, Args&&... args)
        {
            return Insert(true, k, std::forward<Args>(args)...);
        }

        template<typename... Args>
        pair<iterator, bool> try_emplace(key_type&& k, Args&&... args)
        {
            return Insert(true, std::move(k), std::forward<Args>(args)...);
        }

        void clear() noexcept
        {
            const auto entries = entries_.get();
            std::for_each(entries, entries + count_, [](Entry& entry) {
                destroy(std::addressof(entry.KeyValue_));
                entry.HashCode_ = kNothing;
            });
            count_ = {};
            freeList_ = kNothing;
            freeCount_ = {};
        }

    private:
        polymorphic_allocator<size_type> GetSizeTypeAllocator() const noexcept
        {
            return {allocator_};
        }

        size_type FindFirstNonEmptyEntry() const noexcept
        {
            const auto entries = entries_.get();
            const auto last = entries + count_;
            const auto i = std::find_if(entries, last, [](const Entry& entry) {
                return entry.HashCode_ != kNothing;
            });
            return i == last ? kNothing : i - entries;
        }

        class BucketDeleter
        {
        public:
            BucketDeleter(const dictionary& dict) noexcept
                :dict_{dict}
            {}

            void operator()(size_type* p) const noexcept
            {
                dict.GetSizeTypeAllocator().deallocate(p);
            }

        private:
            const dictionary& dict_;
        };

        class EntryDeleter
        {
        public:
            EntryDeleter(const dictionary& dict) noexcept
                : dict_{dict}
            {}

            void operator()(Entry* p) const noexcept
            {
                dict.allocator_.deallocate(p);
            }

        private:
            const dictionary& dict_;
        };
     
        size_type freeList_;
        size_type freeCount_ = {};
        //最高水位线。
        size_type count_ = {};
        size_type bucketCount_ = {};
        polymorphic_allocator<Entry> allocator_;
        const EntryDeleter entryDeleter_ {*this};
        const BucketDeleter bucketDeleter_ {*this};
        using EntryPtr = unique_ptr<Entry[], EntryDeleter>;
        using BucketPtr = unique_ptr<size_type[], BucketDeleter>;
        EntryPtr entries_ {nullptr, entryDeleter_};
        BucketPtr buckets_ {nullptr, bucketDeleter_};
        
        void Initialize(size_type capacity)
        {
            const auto newSize = Internal::HashHelpers::GetPrime(capacity);
            buckets_ .reset(GetSizeTypeAllocator().allocate(newSize));
            entries_.reset(allocator_.allocate(newSize));
            const auto buckets = buckets_.get();
            const auto entries = entries_.get();
            
            std::fill(buckets, buckets + newSize, kNothing);                    
            std::for_each(entries, entries + newSize, [](Entry& entry) {
                entry.HashCode_ = kNothing;
            });

            freeList = kNothing;
            bucketCount_ = newSize;
        }

        size_type ConstrainHash(size_type original) const noexcept
        {
            return original % bucketCount_;
        }

        template<typename K, typename... Args>
        pair<iterator, bool> Insert(bool addOnly, K&& key, Args&&... args)
        {          
            if (!buckets_) Initialize({});
            auto hashCode = hash_function()(key);
            auto targetBucket = ContrainHash(hashCode);
            for (auto i = buckets_[targetBucket]; i >= 0; i = entries_[i].NextEntryIndex_)
            {
                const auto& curEntry = entries_[i];
                if (curEntry.HashCode_ == hashCode && key_eq()(curEntry.KeyValue_.first))
                {
                    if (!addOnly)
                        curEntry.KeyValue_.second = mapped_type(std::forward<Args>(args)...);
                    return {};
                }                 
            }
            
            size_type index;
            if (freeCount_ > 0)
            {
                index = freeList_;
                --freeCount_;
                freeList_ = entries_[freeList_].NextEntryIndex_;
            }
            else
            {
                if (bucketCount_ == count_)
                {
                    Resize();
                    targetBucket = ConstrainHash(hashCode);
                }
                index = count_;
                ++count_;
            }

            ConstructEntry(entries_[index], piecewise_construct, Yupei::forward_as_tuple(std::forward<K>(key)),
                Yupei::forward_as_tuple(std::forward<Args>(args)...));
            entries_[index].NextEntryIndex_ = buckets_[targetBucket];
            buckets_[targetBucket] = index;
        }

        template<typename... Args>
        void ConstructEntry(Entry& entry, Args&&... args)
        {
            YPASSERT(entry.HashCode_ == -1, "Construct on existing value.");
            Yupei::construct(std::addressof(entry.KeyValue_), std::forward<Args>(args)...);
        }

        void Resize()
        {
            const auto newSize = Internal::HashHelpers::ExpandPrime(bucketCount_);
            BucketPtr newBuckets {GetSizeTypeAllocator().allocate(newSize), bucketDeleter_};
            EntryPtr newEntries {allocator_.allocate(newSize)};
            const auto nb = newBuckets.get();
            const auto ne = newEntries.get();
            const auto oldEntries = entries_.get();
            std::fill(nb, nb + newSize, kNothing);
            
            for_each_dual(oldEntries, oldEntries + count_, ne, ne + count_,
                [](Entry& entry1, Entry& entry2) 
                {
                    if (entry1.HashCode_ != kNothing)
                        construct(std::addressof(entry2.KeyValue_), std::move(entry1.KeyValue_));                    
                    entry2.HashCode_ = entry1.HashCode_;
                    destroy(std::addressof(entry1));
                });

            for_each_i(ne, ne + count_, [this](size_type i, Entry& entry) {
                const auto hashCode = entry.HashCode_;
                if (hashCode != kNothing)
                {
                    const auto targetBucket = ConstrainHash(hashCode);
                    entry.NextEntryIndex_ = newBuckets[targetBucket];
                    newBuckets[targetBucket] = i;
                }
            });

            buckets_ = std::move(newBuckets);
            entries_ = std::move(newEntries);

            bucketsCount_ = newSize;
        }

        size_type FindEntry(const key_type& key)
        {
            const auto hashCode = hash_function()(key);
            const auto targetBucket = ConstrainHash(hashCode);
            for (auto i = buckets_[targetBucket]; i != kNothing; i = entries_[i].NextEntryIndex_)
                if (entries_[i].HashCode_ == hashCode && entries[i].KeyValue_.first == key)
                    return i;
            return kNothing;
        }
    };

    namespace Internal
    {
        template<typename DictionaryT>
        auto DictionaryIterator<DictionaryT>::operator++() noexcept -> DictionaryIterator&
        {
            using Entry = typename DictionaryT::Entry;
            YPASSERT(dict_ != nullptr, "Iterator is null!");
            const auto entries = dict_->entries_.get();
            const auto i = std::find_if(entries + index_, entries + dict_->count_, [](const Entry& entry) {
                return entry.HashCode_ != -1;
            });
            index_ = i - entries;
            return *this;
        }

        template<typename DictionaryT>
        auto DictionaryConstIterator<DictionaryT>::operator++() noexcept -> DictionaryConstIterator&
        {
            using Entry = typename DictionaryT::Entry;
            YPASSERT(dict_ != nullptr, "Iterator is null!");
            const auto entries = dict_->entries_.get();
            const auto i = std::find_if(entries + index_, entries + dict_->count_, [](const Entry& entry) {
                return entry.HashCode_ != -1;
            });
            index_ = i - entries;
            return *this;
        }

        template<typename DictionaryT>
        auto DictionaryLocalIterator<DictionaryT>::operator++() noexcept -> DictionaryLocalIterator&
        {
            YPASSERT(dict_ != nullptr, "Iterator is null!");
            YPASSERT(index_ == -1, "Increase an end iterator!");
            index_ = dict_->entries_[index_].NextEntryIndex_;
            return *this;
        }

        template<typename DictionaryT>
        auto DictionaryConstLocalIterator<DictionaryT>::operator++() noexcept -> DictionaryConstLocalIterator&
        {
            YPASSERT(dict_ != nullptr, "Iterator is null!");
            YPASSERT(index_ == -1, "Increase an end iterator!");
            index_ = dict_->entries_[index_].NextEntryIndex_;
            return *this;
        }
    }

}