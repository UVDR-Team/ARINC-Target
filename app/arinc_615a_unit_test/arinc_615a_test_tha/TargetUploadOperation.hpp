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
 * @brief Declaration of Class TargetUploadOperation.
 **/

#ifndef ARINC615A_TEST_THA_TARGETUPLOADOPERATION_HPP
#define ARINC615A_TEST_THA_TARGETUPLOADOPERATION_HPP

#include "TargetOperation.hpp"

#include "UploadOperationConfiguration.hpp"

#include <arinc_615a/tftp/clients/Clients.hpp>

#include <arinc_615a/information/UploadLoadStatus.hpp>
#include <arinc_615a/information/UploadLoad.hpp>

#include <arinc_615a/target/UploadOperationHandler.hpp>

#include <arinc_665/files/LoadFileInfo.hpp>

#include <tftp/files/Files.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>

#include <atomic>

namespace Arinc615aTha {

/**
 * @brief Upload Operation on Target Side.
 *
 * - execute operation
 * - set status in progress
 * - Wait for List of Loads
 * - for each load receive Load Header
 * - for each load receive files
 * - set completed status
 **/
class TargetUploadOperation final :
  public TargetOperation,
  private Arinc615a::Target::UploadOperationHandler
{
  public:
    /**
     * @brief Initialises the Upload Operation.
     *
     * @param[in] finished
     *   Finished handler
     * @param[in] ioContext
     *   I/O context which can be used for asynchronous operations.
     * @param[in] configuration
     *   Target Data Loader Upload Configuration.
     * @param[in] protocol
     *   ARINC 615A target protocol.
     * @param[in] targetId
     *   Target ID
     * @param[in] statusTransmissionRate
     *   Transmission Rate of Status Information.
     **/
    TargetUploadOperation(
      Finished finished,
      boost::asio::io_context &ioContext,
      const UploadOperationConfiguration &configuration,
      Arinc615a::Target::Protocol &protocol,
      Arinc615a::TargetId targetId,
      std::chrono::seconds statusTransmissionRate );

    //! Destructor
    ~TargetUploadOperation() override;

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
    //! @copydoc Arinc615a::Target::UploadOperationHandler::initialised
    void initialised() override;

    //! @copydoc Arinc615a::Target::UploadOperationHandler::finished
    void finished( Arinc615a::FinalStatus finalStatus, std::string_view description ) override;

    //! @copydoc Arinc615a::Target::UploadOperationHandler::abortRequest
    void abortRequest( Arinc615a::AbortRequest abortRequest ) override;

    //! @copydoc Arinc615a::Target::UploadOperationHandler::status
    void status( const Arinc615a::Information::UploadStatus &status ) override;

    /**
     * @copydoc Arinc615a::Target::UploadOperationHandler::loadList
     *
     * This handler decodes all advertised loads and starts download of it.
     **/
    void loadList( const Arinc615a::Information::UploadLoads &loads ) override;

    /**
     * @brief Receive and handle next Load Header.
     **/
    void receiveLoadHeader();

    /**
     * @brief Option Negotiation Operation for Load Upload Header File Transmission.
     *
     * Negotiates the additional options received from the Target TFTP Server.
     *
     * @param[in] providedPartNumber
     *   Part Number Option as sent to the DLA.
     * @param[in] partNumber
     *   Received ARINC 615A Part Number Option.
     * @param[in] checksum
     *   Received ARINC 615A Checksum Option.
     *
     * @return If Option Negotiation was successful.
     **/
    [[nodiscard]] bool uploadHeaderFileOptionsNegotiation(
      std::string_view providedPartNumber,
      std::string_view partNumber,
      const Arinc645::CheckValue &checksum );

    /**
     * @brief Handler for received header file.
     *
     * decodes load header and initiates file transfers.
     *
     * @param[in] loadHeader
     *   Received load header file.
     * @param[in] status
     *   Transfer status.
     **/
    void uploadHeaderFileCompleted( Tftp::Files::MemoryFilePtr loadHeader, Arinc615a::Tftp::TransferStatus status );

    /**
     * @brief Receive next File in List.
     **/
    void receiveFile();

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
     * @brief Handler for received data/ support file.
     *
     * @param[in] file
     *   Received file data.
     * @param[in] status
     *   Transfer status.
     **/
    void fileCompleted( Tftp::Files::StreamFilePtr file, Arinc615a::Tftp::TransferStatus status );

    //! THA configuration
    const UploadOperationConfiguration &configurationV;

    //! ARINC 615A Target Operation
    Arinc615a::Target::UploadOperationPtr operationV;

    //! List of Loads to process
    Arinc615a::Information::UploadLoads loadsV;
    //! Current Load
    Arinc615a::Information::UploadLoads::iterator currentLoadV;
    //! Load Header Operation
    Arinc615a::Tftp::Clients::OperationPtr loadHeaderOperationV;

    //! Files (Data + Support)
    Arinc665::Files::LoadFilesInfo filesV;
    //! Current File
    Arinc665::Files::LoadFilesInfo::iterator currentFileV;

    //! File Operation
    Arinc615a::Tftp::Clients::OperationPtr fileOperationV;
};

}

#endif
