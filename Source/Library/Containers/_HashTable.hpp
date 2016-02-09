//#pragma once
//
//#include "hash.h"
//#include "functional.h"
//#include "iterator.h"
//#include "list.h"
//#include "SmartPointers\unique_ptr.h"
//#include "MemoryReasource\memory_resource.h"
//#include "..\Custom\Assert.h"
//#include "..\CLib\math.h"
//
//namespace Yupei
//{
//    namespace Internal
//    {   
//        template<typename ValueT>
//        struct HashTableListNodeBase
//        {
//
//        };
//
//        template<typename ValueT>
//        struct HashTableListNode
//        {           
//            HashTableListNode* NextNode_;
//            ValueT Value_;
//        };
//        
//        extern std::size_t GetNextPrime(std::size_t n);
//
//        template<typename KeyT, typename ValueT, typename HashFun, typename KeyEqualT = equal_to<ValueT>>
//        class HashTable : polymorphic_allocator<Internal::HashTableListNode<ValueT>>, KeyEqualT 
//        {
//        public:           
//            using value_type = ValueT;
//            using key_type = KeyT;
//            using size_type = std::size_t;
//            using allocator_type = polymorphic_allocator<Internal::HashTableListNode<ValueT>>;
//            using key_equal = KeyEqualT;
//            using hasher = HashFun;
//
//        private:
//            using MyType = HashTable<KeyT, ValueT, ExtractFunT, KeyEqualT>;
//
//        public:
//            using iterator = HashTableIterator<MyType>;
//            using local_iterator = HashTableLocalIterator<MyType>;
//            using const_local_iterator = HashTableConstLocalIterator<MyType>;
//
//        private:
//            using NodeType = HashTableListNode<value_type>;
//            using NodePointer = NodeType*;
//
//        public:
//            HashTable()
//                :elementsCount_{}
//            {}
//
//            HashTable(memory_resource_ptr resource, key_equal keyEqual = {})
//                :allocator_type{resource},
//                key_equal{keyEqual},
//                buckets_{resource},
//                elementsCount_{}
//            {}
//
//            allocator_type get_allocator() const noexcept
//            {
//                return static_cast<allocator_type>(*this);
//            }
//
//            size_type bucket_count() const noexcept
//            {
//                return buckets_.size();
//            }
//
//            float load_factor() const noexcept
//            {
//                return static_cast<float>(elementsCount_) /
//                    static_cast<float>(bucketsCount_);
//            }  
//
//            size_type size() const noexcept
//            {
//                return elementsCount_;
//            }
//            
//            float max_load_factor() const noexcept
//            {
//                return maxLoadFactor_;
//            }
//
//            void max_load_factor(float ml) noexcept
//            {
//                maxLoadFactor_ = ml;
//            }
//
//            iterator begin() noexcept
//            {
//               // return {beginNode_.Next_};
//            }
//
//            const_iterator begin() const noexcept
//            {
//                //return {beginNode_.Next_};
//            }
//
//            const_iterator cbegin() const noexcept
//            {
//                return begin();
//            }
//
//            local_iterator end(size_type n) noexcept
//            {
//                return {buckets_[n]};
//            }
//
//            const_local_iterator end(size_type n) const noexcept
//            {
//                return {buckets_[n]};
//            }
//
//            const_local_iterator cend(size_type n) const noexcept
//            {
//                return end(n);
//            }
//
//        private:
//            unique_ptr<NodePointer[]> buckets_;
//            size_type bucketsCount_;
//            size_type elementsCount_;
//            float maxLoadFactor_;
//
//            static inline constexpr bool IsHashPower2(size_type s) noexcept
//            {               
//                return s > 2 && (s & (s - 1)) == 0;
//            }
//
//            static inline constexpr size_type ContrainHash(size_type h, size_type bucketCount) noexcept
//            {
//                return !(bucketCount & (bucketCount - 1)) ? h & (bucketCount - 1) : h % bucketCount;
//            }
//
//            auto key_equal() const noexcept -> key_equal { return static_cast<key_equal>(*this); }
//            
//            template<typename... ParamsT>
//            auto MakeNode(ParamsT&&... params) -> NodePointer
//            {
//                const auto deleteFn = [this](NodePointer p) {this->deallocate(p, 1);};
//                unique_ptr<NodeType, decltype(deleteFn)> holder{this->allocate(1), deleteFn};               
//                Yupei::construct(Yupei::addressof(holder->Value_), Yupei::forward<ParamsT>(params)...);
//                return holder;
//            }
//
//            size_type GetNextBucketCount(size_type n) const noexcept
//            {
//                return {};
//            }
//
//            void ReHash(size_type n)
//            {
//                const auto newBucketCount = GetNextBucketCount(n);
//                auto newBuckets = make_unique<NodePointer[]>(newBucketCount);
//                
//            }
//
//            template<typename... ParamsT>
//            auto InsertUnique(ParamsT&&... params) -> pair<iterator, bool>
//            {
//                auto node = MakeNode(Yupei::forward<ParamsT>(params)...);
//                auto hashCode = hasher{}(node->Value_);
//                bool inserted = false;
//                const auto bucketsCount = bucket_count();
//                if (bucketsCount != 0)
//                {
//                    auto constrainedHash = HashTable::ContrainHash(hashCode, bucketsCount);
//                    
//                }
//            }
//
//            size_type MinimalSize() const noexcept
//            {
//                return static_cast<size_type>(ceil(size() / max_load_factor()));
//            }
//            
//        };
//
//        //template<typename HashTableT>
//        //class HashTableIterator
//    }
//}