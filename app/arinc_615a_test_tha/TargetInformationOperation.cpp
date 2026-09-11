// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @ingroup arinc_615a_test_tha
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class TargetInformationOperation.
 **/

#include "TargetInformationOperation.hpp"

#include "TargetDataLoaderConfiguration.hpp"

#include <arinc_615a/target/InformationOperation.hpp>
#include <arinc_615a/target/Protocol.hpp>
#include <arinc_615a/target/OperationConfiguration.hpp>

#include <arinc_615a/information/Status.hpp>
#include <arinc_615a/information/TargetHardware.hpp>
#include <arinc_615a/information/PartNumber.hpp>

#include <arinc_615a/tftp/Arinc615aOptions.hpp>

#include <arinc_615a/StatusCode.hpp>
#include <arinc_615a/StatusCodeDescription.hpp>

#include <tftp/files/StreamFile.hpp>

#include <tftp/packets/TftpOptions.hpp>

#include <tftp/clients/Operation.hpp>

#include <spdlog/spdlog.h>

#include <boost/exception/diagnostic_information.hpp>

#include <fstream>

namespace Arinc615aTha {

TargetInformationOperation::TargetInformationOperation(
  Finished finished,
  boost::asio::io_context &ioContext,
  const InformationOperationConfiguration &configuration,
  Arinc615a::Target::Protocol &protocol,
  Arinc615a::TargetId targetId,
  std::chrono::seconds statusTransmissionRate ) :
  TargetOperation{ std::move( finished ), ioContext },
  configurationV{ configuration },
  operationV{ protocol.informationOperation(
    Arinc615a::Target::InformationOperationConfiguration{
      .handler = *this,
      .targetId = std::move( targetId ),
      .statusTransmissionRate = statusTransmissionRate } ) }
{
}

TargetInformationOperation::~TargetInformationOperation() = default;

void TargetInformationOperation::initialise(
  boost::asio::ip::udp::endpoint hostAddress,
  Tftp::Packets::TftpOptions clientTftpOptions,
  const std::optional< uint16_t > port )
{
  assert( operationV );

  try
  {
    operationV->start( std::move( hostAddress ), std::move( clientTftpOptions ), port );
  }
  catch ( const boost::exception &e )
  {
    SPDLOG_ERROR( "Error during Information operation: {}", boost::diagnostic_information( e ) );
  }
  catch ( const std::exception &e )
  {
    SPDLOG_ERROR( "Error during Information operation: {}", boost::diagnostic_information( e ) );
  }
  catch ( ... )
  {
    SPDLOG_ERROR( "Error during Information operation" );
  }
}

void TargetInformationOperation::abort()
{
  assert( operationV );

  operationV->finished( Arinc615a::FinalStatus::AbortedByTargetHardware, "THA Abort" );
}

void TargetInformationOperation::initialised()
{
  assert( operationV );

  // send configuration
  boost::asio::post( ioContext(), std::bind_front( &TargetInformationOperation::transmitConfiguration, this ) );
}

void TargetInformationOperation::finished( const Arinc615a::FinalStatus finalStatus, std::string_view description )
{
  SPDLOG_INFO(
    "Operation finished: {} '{}'",
    Arinc615a::StatusCodeDescription::instance().name( Arinc615a::statusCode( finalStatus ) ),
    description );

  finished();
}

void TargetInformationOperation::abortRequest( const Arinc615a::AbortRequest abortRequest )
{
  SPDLOG_INFO( "Abort Request from host" );

  // directly abort operation
  operationV->finished( Arinc615a::finalStatus( abortRequest ) );
}

void TargetInformationOperation::status( const Arinc615a::Information::InformationStatus &status )
{
  SPDLOG_INFO(
    "Status:\n"
    "\tCounter:         {}\n"
    "\tStatus:          {} ({})\n"
    "\tDescription:     '{}'\n"
    "\tException Timer: {}\n"
    "\tEstimated Time:  {}\n",
    status.counter(),
    Arinc615a::StatusCodeDescription::instance().name( status.code() ),
    std::to_underlying( status.code() ),
    status.description(),
    status.exceptionTimer(),
    status.estimatedTime() );
}

void TargetInformationOperation::transmitConfiguration()
{
  operationV->targetInformation( configurationV.targetsHardware, configurationV.listCheckValue );
}

}
