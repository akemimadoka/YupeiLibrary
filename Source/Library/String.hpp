#pragma once

#include "StringView.hpp"
#include "MemoryResource\MemoryResource.hpp"
#include "Ranges\Xrange.hpp"
#include "Limits.hpp"
#include "Containers\Vector.hpp"
#include "Searchers.hpp"
#include <algorithm>
#include <utility>
#include <cstdint>

namespace Yupei
{
	template<string_type StringT>
	class basic_string
	{
	public:
		using view_type = basic_string_view<StringT>;
		using value_type = typename view_type::value_type;
		using size_type = std::size_t;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;
		using iterator = pointer;
		using const_iterator = const_pointer;

		basic_string(memory_resource_ptr pmr = {}) noexcept
			:allocator_ { pmr }
		{
			SetSmallSize(0);
		}

		basic_string(size_type n, memory_resource_ptr pmr = {})
			:basic_string(pmr)
		{
			append(value_type {}, n);
		}

		basic_string(const view_type& view, memory_resource_ptr pmr = {})
			:basic_string(pmr)
		{
			append(view);
		}

		basic_string(const_pointer value, memory_resource_ptr pmr = {})
			:basic_string(value, 0, Internal::StrLen(value), pmr)
		{}

		basic_string(const_pointer value, size_type startIndex, size_type count, memory_resource_ptr pmr = {})
			:basic_string(view_type { value + startIndex, count }, pmr)
		{}

		basic_string(const basic_string& other)
			:basic_string()
		{
			append(other);
		}

		basic_string& operator=(const basic_string& other)
		{
			basic_string(other).swap(*this);
			return *this;
		}

		basic_string(basic_string&& other) noexcept
			:basic_string()
		{
			storage_ = other.storage_;
			other.SetSmallSize(0);
		}

		basic_string& operator=(basic_string&& other) noexcept
		{
			basic_string(std::move(other)).swap(*this);
			return *this;
		}

		~basic_string()
		{
			if (IsBig())
				allocator_.deallocate(storage_.big_.ptr_, GetBigSize());
			SetSmallSize(0);
		}

		void swap(basic_string& other)
		{
			std::swap(storage_, other.storage_);
		}

		size_type size() const noexcept
		{
			return GetSize();
		}

		view_type to_string_view() const noexcept
		{
			if (IsBig())
				return { storage_.big_.ptr_, GetBigSize() };
			else
				return { storage_.small_.data_, GetSmallSize() };
		}

		size_type capacity() const noexcept
		{
			return GetCapacity();
		}

		pointer data() & noexcept
		{
			return c_str();
		}

		const_pointer data() const & noexcept
		{
			return c_str();
		}

		pointer data() && noexcept = delete;
		const_pointer data() const && noexcept = delete;

		pointer c_str() & noexcept
		{
			if (IsBig())
			{
				const auto str = storage_.big_.ptr_;
				str[storage_.big_.size_] = {};
				return str;
			}
			else
			{
				const auto str = storage_.small_.data_;
				str[storage_.small_.size_] = {};
				return str;
			}
		}

		const_pointer c_str() const & noexcept
		{
			if (IsBig())
			{
				const auto str = storage_.big_.ptr_;
				const_cast<reference>(str[storage_.big_.size_]) = {};
				return str;
			}
			else
			{
				const auto str = storage_.small_.data_;
				const_cast<reference>(str[storage_.small_.size_]) = {};
				return str;
			}
		}

		pointer c_str() && noexcept = delete;
		const_pointer c_str() const && noexcept = delete;

		reference operator[](size_type n) noexcept
		{
			return GetBegin()[n];
		}

		const_reference operator[](size_type n) const noexcept
		{
			return GetBegin()[n];
		}

		reference at(size_type n)
		{
			if (n >= size()) throw std::out_of_range("Out of range!");
			return (*this)[n];
		}

		const_reference at(size_type n) const
		{
			if (n >= size()) throw std::out_of_range("Out of range!");
			return (*this)[n];
		}

		reference front() noexcept
		{
			return *GetBegin();
		}

		const_reference front() const noexcept
		{
			return *GetBegin();
		}

		reference back() noexcept
		{
			return GetEnd()[-1];
		}

		const_reference back() const noexcept
		{
			return GetEnd()[-1];
		}

