# SPDX-License-Identifier: MPL-2.0

# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required( VERSION 3.24 )

target_sources(
  arinc_665_qt

  PUBLIC
    FILE_SET HEADERS
      FILES
        ImportMediaSetSettingsPage.hpp
        ImportMediaSetWizard.hpp
        ImportMediaSetXmlSettingsPage.hpp
        ImportMediaSetXmlWizard.hpp
        MediaSetManager.hpp
        MediaSetManagerSettingsDialog.hpp
        MediaSetManagerWindow.hpp
        ViewMediaSetDialog.hpp

  PRIVATE
    ImportMediaSetSettingsPage.cpp
    ImportMediaSetSettingsPage.ui
    ImportMediaSetWizard.cpp
    ImportMediaSetWizard.ui
    ImportMediaSetXmlSettingsPage.cpp
    ImportMediaSetXmlSettingsPage.ui
    ImportMediaSetXmlWizard.cpp
    ImportMediaSetXmlWizard.ui
    MediaSetManagerSettingsDialog.cpp
    MediaSetManagerSettingsDialog.ui
    MediaSetManagerWindow.cpp
    MediaSetManagerWindow.ui
    ViewMediaSetDialog.cpp
    ViewMediaSetDialog.ui )
