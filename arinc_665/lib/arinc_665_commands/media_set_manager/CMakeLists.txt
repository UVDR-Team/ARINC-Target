# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.24 )

target_sources(
  arinc_665_commands

  PUBLIC
    FILE_SET HEADERS
      FILES
        CreateMediaSetManagerCommand.hpp
        ImportMediaSetCommand.hpp
        ImportMediaSetXmlCommand.hpp
        ListBatchesCommand.hpp
        ListLoadsCommand.hpp
        ListMediaSetsCommand.hpp
        MediaSetManager.hpp
        RemoveMediaSetCommand.hpp

  PRIVATE
    CreateMediaSetManagerCommand.cpp
    ImportMediaSetCommand.cpp
    ImportMediaSetXmlCommand.cpp
    ListBatchesCommand.cpp
    ListLoadsCommand.cpp
    ListMediaSetsCommand.cpp
    MediaSetManager.cpp
    RemoveMediaSetCommand.cpp )
