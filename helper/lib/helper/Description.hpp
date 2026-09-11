// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Template Class Description.
 **/

#ifndef HELPER_DESCRIPTION_HPP
#define HELPER_DESCRIPTION_HPP

#include <helper/Helper.hpp>
#include <helper/DescriptionEntry.hpp>

#ifdef _MSC_VER
// reduce names for boost::multi_index. See
// http://www.boost.org/doc/libs/1_63_0/libs/multi_index/doc/compiler_specifics.html
#undef BOOST_MULTI_INDEX_LIMIT_INDEXED_BY_SIZE
#define BOOST_MULTI_INDEX_LIMIT_INDEXED_BY_SIZE 3

#undef BOOST_MULTI_INDEX_LIMIT_TAG_SIZE
#define BOOST_MULTI_INDEX_LIMIT_TAG_SIZE 1

#undef BOOST_MULTI_INDEX_LIMIT_COMPOSITE_KEY_SIZE
#define BOOST_MULTI_INDEX_LIMIT_COMPOSITE_KEY_SIZE 1
#endif

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

#include <boost/exception/exception.hpp>

#include <concepts>
#include <exception>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

namespace Helper {

//! Description Not Found Exception
class DescriptionNotFoundException : public std::exception, public boost::exception
{
  public:
    /**
     * @brief Returns a short exception description.
     *
     * @return Exception description.
     **/
    char const * what() const noexcept override;
};

/**
 * @brief Is Derived from DescriptionEntry Concept.
 *
 * Requires that @p DescT inherits from DescriptionEntry< EnumT, ValueT >.
 *
 * @tparam DescT
 *   Description Type
 * @tparam EnumT
 *   Enumeration Type
 * @tparam ValueT
 *   Value Type
 **/
template< typename DescT, typename EnumT, typename ValueT >
concept DerivedFromDescriptionEntry = std::is_base_of_v< DescriptionEntry< EnumT, ValueT >, DescT >;

/**
 * @brief Template class to give a description to an enumeration
 *
 * This class maps an enumeration value to a description type, which contains a name (string) and a value (might not be
 * the enum-value).
 *
 * @tparam SingletonT
 *   Derived class name for Singleton generation.
 * @tparam EnumT
 *   Enumeration, which is used as key.
 * @tparam ValueT
 *   Value, which is mapped to the enum value
 * @tparam DescEntryT
 *   Description of value
 **/
template<
  typename SingletonT,
  Enumeration EnumT,
  Value ValueT = typename std::underlying_type_t< EnumT >,
  DerivedFromDescriptionEntry< EnumT, ValueT > DescEntryT = DescriptionEntry< EnumT, ValueT > >
class Description
{
  public:
    //! Enumeration Type
    using Enum = EnumT;
    //! Value Type
    using Value = ValueT;
    //! Description Entry Type
    using DescriptionEntry = DescEntryT;

    struct ByName;
    struct ByEnum;
    struct ByValue;

    //! Description Index Type - we directly link to base class
    using DescriptionIndex = boost::multi_index_container<
      DescEntryT,
      boost::multi_index::indexed_by<

        // Index 0 - Name
        boost::multi_index::ordered_unique<
          boost::multi_index::tag< ByName >,
          boost::multi_index::member<
            Helper::DescriptionEntry< EnumT, ValueT >,
            const std::string,
            &Helper::DescriptionEntry< EnumT, ValueT >::name >,
          // allow comparison against compatible types
          std::less< > >,

        // Index 1 - Enumeration
        boost::multi_index::ordered_unique<
          boost::multi_index::tag< ByEnum >,
          boost::multi_index::member<
            Helper::DescriptionEntry< EnumT, ValueT >,
            const typename DescEntryT::EnumType,
            &Helper::DescriptionEntry< EnumT, ValueT >::enumeration > >,

        // Index 2 - Value
        boost::multi_index::ordered_unique<
          boost::multi_index::tag< ByValue >,
          boost::multi_index::member<
            Helper::DescriptionEntry< EnumT, ValueT >,
            const typename DescEntryT::ValueType,
            &Helper::DescriptionEntry< EnumT, ValueT >::value> >
      > >;

    //! Iterator Type
    using Iterator = typename DescriptionIndex::template index< ByEnum >::type::const_iterator;

