#pragma once

#include <cassert>
#include <memory>
#include <utility>
#include <tuple>
#include "../MemoryResource/MemoryResource.hpp"
#include "../ConstructDestruct.hpp"
#include "../Extensions.hpp"

namespace Yupei
{
	namespace Internal
	{
		//Robert Sedgewick 提出的 LLRB 实现。
		//TODO: 对于递归调用的 delete/insert，要判断是否为空？
		template<typename SelectKeyFun, typename ValueT, typename CompFun>
		class RedBlackTree : CompFun
		{
		public:
			using value_type = ValueT;
			using key_type = decltype(SelectKeyFun()(std::declval<ValueT>()));
			using key_compare = CompFun;
			using size_type = std::size_t;

			RedBlackTree(const RedBlackTree& other)
				:allocator_{other.allocator_}
			{
				head_ = CopyTree(other.head_);
				head_->Parent = {};
				size_ = other.size_;
			}

			RedBlackTree(RedBlackTree&& other) noexcept
				:head_ { other.head_ },
				allocator_ { std::move(other.allocator_) },
				size_ { other.size_ }
			{
				other.head_ = {};
				other.size_ = {};
			}

			RedBlackTree& operator=(const RedBlackTree& rhs)
			{
				RedBlackTree { rhs }.swap(*this);
				return *this;
			}

			RedBlackTree& operator=(RedBlackTree&& rhs) noexcept
			{
				RedBlackTree { std::move(rhs) }.swap(*this);
				return *this;
			}

			key_compare key_comp() const noexcept
			{
				return static_cast<const key_compare&>(*this);
			}

			template<bool IsMap, bool AllowDuplicate, typename KeyT, typename... ParamsT>
			void Insert(std::bool_constant<IsMap> isMap, std::bool_constant<AllowDuplicate> allowDuplicate,
				KeyT&& key, ParamsT&&... params)
			{
				head_ = InsertImp(isMap, allowDuplicate, head_, std::forward<KeyT>(key), std::forward<ParamsT>(params)...);
			}

			TreeNode* Find(const key_type& key) const noexcept
			{
				auto node = head_;
				while (node != nullptr)
				{
					if (key_comp()(SelectKeyFun()(node->Value_), key))
						node = node->Children_[kRight];
					else if (key_comp()(key, SelectKeyFun()(node->Value_)))
						node = node->Children_[kLeft];
					else
						return node;
				}
				return {};
			}

			std::size_t Delete(const key_type& key) noexcept
			{
				assert(size_ == 0);
				std::size_t deletedCount = {};
				head_ = DeleteImp(head_, key, deletedCount);
				size_ -= deletedCount;
				return deletedCount;
			}

			std::size_t Size() const noexcept
			{
				return size_;
			}

			void Clear() noexcept
			{
				if (head_)
					ClearImp(head_);
				head_ = {};
				size_ = {};
			}

			~RedBlackTree()
			{
				Clear();
			}

			void swap(RedBlackTree& other) noexcept
			{
				using std::swap;
				swap(head_, other.head_);
				swap(size_, other.size_);
			}

		private:
			enum
			{
				kLeft = 0,
				kRight = 1
			};

			enum NodeColor
			{
				kRed = 0,
				kBlack = 1
			};

			class TreeNode
			{
			public:
				template<typename... ParamsT>
				TreeNode(NodeColor color, ParamsT&&... params)
					:Color_ { color }, Value_(std::forward<ParamsT>(params)...)
				{}

				value_type Value_;
				NodeColor Color_;
				TreeNode* Parent_;
				TreeNode* Children_[2] = {};
			};

			TreeNode* head_ = {};
			polymorphic_allocator<TreeNode> allocator_;
			size_type size_ = {};

			static void ClearImp(TreeNode* node) noexcept
			{
				assert(node != nullptr);
				const auto leftChild = node->Children_[kLeft];
				const auto rightChild = node->Children_[kRight];
				DeallocateNode(node);
				if (leftChild) ClearImp(leftChild);
				if (rightChild) ClearImp(rightChild);
			}

			static TreeNode* CopyTree(TreeNode* node)
			{
				if (node)
				{
					const auto srcLeftChild = node->Children_[kLeft];
					const auto srcRightChild = node->Children_[kRight];
					const auto desLeftChild = CopyTree(srcLeftChild);
					const auto desRightChild = CopyTree(srcRightChild);
					const auto deleteFn = [this](TreeNode* node) noexcept {allocator_.deallocate(node, 1);};
					auto newNode = std::unique_ptr<TreeNode, decltype(deleteFn)> { allocator_.allocate(1), deleteFn };
					Yupei::construct(std::addressof(newNode->Value_), node->Value_);
					newNode->Color_ = node->Color_;
					desLeftChild->Parent_ = newNode.get();
					desRightChild->Parent = newNode.get();
					return newNode.release();
				}
				return {};
			}

