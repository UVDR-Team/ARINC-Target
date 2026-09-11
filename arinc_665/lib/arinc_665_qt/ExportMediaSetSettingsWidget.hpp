// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Qt::ExportMediaSetSettingsWidget.
 **/

#ifndef ARINC_665_QT_EXPORTMEDIASETSETTINGSWIDGET_HPP
#define ARINC_665_QT_EXPORTMEDIASETSETTINGSWIDGET_HPP

#include <arinc_665_qt/Arinc665Qt.hpp>

#include <arinc_665/utils/Utils.hpp>

#include <QGroupBox>

#include <memory>

namespace Arinc665Qt {

namespace Ui{
class ExportMediaSetSettingsWidget;
}

/**
 * @brief Export ARINC 665 Media Set Settings Widget.
 *
 * Provides a Widget where the user can select:
 *  - ARINC 665 Generator Version,
 *  - Create Batch Files policy, and
 *  - Create Load Header Files policy.
 */
class ARINC_665_QT_EXPORT ExportMediaSetSettingsWidget final : public QGroupBox
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Widget.
     *
     * @param[in] parent
     *   Widget parent.
     **/
    explicit ExportMediaSetSettingsWidget( QWidget * parent = nullptr );

    //! Destructor
    ~ExportMediaSetSettingsWidget() override;

    /**
     * @brief Returns if the widget inputs are valid and completed.
     *
     * @return if the widget inputs are valid and completed.
     **/
    [[nodiscard]] bool completed() const;

    /**
     * @brief Initialises the widgets with its defaults.
     *
     * @param[in] defaults
     *   Media Set Defaults
     **/
    void defaults( const Arinc665::Utils::MediaSetDefaults &defaults );

  signals:
    /**
     * @brief Emitted the ARINC 665 Version Flag.
     *
     * @param[in] version
     *   ARINC 665 version used for exporting.
     **/
    void arinc665Version( Arinc665::SupportedArinc665Version version );

    /**
     * @brief Emitted the Create Batch Files Flag.
     *
     * @param[in] createBatchFiles
     *   Defines, if Batch Files are created by exporter or pre-existing ones are used.
     **/
    void createBatchFiles( Arinc665::Utils::FileCreationPolicy createBatchFiles );

    /**
     * @brief Emitted the Create Load Header Files Flag.
     *
     * @param[in] createLoadHeaderFiles
     *   Defines, if Load Header Files are created by exporter or pre-existing ones are used.
     **/
    void createLoadHeaderFiles( Arinc665::Utils::FileCreationPolicy createLoadHeaderFiles );

  private slots:
    /**
     * @brief ARINC 665 Version Index selected slot.
     *
     * Emits the signal with the correct ARINC 665 Version.
     *
     * @param[in] index
     *   Combo Box Item Index
     **/
    void arinc665VersionIndexSelected( int index );

    /**
     * @brief Create Batch Files Index selected slot.
     *
     * Emits the signal with the correct Create Batch Files value.
     *
     * @param[in] index
     *   Combo Box Item Index
     **/
    void createBatchFilesIndexSelected( int index );

    /**
     * @brief Create Load Header Files Index selected slot.
     *
     * Emits the signal with the correct Create Load Header Files value.
     *
     * @param[in] index
     *   Combo Box Item Index
     **/
    void createLoadHeadersFilesIndexSelected( int index );

  private:
    //! UI (designer)
    std::unique_ptr< Ui::ExportMediaSetSettingsWidget > ui;
    //! Support ARINC 665 Version Model
    std::unique_ptr< SupportedArinc665VersionModel >
      supportedArinc665VersionModelV;
    //! File Creation Policy Model (used for load header and batch files)
    std::unique_ptr< FileCreationPolicyModel > fileCreationPolicyModelV;
};

}

#endif
