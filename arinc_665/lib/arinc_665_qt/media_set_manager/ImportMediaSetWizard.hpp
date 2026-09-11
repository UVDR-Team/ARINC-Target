// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Qt::MediaSetManager::ImportMediaSetWizard.
 **/

#ifndef ARINC_665_QT_MEDIA_SET_MANAGER_IMPORTMEDIASETWIZARD_HPP
#define ARINC_665_QT_MEDIA_SET_MANAGER_IMPORTMEDIASETWIZARD_HPP

#include <arinc_665_qt/media_set_manager/MediaSetManager.hpp>

#include <arinc_665/utils/Utils.hpp>

#include <QWizard>

#include <memory>

namespace Arinc665Qt::MediaSetManager {

namespace Ui {
class ImportMediaSetWizard;
}

/**
 * @brief Import ARINC 665 %Media Set Wizard
 *
 * Imports a ARINC 665 Media Set into the Media Set Manager.
 **/
class ARINC_665_QT_EXPORT ImportMediaSetWizard final : public QWizard
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Decompile Media Set Wizard.
     *
     * @param[in] mediaSetManager
     *   Media Set Manager
     * @param[in] parent
     *   Widget parent.
     **/
    explicit ImportMediaSetWizard( Arinc665::Utils::MediaSetManagerPtr mediaSetManager, QWidget * parent = nullptr );

    //! Destructor
    ~ImportMediaSetWizard() override;

  private slots:
    /**
     * @brief Slot called when the user updates the Media Paths.
     *
     * @param[in] mediaPaths
     *   Media Paths
     **/
    void updateMediaPaths( const Arinc665::Utils::MediaPaths &mediaPaths );

    /**
     * @brief Slot called when the user changed the check file integrity handling.
     *
     * @param[in] checkFileIntegrity
     *   Check File Integrity indicator
     **/
    void checkFileIntegrity( bool checkFileIntegrity );

    /**
     * @brief Import Media Set into media set manager
     **/
    void importMediaSet();

  private:
    //! UI (designer)
    std::unique_ptr< Ui::ImportMediaSetWizard > uiV;
    //! Media Set Manager
    Arinc665::Utils::MediaSetManagerPtr mediaSetManagerV;
    //! ARINC 665 Media Set Copier
    Arinc665::Utils::FilesystemMediaSetCopierPtr copierV;
    //! Media Paths
    Arinc665::Utils::MediaPaths mediaPathsV;
    //! Check File Integrity
    bool checkFileIntegrityV{ false };
};

}

#endif