			//key 与 params... 的构造问题。
			//如果是 map，需要 std::piecewise_construct；
			//如果是 set，不需要。
			template<bool IsMap, bool AllowDuplicate, typename KeyT, typename... ParamsT>
			TreeNode* InsertImp(std::bool_constant<IsMap> isMap, std::bool_constant<AllowDuplicate> allowDuplicate,
				TreeNode* node, KeyT&& key, ParamsT&&... params)
			{
				if (node == nullptr)
				{
					++size_;
					return MakeNode(isMap, kRed, std::forward<KeyT>(key), std::forward<ParamsT>(params)...).release();
				}				
				//一些前置条件。
				//node 为空的情况上面已经处理。下面代码中保证 node 不为空。
				
				const auto& thisKey = SelectKeyFun()(node->Value_);
				if (key_comp()(thisKey, std::forward<KeyT>(key)))
				{
					auto& child = node->Children_[kLeft];
					child = InsertImp(isMap, allowDuplicate, child, std::forward<KeyT>(key), std::forward<ParamsT>(params)...);
					child->Parent_ = node;
				}
				else if (key_comp()(thisKey, std::forward<KeyT>(key)))
				{
					auto& child = node->Children_[kRight];
					child = InsertImp(isMap, allowDuplicate, child, std::forward<KeyT>(key), std::forward<ParamsT>(params)...);
					child->Parent_ = node;
				}
				else if (allowDuplicate)
				{
					//相等时选择右面。
					auto& child = node->Children_[kRight];
					child = InsertImp(isMap, allowDuplicate, child, std::forward<KeyT>(key), std::forward<ParamsT>(params)...);
					child->Parent_ = node;
				}

				return FixUp(node);
			}

			TreeNode* DeleteImp(TreeNode* node, const key_type& key, std::size_t& deletedCount)
			{
				const auto leftChild = node->Children_[kLeft];
				if (key_comp()(key, SelectKeyFun()(node->Value_)))
				{
					if (!IsRed(leftChild) && !IsRed(leftChild->Children_[kLeft]))
						node = MoveRedLeft(node);
					const auto newLeft = DeleteImp(node, key, deletedCount);
					newLeft->Parent_ = node;
					node->Children_[kLeft] = newLeft;
				}
				else
				{
					if (IsRed(leftChild)) node = RotateRight(node);

					const auto isBigger = key_comp()(SelectKeyFun()(node->Value_), key);
					//如果 right 不为空，那么会找它的后继。
					if (!isBigger && node->Children_[kRight] == nullptr)
					{
						//此时在底，并且已经找到了 key。
						DeallocateNode(node);
						++deletedCount;
						return nullptr;
					}

					const auto rightChild = node->Children_[kRight];
					if (!IsRed(rightChild) && !IsRed(rightChild->Children_[kLeft]))
						node = MoveRedRight(node);

					if (!isBigger)
					{
						//找到后继。
						const auto successor = Minimum(node->Children_[kRight]);
						//只有 node 为空，minNode 才会为空，而 node 为空的情况
						node->Value_ = std::move(successor->Value_);
						const auto deleted = DeleteMin(node->Children_[kRight]);
						deleted->Parent_ = node;
						node->Children_[kRight] = deleted;
					}
					else
					{
						const auto newRight = DeleteImp(node, key, deletedCount);
						newRight->Parent_ = node;
						node->Children_[kRight] = newRight;
					}
				}
				return FixUp(node);
			}

			static TreeNode* Minimum(TreeNode* node) noexcept
			{
				if (node == nullptr) return nullptr;
				while (node->Children_[kLeft] != nullptr) node = node->Children_[kLeft];
				return node;
			}

			static TreeNode* FixUp(TreeNode* node) noexcept
			{
				assert(node != nullptr);
				//遇到 4-node 进行颜色翻转。
				if (IsRed(node->Children_[kLeft]) && IsRed(node->Children_[kRight]))
					ColorFlip(node);

				//向上回溯的时候修正：
				//1.作为右孩子的红节点。（LLRB 限制只能是左孩子）。
				//2.连着两个左红节点。（在分离 4-node 的时候产生）。
				if (IsRed(node->Children_[kRight]))
					node = RotateLeft(node);

				//在 && 右面使用 left 的时候保证 left 不为空：如果为空那么 IsRed 返回 false。
				auto& left = node->Children_[kLeft];
				if (IsRed(left) && IsRed(left->Children[kLeft]))
					node = RotateRight(node);

				return node;
			}

			template<typename KeyT, typename... ParamsT>
			auto MakeNode(std::true_type, NodeColor color, KeyT&&... key, ParamsT&&... params)
			{
				//FIXME!: 这里没有使用 allocator。
				//是 map，使用 std::piecewise_construct 构造。
				return Yupei::make_unique_del<TreeNode>([this](TreeNode* node) noexcept {allocator_->deallocate(node);}, color, 
					std::piecewise_construct, std::forward_as_tuple(std::forward<KeyT>(key)), std::forward_as_tuple(std::forward<ParamsT>(params)...));
			}

