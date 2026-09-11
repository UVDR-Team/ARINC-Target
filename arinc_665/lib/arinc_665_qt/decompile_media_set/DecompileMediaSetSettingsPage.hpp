// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Qt::DecompileMediaSetSettingsPage.
 **/

#ifndef ARINC_665_QT_DECOMPILE_MEDIA_SET_DECOMPILEMEDIASETSETTINGSPAGE_HPP
#define ARINC_665_QT_DECOMPILE_MEDIA_SET_DECOMPILEMEDIASETSETTINGSPAGE_HPP

#include <arinc_665_qt/decompile_media_set/DecompileMediaSet.hpp>

#include <arinc_665/utils/Utils.hpp>

#include <QWizardPage>

#include <memory>

namespace Arinc665Qt {

namespace Ui{
class DecompileMediaSetSettingsPage;
}

/**
 * @brief Decompile %Media Set Settings Page.
 *
 * Provides following inputs:
 *  - Media Paths, and
 *  - Check file integrity.
 **/
class ARINC_665_QT_EXPORT DecompileMediaSetSettingsPage final :
  public QWizardPage
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Page.
     *
     * @param[in] parent
     *   Widget parent.
     **/
    explicit DecompileMediaSetSettingsPage( QWidget * parent = nullptr );

    //! Destructor
    ~DecompileMediaSetSettingsPage() override;

    /**
     * @brief Indicates if the page is completed.
     *
     * Checks all input fields for validity.
     *
     * @return if page is completed
     **/
    [[nodiscard]] bool isComplete() const override;

    /**
     * @brief Initialises Page.
     *
     * Clears content of file path model.
     **/
    void initializePage() override;

    /**
     * @brief Initialises the page with its defaults.
     *
     * @param[in] defaults
     *   Media Set Defaults
     **/
    void defaults( const Arinc665::Utils::MediaSetDefaults &defaults );

  signals:
    /**
     * @brief Signal emitted when the user adds or delete media paths.
     *
     * @param[in] mediaPaths
     *   Updated Media Paths
     **/
    void mediaPathsChanged( const Arinc665::Utils::MediaPaths &mediaPaths );

    /**
     * @brief Signal emitted when the check file integrity field has been selected by user.
     *
     * @param[in] checkFileIntegrity
     *   Check file integrity indicator.
     **/
    void checkFileIntegrity( bool checkFileIntegrity );

  private slots:
    /**
     * @brief Slot called, when the file integrity changed checkbox has been changed.
     *
     * Check state is determined and signal @ref checkFileIntegrity() is emitted.
     *
     * @param[in] state
     *   Check State.
     **/
    void checkFileIntegrityStateChanged( Qt::CheckState state );

  private:
    //! UI (designer)
    std::unique_ptr< Ui::DecompileMediaSetSettingsPage > ui;
};

}

#endif
