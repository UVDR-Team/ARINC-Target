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
 * @brief Declaration of Class TargetOperatorDefinedDownloadOperation.
 **/

#ifndef ARINC615A_TEST_THA_TARGETOPERATORDEFINEDDOWNLOADOPERATION_HPP
#define ARINC615A_TEST_THA_TARGETOPERATORDEFINEDDOWNLOADOPERATION_HPP

#include "TargetOperation.hpp"

#include "DownloadOperationConfiguration.hpp"

#include <arinc_615a/tftp/clients/Clients.hpp>

#include <arinc_615a/information/DownloadFileStatus.hpp>

#include <arinc_615a/target/OperatorDefinedDownloadOperationHandler.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>

namespace Arinc615aTha {

/**
 * @brief Target Operator Defined Download Operation.
 **/
class TargetOperatorDefinedDownloadOperation final :
  public TargetOperation,
  private Arinc615a::Target::OperatorDefinedDownloadOperationHandler
{
  public:
    /**
     * @brief Initialises the Operator Defined Download Operation.
     *
     * @param[in] finished
     *   Finished handler.
     * @param[in] ioContext
     *   I/O context which can be used for asynchronous operations.
     * @param[in] configuration
     *   Target Data Loader Download Configuration.
     * @param[in] protocol
     *   ARINC 615A target protocol.
     * @param[in] targetId
     *   Target ID.
     * @param[in] statusTransmissionRate
     *   Transmission Rate of Status Information.
     **/
    TargetOperatorDefinedDownloadOperation(
      Finished finished,
      boost::asio::io_context &ioContext,
      const DownloadOperationConfiguration &configuration,
      Arinc615a::Target::Protocol &protocol,
      Arinc615a::TargetId targetId,
      std::chrono::seconds statusTransmissionRate );

    //! Destructor
    ~TargetOperatorDefinedDownloadOperation() override;

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
    //! @copydoc Arinc615a::Target::OperatorDefinedDownloadOperationHandler::initialised
    void initialised() override;

    //! @copydoc Arinc615a::Target::OperatorDefinedDownloadOperationHandler::finished
    void finished( Arinc615a::FinalStatus finalStatus, std::string_view description ) override;

    //! @copydoc Arinc615a::Target::OperatorDefinedDownloadOperationHandler::abortRequest
    void abortRequest( Arinc615a::AbortRequest abortRequest ) override;

    /**
     * @copydoc Arinc615a::Target::OperatorDefinedDownloadOperationHandler::status
     *
     * Decodes and prints status.
     * If load list has not been sent, send it.
     **/
    void status( const Arinc615a::Information::DownloadStatus &status ) override;

    //! @copydoc Arinc615a::Target::OperatorDefinedDownloadOperationHandler::downloadingAnswer
    void downloadingAnswer( const Arinc615a::Information::DownloadFiles &files ) override;

    /**
     * @brief Send Load List.
     **/
    void sendLoadList();

    /**
     * @brief Send next File in List.
     **/
    void sendFile();

    /**
     * @brief Option Negotiation Operation for File Transmission.
     *
     * Negotiates the additional options received from the Host TFTP Server.
     *
     * @param[in] providedPartNumber
     *   Part Number Option as sent to the DLA.
     * @param[in] providedCheckValue
     *   Check Value Option as sent to the DLA.
     * @param[in] partNumber
     *   Received ARINC 615A Part Number Option.
     * @param[in] checksum
     *   Received ARINC 615A Checksum Option.
     *
     * @return If Option Negotiation was successful.
     **/
    [[nodiscard]] bool fileOptionsNegotiation(
      std::string_view providedPartNumber,
      const Arinc645::CheckValue &providedCheckValue,
      std::string_view partNumber,
      const Arinc645::CheckValue &checksum );

    /**
     * @brief Handler for transmitted file.
     *
     * @param[in] status
     *   Transfer status.
     **/
    void fileCompleted( Arinc615a::Tftp::TransferStatus status );

    //! THA configuration
    const DownloadOperationConfiguration &configurationV;

    //! ARINC 615A Target Operation
    Arinc615a::Target::OperatorDefinedDownloadOperationPtr operationV;

    //! Status of Abort Request
    std::optional< Arinc615a::AbortRequest > abortRequestPending;
    //! File List Sent
    bool fileListSent{ false };

    //! Available File List
    std::map< std::string, std::filesystem::path > availableFilesV;
    //! File List Requested from Host
    Arinc615a::Information::DownloadFiles filesV;
    //! Current File Iterator
    Arinc615a::Information::DownloadFiles::iterator currentFileV;

    //! File Operation
    Arinc615a::Tftp::Clients::OperationPtr fileOperationV;
};

}

#endif