		iterator begin() noexcept
		{
			return { GetBegin() };
		}

		const_iterator begin() const noexcept
		{
			return { GetBegin() };
		}

		iterator end() noexcept
		{
			return { GetEnd() };
		}

		const_iterator end() const noexcept
		{
			return { GetEnd() };
		}

		void push_back(value_type c)
		{
			const auto prevSize = size();
			*ReserveMore(1) = c;
			SetSize(prevSize + 1);
		}

		void pop_back(size_type n = 1) noexcept
		{
			IncrementSize(limits_max_v<size_type> +n - 1);
		}

		void append(value_type c)
		{
			push_back(c);
		}

		void append(const_pointer str)
		{
			append(view_type { str });
		}

		void append(const_pointer str, size_type startIndex, size_type charCount)
		{
			append(view_type { str + startIndex, charCount });
		}

		//这里会警告有未使用的变量 i，等 [[maybe_unused]]。
		void append(value_type value, size_type repeatCount)
		{
			const auto prevSize = size();
			ReserveMore(repeatCount);
			for (auto i : xrange(repeatCount))
				UncheckedPush(value);
			SetSize(prevSize + repeatCount);
		}

		void append(const view_type& view)
		{
			using Yupei::cbegin;
			using Yupei::cend;
			const auto prevSize = size();
			const auto viewSize = view.size();
			const auto newSpace = ReserveMore(viewSize);
			std::copy(cbegin(view), cend(view), newSpace);
			SetSize(prevSize + viewSize);
		}

		void append(const basic_string& value)
		{
			append(value.to_string_view());
		}

		size_type max_size() const noexcept
		{
			return allocator_.max_size();
		}

		void append(const basic_string& value, size_type startIndex, size_type count)
		{
			append(value.to_string_view().substr(startIndex, count));
		}

		//将 count 个 value 插入到 index 之前。
		void insert(size_type index, const view_type& value, size_type count = 1)
		{
			using Yupei::cbegin;
			using Yupei::cend;
			if (count == 0) return;
			const auto prevSize = size();
			const auto charsToInsert = value.size() * count;
			auto insertPlace = MakeRoom(charsToInsert, index, index);
			for (;;)
			{
				insertPlace = std::copy(cbegin(value), cend(value), insertPlace);
				--count;
				if (count == 0)
					break;
			}
			SetSize(prevSize + charsToInsert);
		}

		void insert(size_type index, const_pointer value, size_type startIndex, size_type charCount)
		{
			insert(index, view_type { value + startIndex, charCount });
		}

		void remove(size_type startIndex, size_type length)
		{
			//TODO: 这里可以更高效。
			assert(startIndex + length < size());
			const auto strBegin = GetBegin();
			const auto strEnd = GetEnd();
			const auto curSize = size();
			const auto removeBegin = strBegin + startIndex;
			std::move(removeBegin + length, strEnd, removeBegin);
			SetSize(curSize - length);
		}

