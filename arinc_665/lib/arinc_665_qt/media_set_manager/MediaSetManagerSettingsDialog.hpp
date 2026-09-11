// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Qt::MediaSetManager::MediaSetManagerSettingsDialog.
 **/

#ifndef ARINC_665_QT_MEDIA_SET_MANAGER_MEDIASETMANAGERSETTINGSDIALOG_HPP
#define ARINC_665_QT_MEDIA_SET_MANAGER_MEDIASETMANAGERSETTINGSDIALOG_HPP

#include <arinc_665_qt/media_set_manager/MediaSetManager.hpp>

#include <arinc_665/utils/Utils.hpp>

#include <QDialog>

#include <memory>

namespace Arinc665Qt::MediaSetManager {

namespace Ui{
class MediaSetManagerSettingsDialog;
}

/**
 * @brief Media Set Manager Settings Dialog.
 **/
class ARINC_665_QT_EXPORT MediaSetManagerSettingsDialog  final : public QDialog
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Media Set Manager Settings Dialog.
     *
     * @param[in] parent
     *   Widget parent.
     **/
    explicit MediaSetManagerSettingsDialog( QWidget * parent = nullptr );

    //! Destructor
    ~MediaSetManagerSettingsDialog() override;

    /**
     * @brief Returns the Configuration from the settings.
     *
     * @return Configuration
     **/
    [[nodiscard]] Arinc665::Utils::MediaSetDefaults configuration() const;

    /**
     * @brief Loads Dialog from configuration
     *
     * @param[in] defaults
     *   Configuration to load from
     **/
    void configuration( const Arinc665::Utils::MediaSetDefaults &defaults );

  private:
    //! UI (designer)
    std::unique_ptr< Ui::MediaSetManagerSettingsDialog > uiV;
    //! Support ARINC 665 Version Model
    std::unique_ptr< SupportedArinc665VersionModel > supportedArinc665VersionModelV;
    //! File Creation Policy Model (used for load header and batch files)
    std::unique_ptr< FileCreationPolicyModel > fileCreationPolicyModelV;
};

}

#endif
