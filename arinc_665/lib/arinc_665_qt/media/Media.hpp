// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Namespace Arinc665Qt::Media.
 **/

/**
 * @dir
 * @brief Declaration/ Definition of Namespace Arinc665Qt::Media.
 **/

#ifndef ARINC_665_QT_MEDIA_MEDIA_HPP
#define ARINC_665_QT_MEDIA_MEDIA_HPP

#include <arinc_665_qt/Arinc665Qt.hpp>

/**
 * @brief ARINC 665 %Media Qt Classes.
 *
 * Provides Models and Widgets for @ref Arinc665::Media classes.
 **/
namespace Arinc665Qt::Media {

class MediaSetViewWidget;

class MediaSetWidget;

class DirectoryWidget;

class FileWidget;
class BatchWidget;
class LoadWidget;
class RegularFileWidget;

class MediaSetModel;
class MediaSetsModel;

class BatchesModel;
class BatchInfoModel;

class LoadsModel;
class LoadFilesModel;
class TargetHardwareIdsPositionsModel;

}

#endif