    /**
     * @brief Get the Single Instance.
     *
     * @return Constant reference to instance.
     **/
    [[nodiscard]] static const SingletonT& instance();

    /**
     * @name Description Access
     *
     * @{
     **/

    /**
     * @brief Returns the beginning of the enum index.
     *
     * @return Begin of the enum index.
     **/
    [[nodiscard]] Iterator begin() const;

    /**
     * @brief Returns the end of the enum index.
     *
     * @return End of the enum index.
     **/
    [[nodiscard]] Iterator end() const;

    /**
     * @brief Returns description map.
     *
     * @return Constant reference to description map.
     **/
    [[nodiscard]] const DescriptionIndex& descriptions() const;

    /**
     * @brief Returns the description for the given enumeration value.
     *
     * @param[in] enumeration
     *   Enumeration Value
     *
     * @return Constant reference to description.
     *
     * @throw DescriptionNotFoundException
     *   If no entry is found.
     **/
    [[nodiscard]] const DescEntryT& description( Enum enumeration ) const;

    /**
     * @brief Returns the description for the given value.
     *
     * @param[in] value
     *   Value
     *
     * @return Constant reference to description.
     *
     * @throw DescriptionNotFoundException
     *   If no entry is found.
     **/
    [[nodiscard]] const DescEntryT& description( Value value ) const;

    /**
     * @brief Returns the description for the given name.
     *
     * @param[in] name
     *   Name.
     *
     * @return Constant reference to description.
     *
     * @throw DescriptionNotFoundException
     *   If no entry is found.
     **/
    [[nodiscard]] const DescEntryT& description( std::string_view name ) const;

    /** @} **/

    /**
     * @name Value Access
     *
     * @{
     **/

    /**
     * @brief Search for value of the given enum.
     *
     * @param[in] enumeration
     *   Enumeration value
     *
     * @return Value for the given enum.
     * @retval {}
     *   When @p enumeration does not map to any enumeration.
     **/
    [[nodiscard]] std::optional< Value > value( Enum enumeration ) const;

    /**
     * @brief Search for value of the given name.
     *
     * @param[in] name
     *   Name.
     *
     * @return The value for the given name.
     * @retval {}
     *   When @p name does not map to any enumeration.
     **/
    [[nodiscard]] std::optional< Value > value( std::string_view name ) const;

    /** @} **/

    /**
     * @name Enumeration Access
     *
     * @{
     **/

    /**
     * @brief Reverse search of enumeration value for given value.
     *
     * @param[in] value
     *   Value to search for.
     *
     * @return  found value or InvalidE if not found.
     * @retval {}
     *   When value does not map to any enumeration.
     **/
    [[nodiscard]] std::optional< Enum > enumeration( Value value ) const;

    /**
     * @brief Reverse search of enumeration value for the given Name.
     *
     * @param[in] name
     *   Name to search for.
     *
     * @return The found value or InvalidE if not found.
     * @retval {}
     *   When name does not name to any enumeration.
     **/
    [[nodiscard]] std::optional< Enum > enumeration( std::string_view name ) const;

    /** @} **/

    /**
     * @name Name Access
     *
     * @{
     **/

    /**
     * @brief Search for the name of the given value.
     *
     * @param[in] value
     *   Value to search for.
     *
     * @return The name for the given enum.
     * @retval {}
     *   When @p value is invalid.
     **/
    [[nodiscard]] std::string_view name( Value value ) const;

    /**
     * @brief Search for the name of the given enum.
     *
     * @param[in] enumeration
     *   Enum value
     *
     * @return The name for the given enum.
     * @retval {}
     *   When @p enumeration is invalid.
     **/
    [[nodiscard]] std::string_view name( Enum enumeration ) const;

    /** @} **/

  protected:
    //! Default Constructs Description.
    Description() = default;

    /**
     * @brief Constructs a description from the given description list.
     *
     * @param[in] descriptions
     *   Description initialiser list.
     **/
    Description( std::initializer_list< DescEntryT > descriptions );

    //! @brief Destructor.
    virtual ~Description() noexcept = default;

  private:
    //! Description
    DescriptionIndex descriptionV{};
};

}

#include <helper/Description.ipp>

#endif
