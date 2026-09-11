// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Commands::CommandRegistryImpl.
 **/

#include "CommandRegistryImpl.hpp"

#include <boost/exception/all.hpp>

#include <boost/program_options/errors.hpp>

#include <cassert>

namespace Commands {

const CommandRegistryImpl::Commands& CommandRegistryImpl::commands() const
{
  return commandsV;
}

void CommandRegistryImpl::command(
  std::string_view name,
  std::string_view description,
  ExecuteFunction executeFunction,
  HelpFunction helpFunction )
{
  commandsV.insert(
    { std::string{ name }, std::make_tuple( std::string{ description }, executeFunction, helpFunction ) } );
}

std::optional< CommandRegistryImpl::Command> CommandRegistryImpl::command( std::string_view name )
{
  const auto commandIt{ commandsV.find( name ) };

  if ( commandIt == commandsV.end() )
  {
    return {};
  }

  return commandIt->second;
}

void CommandRegistryImpl::execute( std::string_view name, const Parameters &parameters )
{
  const auto foundCommand{ command( name ) };

  if ( !foundCommand )
  {
    BOOST_THROW_EXCEPTION( boost::program_options::invalid_option_value{ std::string{ name } } );
  }

  const auto executeFunction{ std::get< 1 >( *foundCommand ) };

  assert( executeFunction );

  executeFunction( parameters );
}

void CommandRegistryImpl::help( std::string_view name )
{
  const auto foundCommand{ command( name ) };

  if ( !foundCommand )
  {
    BOOST_THROW_EXCEPTION( boost::program_options::invalid_option_value{ std::string{ name } } );
  }

  assert( std::get< 2 >( *foundCommand ) );

  std::get< 2>( *foundCommand )();
}

}
