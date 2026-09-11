# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.24 )

target_sources(
  arinc_665_qt

  PUBLIC
    FILE_SET HEADERS
      FILES
        BatchesModel.hpp
        BatchInfoModel.hpp
        BatchWidget.hpp
        DirectoryWidget.hpp
        FileWidget.hpp
        LoadFilesModel.hpp
        LoadsModel.hpp
        LoadWidget.hpp
        Media.hpp
        MediaSetModel.hpp
        MediaSetsModel.hpp
        MediaSetViewWidget.hpp
        MediaSetWidget.hpp
        RegularFileWidget.hpp
        TargetHardwareIdsPositionsModel.hpp

  PRIVATE
    BatchesModel.cpp
    BatchInfoModel.cpp
    BatchWidget.cpp
    BatchWidget.ui
    DirectoryWidget.cpp
    DirectoryWidget.ui
    FileWidget.cpp
    FileWidget.ui
    LoadFilesModel.cpp
    LoadsModel.cpp
    LoadWidget.cpp
    LoadWidget.ui
    MediaSetModel.cpp
    MediaSetsModel.cpp
    MediaSetViewWidget.cpp
    MediaSetViewWidget.ui
    MediaSetWidget.cpp
    MediaSetWidget.ui
    RegularFileWidget.cpp
    RegularFileWidget.ui
    TargetHardwareIdsPositionsModel.cpp )