			template<typename KeyT, typename... ParamsT>
			auto MakeNode(std::false_type, NodeColor color, KeyT&&... key, ParamsT&&... params)
			{
				//是 set，直接构造。
				return Yupei::make_unique_del<TreeNode>([this](TreeNode* node) noexcept {allocator_->deallocate(node);},
					color, std::forward<KeyT>(key));
			}

			static TreeNode* Rotate(TreeNode* node, int direction) noexcept
			{
				const auto reversedDirection = ReverseDirection(direction);
				const auto newHead = node->Children_[reversedDirection];
				const auto l = newHead->Children_[direction];
				newHead->Children_[direction] = node;
				node->Parent_ = newHead;
				node->Children_[reversedDirection] = l;
				l->Parent_ = node;
				return newHead;
			}

			static TreeNode* ColorFlip(TreeNode* node) noexcept
			{
				const auto FlipSingleNode = [](TreeNode* node) noexcept
				{
					auto& color = node->Children_[kLeft]->Color_;
					color = FlipColor(color);
				};
				FlipSingleNode(node);
				FlipSingleNode(node->Children_[kLeft]);
				FlipSingleNode(node->Children_[kRight]);
				return node;
			}

			static bool IsRed(TreeNode* node) noexcept
			{
				if (node == nullptr) return false;
				return node->Color_ == kRed;
			}

			static TreeNode* RotateLeft(TreeNode* node) noexcept
			{
				return Rotate(node, kLeft);
			}

			static TreeNode* RotateRight(TreeNode* node) noexcept
			{
				return Rotate(node, kRight);
			}

			static TreeNode* MoveRedRight(TreeNode* node) noexcept
			{
				//"Borrow from sibling if necessary".
				//when h.right and h.right.left are both BLACK（问题，如果 h.right.right 为红，那么还需要 borrow 吗？）
				//分两种情况。
				//1. 当前节点的左孩子的左孩子是黑的。那么 ColorFlip 即可。
				//2. 如果是红的，那么会出现连续两个左孩子都为红的情况，此时右旋转并 ColorFlip。
				ColorFlip(node);
				if (IsRed(node->Children_[kLeft]->Children_[kLeft]))
				{
					node = RotateRight(node);
					ColorFlip(node);
				}
				return node;
			}

			static TreeNode* MoveRedLeft(TreeNode* node) noexcept
			{
				ColorFlip(node);
				const auto rightChild = node->Children_[kRight];
				if (IsRed(rightChild->Children_[kLeft]))
				{
					const auto newRight = RotateRight(rightChild);
					newRight->Parent_ = rightChild;
					node->Children_[kRight] = newRight;
					node = RotateRight(node);
					ColorFlip(node);
				}
				return node;
			}

			void DeleteMax() noexcept
			{
				head_ = DeleteMax(head_);
				head_->Color_ = kBlack;
			}
		
			TreeNode* DeleteMax(TreeNode* node) noexcept
			{
				//删除一个 2-node 会破坏 2-3 树的性质，故在自上而下的时候将 > 2-node 向下“推”，从而保证被删除的阶段不是 2-node。
				if (IsRed(node->Children_[kLeft]))
					node = RotateRight(node);

				const auto& rightChild = node->Children_[kRight];
				if (rightChild == nullptr)
				{
					//已经到了底部。
					DeallocateNode(node);
					return nullptr;
				}
					
				if (!IsRed(rightChild) && !IsRed(rightChild->Children_[kRight]))
					node = MoveRedRight(node);

				const auto child = DeleteMax(node->Children_[kRight]);
				child->Parent_ = node;
				node->Children_[kRight] = child;

				return FixUp(node);
			}

			TreeNode* DeleteMin(TreeNode* node) noexcept
			{
				const auto leftChild = node->Children_[kLeft];
				if (leftChild == nullptr)
				{
					DeallocateNode(node);
					return nullptr;
				}

				if (!IsRed(leftChild) && !IsRed(leftChild->Children_[kLeft]))
					node = MoveRedLeft(node);

				const auto newLeft = DeleteMin(node->Children_[kLeft]);
				newLeft->Parent_ = node;
				node->Children_[kLeft] = newLeft;

				return FixUp(node);
			}

			static void DeallocateNode(TreeNode* node) noexcept
			{
				Yupei::destroy_at(std::addressof(node->Value_));
				allocator_.deallocate(node, 1);
			}

			static int ReverseDirection(int direction) noexcept
			{
				switch (direction)
				{
				case kLeft:
					return kRight;
				case kRight:
					return kLeft;
				default:
					break;
				}
				assert(0);
			}

			static NodeColor FlipColor(NodeColor color) noexcept
			{
				switch (color)
				{
				case kRed:
					return kBlack;
				case kBlack:
					return kRed;
				default:
					break;
				}
				assert(0);
			}
		};
	}

}