		void replace(const view_type& oldString, const view_type& newString)
		{
			using Yupei::cbegin;
			using Yupei::cend;
			using Yupei::begin;
			using Yupei::end;
			vector<iterator> replacements;
			//TODO: 小字符串应该使用 default searcher。
			const auto searcher = make_boyer_moore_searcher(begin(oldString), end(oldString));
			pair<iterator, iterator> result;
			const auto strBegin = begin(*this);
			const auto strEnd = end(*this);
			auto searchBegin = strBegin;
			for (;;)
			{
				result = searcher(searchBegin, strEnd);
				if (result.first != strEnd)
					replacements.push_back(result.first);
				else break;
				searchBegin = result.second;
			}
			if (!replacements.empty())
			{
				const auto oldSize = oldString.size();
				const auto newSize = newString.size();
				const auto prevEntireStringSize = size();

				//TODO: sizeToEnsure 的溢出问题。
				auto sizeToEnsure = prevEntireStringSize;
				if (oldSize > newSize)
					sizeToEnsure -= (oldSize - newSize) * replacements.size();
				else
					sizeToEnsure += (newSize - oldSize) * replacements.size();

				//脏。                
				auto oldStorage = strBegin;
				size_type curReplace = {};
				const auto wasBig = IsBig();
				if ((!wasBig && sizeToEnsure < SmallMaxLength - 1)
					|| sizeToEnsure < capacity() //注意这里，必须严格小于 capacity，因为要保留空字符的位置。
					)
				{
					//从 elementsToAllocate 这个位置开始向后移。
					size_type cur = replacements.size() - 1;
					auto curProgress = strBegin + sizeToEnsure;
					auto oldEnd = strEnd;
					for (; cur != size_type(-1); --cur)
					{
						auto replacement = replacements[cur];
						const auto moveStart = replacement + oldSize;
						curProgress = std::move_backward(moveStart, oldEnd, curProgress);
						curProgress = std::copy_backward(cbegin(newString), cend(newString), curProgress);
						oldEnd = replacement;
					}
					SetSize(sizeToEnsure);
				}
				else
				{
					const auto elementsToAllocate = ((sizeToEnsure + 1 + 0x0Fu) & size_type(-0x10));
					if (elementsToAllocate < sizeToEnsure) throw std::bad_array_new_length();

					const auto newStorage = allocator_.allocate(elementsToAllocate);
					auto curProgress = newStorage;
					for (; curReplace != replacements.size();)
					{
						const auto oldEnd = replacements[curReplace];
						curProgress = std::copy(oldStorage, oldEnd, curProgress);
						curProgress = std::copy(cbegin(newString), cend(newString), curProgress);
						oldStorage = oldEnd + oldString.size();
						++curReplace;
					}
					std::copy(oldStorage, strEnd, curProgress);
					SetBigCapacity(elementsToAllocate);
					if (wasBig) allocator_.deallocate(storage_.big_.ptr_, GetBigSize());
					storage_.big_.ptr_ = newStorage;
					storage_.big_.size_ = sizeToEnsure;
				}
			}
		}

		void replace(value_type oldChar, value_type newChar) noexcept
		{
			for (auto& c : *this)
				if (c == oldChar)
					c = newChar;
		}

		void reserve(size_type newCap)
		{
			Reserve(newCap);
		}

	private:
		struct Big
		{
			size_type capacity_;
			size_type size_;
			pointer ptr_;
		};

		static constexpr size_type SmallMaxLength = max(2, sizeof(Big) / sizeof(value_type));
		using UnsignedCharT = std::make_unsigned_t<value_type>;

		//padding?
		struct Small
		{
			UnsignedCharT size_;
			value_type data_[SmallMaxLength - 1];
		};

		union Storage
		{
			Big big_;
			Small small_;
		}storage_;

		polymorphic_allocator<value_type> allocator_;

		static constexpr size_type BigMask = 1;
		static constexpr UnsignedCharT SmallMask = 1;

		pointer ReserveMore(size_type delta)
		{
			const auto curSize = size();
			const auto sizeToEnsure = curSize + delta;
			const auto curCap = capacity();
			if (sizeToEnsure < curSize) throw std::bad_array_new_length();
			if (sizeToEnsure <= curCap) return GetEnd();
			return MakeRoom(sizeToEnsure, curSize, curSize);
		}

		void Reserve(size_type newCap)
		{
			const auto curCap = capacity();
			const auto curSize = size();
			if (newCap <= curCap) return;
			(void)MakeRoom(newCap, curSize, curSize);
		}

		//例如 Insert、Replace 这些函数都会有类似的要求：
		//先预留好空间。拷贝前半部分，这时在新区域加入或替换，再拷贝后半部分。
		//这个函数的作用：在 copyEndIndex1 与 copyStartIndex2 之间留出 count 的空隙。
		pointer MakeRoom(size_type count, size_type copyEndIndex1, size_type copyStartIndex2)
		{
			assert(copyEndIndex1 <= copyStartIndex2);
			const auto str = GetBegin();
			const auto strEnd = GetEnd();
			const auto copyPrevEnd = str + copyEndIndex1;
			const auto copyLastStart = str + copyStartIndex2;
			const auto prevSize = size();
			const auto elementsRequired = prevSize + count;
			const auto wasBig = IsBig();

			//这里已经处理了 Small 的情况。
			const auto curCap = capacity();
			if (elementsRequired <= curCap)
			{
				std::move_backward(copyLastStart, strEnd, strEnd + count);
				return str + copyEndIndex1;
			}

			const auto elementsToAllocate = ((elementsRequired + 1 + 0x0Fu) & size_type(-0x10));
			if (elementsToAllocate < elementsRequired || elementsToAllocate >= max_size()) throw std::bad_array_new_length();

			const auto newStorage = allocator_.allocate(elementsToAllocate);
			if (copyPrevEnd == strEnd && copyLastStart == strEnd)
				std::copy(str, strEnd, newStorage);//在末尾留 count，直接全部拷贝过去。
			else
			{
				const auto des = std::copy(str, copyPrevEnd, newStorage);
				std::copy(copyLastStart, strEnd, des + count);
			}

			SetBigCapacity(elementsToAllocate);
			if (wasBig) allocator_.deallocate(storage_.big_.ptr_, prevSize);
			else SetBigSize(prevSize);
			storage_.big_.ptr_ = newStorage;
			return newStorage + copyEndIndex1;
		}

