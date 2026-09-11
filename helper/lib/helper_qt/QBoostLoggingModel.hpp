// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class HelperQt::QBoostLoggingModel.
 **/

#ifndef HELPER_QT_QBOOSTLOGGINGMODEL_HPP
#define HELPER_QT_QBOOSTLOGGINGMODEL_HPP

#include <boost/log/sinks/basic_sink_backend.hpp>
#include <boost/log/sinks/frontend_requirements.hpp>

namespace HelperQt {

//! Boost Log Qt Model
class QBoostLoggingModel :
  public boost::log::sinks::basic_sink_backend<
    boost::log::sinks::synchronized_feeding >
{
  public:
    /**
     * @brief The function consumes the log records that come from the frontend
     * @param record
     **/
    void consume( boost::log::record_view const& record );
};

}
#endif
