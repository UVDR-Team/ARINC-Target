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
 * @brief Declaration of Class TargetOperation.
 **/

#ifndef ARINC615A_TEST_THA_TARGETOPERATION_HPP
#define ARINC615A_TEST_THA_TARGETOPERATION_HPP

#include <tftp/packets/Packets.hpp>

#include <tftp/servers/Servers.hpp>

#include <boost/asio/ip/udp.hpp>

#include <functional>

namespace Arinc615aTha {

/**
 * @brief Base class for ARINC 615A THA operations.
 **/
class TargetOperation
{
  public:
    //! Finished Function Callback
    using Finished = std::function< void() >;

    /**
     * @brief Initialises operation.
     *
     * @param[in] finished
     *   Finished handler.
     * @param[in] ioContext
     *   I/O Context.
     **/
    explicit TargetOperation( Finished finished, boost::asio::io_context &ioContext ) :
      finishedV{ std::move( finished ) },
      ioContextV{ ioContext }
    {
    }

    //! Destructor
    virtual ~TargetOperation() = default;

    /**
     * @brief Initialises the operation.
     *
     * @param[in] hostAddress
     *   Host Address
     * @param[in] clientTftpOptions
     *   TFTP Options from Host
     * @param[in] port
     *   Received ARINC 615A Port Option.
     *
     * Returns Immediately.
     **/
    virtual void initialise(
      boost::asio::ip::udp::endpoint hostAddress,
      ::Tftp::Packets::TftpOptions clientTftpOptions,
      std::optional< uint16_t > port ) = 0;

    /**
     * @brief Requests a data loader abort.
     **/
    virtual void abort() = 0;

  protected:
    /**
     * @brief Indicates completion of operation.
     **/
    void finished() const
    {
      if ( finishedV )
      {
        finishedV();
      }
    }

    /**
     * @brief Returns the I/O Context.
     *
     * @return I/O Context.
     **/
    [[nodiscard]] boost::asio::io_context& ioContext()
    {
      return ioContextV;
    }

  private:
    //! Finished callback
    Finished finishedV;
    //! I/O Context
    boost::asio::io_context &ioContextV;
};

}

#endif