		void UncheckedPush(value_type c) noexcept
		{
			assert(capacity() >= size() + 1);
			if (IsBig())
			{
				const auto prevSize = GetBigSize();
				storage_.big_.ptr_[prevSize] = c;
				SetBigSize(prevSize + 1);
			}
			else
			{
				const auto prevSize = GetSmallSize();
				storage_.small_.data_[prevSize] = c;
				SetSmallSize(prevSize + 1);
			}
		}

		bool IsBig() const noexcept
		{
			//如果正在使用 Small，那么 capacity 最低位为 0，否则，存储的 capacity 为奇数（最低位为 1）（实际为偶数）。
			return (storage_.big_.capacity_ & BigMask) != 0;
		}

		void SetSize(size_type s) noexcept
		{
			if (IsBig())
				SetBigSize(s);
			else
				SetSmallSize(s);
		}

		void IncrementSize(size_type delta) noexcept
		{
			if (IsBig())
				storage_.big_.size_ += delta;
			else
			{
				assert(GetSmallSize() + delta <= SmallMaxLength - 1);
				SetSmallSize(GetSmallSize() + delta);
			}
		}

		size_type GetSmallSize() const noexcept
		{
			return storage_.small_.size_ >> 1;
		}

		void SetSmallSize(size_type s) noexcept
		{
			assert(s < SmallMaxLength);
			storage_.small_.size_ = (s << 1);
		}

		size_type GetBigSize() const noexcept
		{
			assert(IsBig());
			return storage_.big_.size_;
		}

		void SetBigSize(size_type s) noexcept
		{
			assert(IsBig());
			storage_.big_.size_ = s;
		}

		//Capacity 只能是偶数，正是我们期望的。
		void SetBigCapacity(size_type s) noexcept
		{
			storage_.big_.capacity_ = s | BigMask;
		}

		size_type GetBigCapacity() const noexcept
		{
			return storage_.big_.capacity_ & ~(BigMask);
		}

		size_type GetSize() const noexcept
		{
			if (IsBig()) return GetBigSize();
			else return GetSmallSize();
		}

		pointer GetBegin() noexcept
		{
			if (IsBig()) return storage_.big_.ptr_;
			else return storage_.small_.data_;
		}

		const_pointer GetBegin() const noexcept
		{
			if (IsBig()) return storage_.big_.ptr_;
			else return storage_.small_.data_;
		}

		pointer GetEnd() noexcept
		{
			if (IsBig()) return storage_.big_.ptr_ + GetBigSize();
			else return storage_.small_.data_ + GetSmallSize();
		}

		const_pointer GetEnd() const noexcept
		{
			if (IsBig()) return storage_.big_.ptr_ + GetBigSize();
			else return storage_.small_.data_ + GetSmallSize();
		}

		size_type GetCapacity() const noexcept
		{
			return (IsBig() ? GetBigCapacity() : SmallMaxLength - 1);
		}
	};

	template<string_type StringT>
	decltype(auto) begin(basic_string<StringT>& str) noexcept
	{
		return str.begin();
	}

	template<string_type StringT>
	decltype(auto) begin(const basic_string<StringT>& str) noexcept
	{
		return str.begin();
	}

	template<string_type StringT>
	decltype(auto) cbegin(const basic_string<StringT>& str) noexcept
	{
		return begin(str);
	}

