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
 * @brief Declaration of Class Arinc615aTha::TargetInformationOperation.
 **/

#ifndef ARINC615A_TEST_THA_TARGETINFORMATIONOPERATION_HPP
#define ARINC615A_TEST_THA_TARGETINFORMATIONOPERATION_HPP

#include "TargetOperation.hpp"

#include "InformationOperationConfiguration.hpp"

#include <arinc_615a/target/InformationOperationHandler.hpp>

#include <boost/asio/ip/udp.hpp>

#include <thread>

namespace Arinc615aTha {

/**
 * @brief Target Information Operation.
 **/
class TargetInformationOperation final : public TargetOperation, private Arinc615a::Target::InformationOperationHandler
{
  public:
    /**
     * @brief Initialises the Information Operation.
     *
     * @param[in] finished
     *   Finished handler.
     * @param[in] ioContext
     *   I/O context which can be used for asynchronous operations.
     * @param[in] configuration
     *   Target Data Loader Information Configuration.
     * @param[in] protocol
     *   ARINC 615A target protocol.
     * @param[in] targetId
     *   Target ID.
     * @param[in] statusTransmissionRate
     *   Transmission Rate of Status Information.
     **/
    TargetInformationOperation(
      Finished finished,
      boost::asio::io_context &ioContext,
      const InformationOperationConfiguration &configuration,
      Arinc615a::Target::Protocol &protocol,
      Arinc615a::TargetId targetId,
      std::chrono::seconds statusTransmissionRate );

    //! Destructor
    ~TargetInformationOperation() override;

    //! @copydoc TargetOperation::initialise
    void initialise(
      boost::asio::ip::udp::endpoint hostAddress,
      Tftp::Packets::TftpOptions clientTftpOptions,
      std::optional< uint16_t > port ) override;

    //! @copydoc TargetOperation::abort
    void abort() override;

    // make inherited method visible
    using TargetOperation::finished;

  private:
    //! @copydoc Arinc615a::Target::InformationOperationHandler::initialised
    void initialised() override;

    //! @copydoc Arinc615a::Target::InformationOperationHandler::finished
    void finished( Arinc615a::FinalStatus finalStatus, std::string_view description ) override;

    //! @copydoc Arinc615a::Target::InformationOperationHandler::abortRequest
    void abortRequest( Arinc615a::AbortRequest abortRequest ) override;

    //! @copydoc Arinc615a::Target::InformationOperationHandler::status
    void status( const Arinc615a::Information::InformationStatus &status ) override;

    /**
     * @brief Transmit Target Configuration.
     **/
    void transmitConfiguration();

    //! THA Configuration
    const InformationOperationConfiguration &configurationV;

    //! ARINC 615A Target Operation
    Arinc615a::Target::InformationOperationPtr operationV;
};

}

#endif
