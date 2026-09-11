# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.24 )

target_sources(
  arinc_665_qt

  PUBLIC
    FILE_SET HEADERS
      FILES
        Resources.hpp

  PRIVATE
    Resources.cpp
    arinc_665_qt_resources.qrc )