	template<string_type StringT>
	decltype(auto) end(basic_string<StringT>& str) noexcept
	{
		return str.end();
	}

	template<string_type StringT>
	decltype(auto) end(const basic_string<StringT>& str) noexcept
	{
		return str.end();
	}

	template<string_type StringT>
	decltype(auto) cend(const basic_string<StringT>& str) noexcept
	{
		return end(str);
	}

	template<string_type StringT>
	decltype(auto) size(const basic_string<StringT>& str) noexcept
	{
		return str.size();
	}

	extern template class basic_string<string_type::wide>;
	extern template class basic_string<string_type::utf8>;
	extern template class basic_string<string_type::utf16>;
	extern template class basic_string<string_type::utf32>;

	using wide_string = basic_string<string_type::wide>;
	using utf8_string = basic_string<string_type::utf8>;
	using utf16_string = basic_string<string_type::utf16>;
	using utf32_string = basic_string<string_type::utf32>;

	template<typename IteratorT>
	class string_reader
	{
	public:
		string_reader(IteratorT start, IteratorT last) noexcept
			: start_ { start }, last_ { last }
		{}

		explicit operator bool() const noexcept
		{
			return start_ != last_;
		}

		std::uint32_t operator()() noexcept
		{
			assert(start_ != last_);
			const auto ret = *start_;
			++start_;
			return static_cast<std::uint32_t>(ret);
		}

	private:
		IteratorT start_, last_;
	};

	template<typename StringT>
	decltype(auto) make_reader(const StringT& str) noexcept
	{
		return string_reader<iterator_t<StringT>>{begin(str), end(str)};
	}


	template<typename ReaderT>
	class utf8_decoder
	{
	public:
		utf8_decoder(ReaderT reader) noexcept
			: reader_ { reader }
		{}

		explicit operator bool() const noexcept
		{
			return !!reader_;
		}

		std::uint32_t operator()()
		{
			//0 ~ 10000(16)：1
			//10000(16) ~ 11000(24)：不合法。
			//11000(24) ~ 11100(28)：2
			//11100(28) ~ 11110(30)：3
			//11110(30)：4
			//11111(31)：不合法。
			static constexpr std::uint8_t MaskTable[32] = {
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
				0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 4, 0
			};
			auto codePoint = reader_();
			const auto byteCount = MaskTable[codePoint >> 3];
			if (byteCount == 0)
				throw std::invalid_argument { "invalid UTF-8 sequence!" };
			else
			{
				static constexpr std::uint32_t NonleadingBytesMask = 0b10111111u;
				const auto Unroll = [&]()
				{
					const auto nextPoint = reader_();
					if ((nextPoint & NonleadingBytesMask) != nextPoint) throw std::invalid_argument { "invalid UTF-8 sequence!" };
					codePoint <<= 6;
					codePoint |= (nextPoint & 0x3Fu);
				};
				switch (byteCount)
				{
				case 1:
					break;
				case 2:
					codePoint &= 0b00011111u;
					Unroll();
					break;
				case 3:
					codePoint &= 0b00001111u;
					Unroll();
					Unroll();
					break;
				case 4:
					codePoint &= 0b00000111u;
					Unroll();
					Unroll();
					Unroll();
					break;
				default:
					break;
				}
				return codePoint;
			}
		}

	private:
		ReaderT reader_;
	};

	template<typename ReaderT>
	utf8_decoder<ReaderT> make_utf8_decoder(ReaderT reader) noexcept
	{
		return { reader };
	}

	template<typename ReaderT>
	class utf8_encoder
	{
	public:
		utf8_encoder(ReaderT reader) noexcept
			: reader_ { reader }
		{}

		explicit operator bool() const noexcept
		{
			return pendingBytes_ != 0 || !!reader_;
		}

