#pragma once

#include <numeric>
#include <utility>


namespace Yupei
{
    namespace Internal
    {
        template<typename ContainerT, bool IsTemp>
        struct ForWithIndexTag
        {
            std::reference_wrapper<ContainerT> Container;

            ForWithIndexTag(ContainerT& container) noexcept
                : Container{container}
            {}
        };

        template<typename ContainerT>
        struct ForWithIndexTag<ContainerT, true>
        {
            ContainerT Container;

            ForWithIndexTag(ContainerT&& container) noexcept
                : Container{std::move(container)}
            {}
        };

        template<typename ContainerT>
        auto MakeTag(ContainerT& container)
        {
            return ForWithIndexTag<ContainerT, false>{container};
        }

        template<typename ContainerT, typename = std::enable_if_t<std::is_rvalue_reference_v<ContainerT>, int> = 0>
        auto MakeTag(ContainerT&& container)
        {
            return ForWithIndexTag<std::remove_reference_t<ContainerT>, true>{std::move(container)};
        }

        template<typename Container, bool IsTemp, typename Func>
        void operator+ (ForWithIndexTag<Container, IsTemp> tag, Func f)
        {
            using std::begin;
            using std::end;
            auto& container = tag.Container.get();
            std::accumulate(begin(container), end(container), std::size_t{}, [&f](std::size_t index, auto& element)
            {
                f(index, element);
                return ++index;
            });
        }
    }
}

#define For(index, element, ...) \
        Yupei::Internal::MakeTag(__VA_ARGS__) + [&](std::size_t index, element)