// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Qt::DecompileMediaSetWizard.
 **/

#ifndef ARINC_665_QT_DECOMPILE_MEDIA_SET_DECOMPILEMEDIASETWIZARD_HPP
#define ARINC_665_QT_DECOMPILE_MEDIA_SET_DECOMPILEMEDIASETWIZARD_HPP

#include <arinc_665_qt/decompile_media_set/DecompileMediaSet.hpp>

#include <arinc_665_qt/media/Media.hpp>

#include <arinc_665/utils/Utils.hpp>

#include <QWizard>

#include <memory>

namespace Arinc665Qt {

namespace Ui{
class DecompileMediaSetWizard;
}

/**
 * @brief Decompile ARINC 665 %Media Set Wizard.
 *
 * This wizard is used to query the Decompile ARINC 665 Media Set settings from the user.
 *
 * When the dialog is accepted, the caller must initialise the operation.
 *
 * Media Paths are accessible via the MediaPathsModel.
 * The Check File Integrity Setting is provided via the signal @ref checkFileIntegrity.
 **/
class ARINC_665_QT_EXPORT DecompileMediaSetWizard final : public QWizard
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Decompile Media Set Wizard.
     *
     * @param[in] parent
     *   Widget parent.
     **/
    explicit DecompileMediaSetWizard( QWidget * parent = nullptr );

    //! Destructor
    ~DecompileMediaSetWizard() override;

  signals:
    /**
     * @brief Signal emitted when the user adds or delete media paths.
     *
     * @param[in] mediaPaths
     *   Updated Media Paths
     **/
    void mediaPathsChanged( const Arinc665::Utils::MediaPaths &mediaPaths );

    /**
     * @brief Signal emitted when the user hass selected the _check file integrity_ field.
     *
     * @param[in] checkFileIntegrity
     *   Check file integrity indicator.
     **/
    void checkFileIntegrity( bool checkFileIntegrity );

  private:
    //! UI (designer)
    std::unique_ptr< Ui::DecompileMediaSetWizard > uiV;
};

}

#endif