		std::uint32_t operator()()
		{
			if (pendingBytes_ != 0)
			{
				const auto ret = pendingBytes_ & 0xFFu;
				pendingBytes_ >>= 8;
				return ret ;
			}

			auto codePoint = reader_();
			//UTF-8 的每个 UTF32 code point 最多 21 位。
			if ((codePoint & 0x1FFFFFu) != codePoint) throw std::invalid_argument { "invalid UTF-32 code point!" };
			//从大到小测起。

			const auto Unroll = [&]()
			{
				pendingBytes_ <<= 8;
				pendingBytes_ |= ((codePoint & 0b11'1111u) | 0b1000'0000u);
				codePoint >>= 6;				
			};

			if ((codePoint >> 16) != 0)
			{
				//4 Bytes
				//取前 3 位返回。
				Unroll();
				Unroll();
				Unroll();
				return codePoint | 0b1111'0000u;
			}
			else if ((codePoint >> 11) != 0)
			{
				//3 Bytes
				//取前 4 位返回。
				Unroll();
				Unroll();
				return codePoint | 0b1110'0000u;;
			}
			else if ((codePoint >> 7) != 0)
			{
				//2 Bytes
				//取前 5 位返回。
				Unroll();
				return codePoint | 0b1100'0000u;
			}
			else
				return codePoint;
		}

	private:
		ReaderT reader_;
		std::uint32_t pendingBytes_ = {};
	};

	template<typename ReaderT>
	utf8_encoder<ReaderT> make_utf8_encoder(ReaderT reader) noexcept
	{
		return { reader };
	}

	template<typename ReaderT>
	class utf16_decoder
	{
	public:
		utf16_decoder(ReaderT reader) noexcept
			: reader_ { reader }
		{}

		explicit operator bool() const noexcept
		{
			return !!reader_;
		}

		//https://tools.ietf.org/html/rfc2781 2.2，十分直白。
		std::uint32_t operator()()
		{
			const auto w1 = reader_();
			if (w1 < 0xD800)
				return w1;
			else
				if (w1 > 0xDFFFu) return w1;
				else
					if (w1 <= 0xDBFFu)
						if (reader_)
						{
							const auto w2 = reader_();
							if (w2 >= 0xDC00u && w2 <= 0xDFFFu)
								return (w1 << 10 | w2);
							else goto NonTrailingError;
						}
						else goto NonTrailingError;
					else
						goto TrailingError;

		TrailingError:
			throw std::invalid_argument { "invalid trailing surrogate!" };
		NonTrailingError:
			throw std::invalid_argument { "invalid Non-trailing surrogate!" };
		}

	private:
		ReaderT reader_;
	};

	template<typename ReaderT>
	utf16_decoder<ReaderT> make_utf16_decoder(ReaderT reader) noexcept
	{
		return { reader };
	}

	template<typename ReaderT>
	class utf16_encoder
	{
	public:
		utf16_encoder(ReaderT reader) noexcept
			: reader_ { reader }
		{}

		explicit operator bool() const noexcept
		{
			return pendingBytes_ != 0 || !!reader_;
		}

		std::uint32_t operator()()
		{
			if (pendingBytes_)
			{
				const auto w2 = pendingBytes_ + 0xDC00u;
				pendingBytes_ = {};
				return w2;
			}
			auto codePoint = reader_();
			if (codePoint > 0x10FFFFu)
				throw std::invalid_argument { "invalid UTF-32 code point" };
			else if (codePoint >= 0x10000u)
			{
				codePoint -= 0x10000u;
				pendingBytes_ = codePoint & 0b1111111111u;
				codePoint |= (codePoint & 0b11111111110000000000u) >> 10;
				codePoint |= 0xD800u;
			}
			return codePoint;
		}

	private:
		ReaderT reader_;
		std::uint32_t pendingBytes_ = {};
	};

	template<typename ReaderT>
	utf16_encoder<ReaderT> make_utf16_encoder(ReaderT reader) noexcept
	{
		return { reader };
	}

	template<typename StringT, typename FilterT>
	void convert(StringT& str, FilterT filter)
	{
		while (filter)
		{
			const auto v = static_cast<value_type_t<StringT>>(filter());
			str.push_back(v);
		}
	}

	utf8_string to_utf8(const wide_string::view_type& wide);
	utf8_string to_utf8(const utf16_string::view_type& wide);
	utf8_string to_utf8(const utf32_string::view_type& wide);

	utf16_string to_utf16(const utf8_string::view_type& utf8);
	utf16_string to_utf16(const utf32_string::view_type& utf8);

	utf32_string to_utf32(const utf8_string::view_type& wide);
	utf32_string to_utf32(const utf16_string::view_type& wide);
	utf32_string to_utf32(const wide_string::view_type& wide);

}