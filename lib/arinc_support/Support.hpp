// SPDX-License-Identifier: MPL-2.0
#ifndef ARINC_SUPPORT_SUPPORT_HPP
#define ARINC_SUPPORT_SUPPORT_HPP

#include <functional>
#include <map>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

// Only the types required by the protocol libraries are retained. There is
// no helper library, platform-directory lookup, scheduler or logger here.
namespace ArincSupport {
template<class Enum>
constexpr std::underlying_type_t<Enum> toUnderlying(Enum value) noexcept
{
  return static_cast<std::underlying_type_t<Enum>>(value);
}
struct VersionInformation;
using VersionsInformation = std::map< std::string, VersionInformation, std::less<> >;

template< typename FunctionT, typename... BoundArgsT >
class FrontBinder
{
  public:
    template< typename FunctionU, typename... BoundArgsU >
    explicit FrontBinder( FunctionU&& function, BoundArgsU&&... boundArgs ) :
      functionV{ std::forward< FunctionU >( function ) },
      boundArgsV{ std::forward< BoundArgsU >( boundArgs )... }
    {
    }

    template< typename... CallArgsT >
    decltype(auto) operator()( CallArgsT&&... callArgs )
    {
      return invoke( std::index_sequence_for< BoundArgsT... >{}, std::forward< CallArgsT >( callArgs )... );
    }

    template< typename... CallArgsT >
    decltype(auto) operator()( CallArgsT&&... callArgs ) const
    {
      return invoke( std::index_sequence_for< BoundArgsT... >{}, std::forward< CallArgsT >( callArgs )... );
    }

  private:
    template< std::size_t... Index, typename... CallArgsT >
    decltype(auto) invoke( std::index_sequence< Index... >, CallArgsT&&... callArgs )
    {
      return std::invoke( functionV, std::get< Index >( boundArgsV )..., std::forward< CallArgsT >( callArgs )... );
    }

    template< std::size_t... Index, typename... CallArgsT >
    decltype(auto) invoke( std::index_sequence< Index... >, CallArgsT&&... callArgs ) const
    {
      return std::invoke( functionV, std::get< Index >( boundArgsV )..., std::forward< CallArgsT >( callArgs )... );
    }

    typename std::decay< FunctionT >::type functionV;
    std::tuple< typename std::decay< BoundArgsT >::type... > boundArgsV;
};

template< typename FunctionT, typename... BoundArgsT >
auto bindFront( FunctionT&& function, BoundArgsT&&... boundArgs )
{
  return FrontBinder< FunctionT, BoundArgsT... >{
    std::forward< FunctionT >( function ), std::forward< BoundArgsT >( boundArgs )... };
}
}

#define ARINC_SUPPORT_EXPORT
#endif
