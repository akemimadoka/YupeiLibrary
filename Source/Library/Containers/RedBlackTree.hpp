#pragma once

#include <cassert>
#include <memory>
#include <utility>
#include <tuple>
#include "..\MemoryResource\MemoryResource.hpp"
#include "..\ConstructDestruct.hpp"
#include "..\Extensions.hpp"

namespace Yupei
{
	namespace Internal
	{
		//Robert Sedgewick 提出的 LLRB 实现。
		template<typename SelectKeyFun, typename ValueT, typename CompFun>
		class RedBlackTree : CompFun
		{
		public:
			using value_type = ValueT;
			using key_type = decltype(SelectKeyFun()(std::declval<ValueT>()));
			using key_compare = CompFun;

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
				TreeNode* Children_[2] = {};
			};

			TreeNode* head_ = {};
			polymorphic_allocator<TreeNode> allocator_;

			//key 与 params... 的构造问题。
			//如果是 map，需要 std::piecewise_construct；
			//如果是 set，不需要。
			template<bool IsMap, bool AllowDuplicate, typename KeyT, typename... ParamsT>
			TreeNode* InsertImp(std::bool_constant<IsMap> isMap, std::bool_constant<AllowDuplicate> allowDuplicate,
				TreeNode* node, KeyT&& key, ParamsT&&... params)
			{
				if (node == nullptr)
					return MakeNode(isMap, kRed, std::forward<KeyT>(key), std::forward<ParamsT>(params)...).release();

				//一些前置条件。
				//node 为空的情况上面已经处理。下面代码中保证 node 不为空。

				//自上而下时，遇到 4-node 进行颜色翻转。
				if (IsRed(node->Children_[kLeft]) && IsRed(node->Children_[kRight]))
					ColorFlip(node);
				const auto& thisKey = SelectKeyFun()(node->Value_);
				if (key_comp()(thisKey, std::forward<KeyT>(key)))
				{
					auto& child = node->Children_[kLeft];
					child = InsertImp(isMap, allowDuplicate, child, std::forward<KeyT>(key), std::forward<ParamsT>(params)...);
				}
				else if (key_comp()(thisKey, std::forward<KeyT>(key)))
				{
					auto& child = node->Children_[kRight];
					child = InsertImp(isMap, allowDuplicate, child, std::forward<KeyT>(key), std::forward<ParamsT>(params)...);
				}
				else if (allowDuplicate)
				{
					//相等时选择左面。
					auto& child = node->Children_[kLeft];
					child = InsertImp(isMap, allowDuplicate, child, std::forward<KeyT>(key), std::forward<ParamsT>(params)...);
				}

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
				node->Children_[reversedDirection] = l;
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