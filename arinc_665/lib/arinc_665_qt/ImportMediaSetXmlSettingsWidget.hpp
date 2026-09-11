// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Qt::ImportMediaSetXmlSettingsWidget.
 **/

#ifndef ARINC_665_QT_IMPORTMEDIASETXMLSETTINGSWIDGET_HPP
#define ARINC_665_QT_IMPORTMEDIASETXMLSETTINGSWIDGET_HPP

#include <arinc_665_qt/Arinc665Qt.hpp>

#include <QGroupBox>
#include <QFileDialog>

#include <filesystem>
#include <memory>

namespace Arinc665Qt {

namespace Ui{
class ImportMediaSetXmlSettingsWidget;
}

/**
 * @brief Import ARINC 665 Media Set XML Settings Widget.
 *
 * Provides a Widget where the user can select:
 *  - ARINC 665 Media Set XML, and
 *  - Base Path for files.
 **/
class ARINC_665_QT_EXPORT ImportMediaSetXmlSettingsWidget final :
  public QGroupBox
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Widget.
     *
     * @param[in] parent
     *   Widget parent.
     **/
    explicit ImportMediaSetXmlSettingsWidget( QWidget * parent = nullptr );

    //! Destructor
    ~ImportMediaSetXmlSettingsWidget() override;

    /**
     * @brief Returns if the widget inputs are valid and completed.
     *
     * @return if the widget inputs are valid and completed.
     **/
    [[nodiscard]] bool completed() const;

  signals:
    /**
     * @brief Emitted when a ARINC 665 Media Set XML file has been selected.
     *
     * @param[in] xmlFile
     *   Selected ARINC 665 Media Set XML file.
     **/
    void xmlFile( const std::filesystem::path &xmlFile );

    /**
     * @brief Emitted when an input directory has been selected.
     *
     * @param[in] directory
     *   Selected input directory.
     **/
    void inputDirectory( const std::filesystem::path &directory );

  private slots:
    /**
     * @brief Slot called when the given XML file has been selected.
     *
     * @param[in] file
     *   Selected file.
     **/
    void xmlFileSelected( const QString &file );

    //! Slot called when input directory has been selected.
    void inputDirectorySelected( const QString &file );

  private:
    //! UI (designer)
    std::unique_ptr< Ui::ImportMediaSetXmlSettingsWidget > uiV;
    //! Select XML File Dialog
    std::unique_ptr< QFileDialog > selectXmlFileDialogV;
    //! Select Input Directory Dialog
    std::unique_ptr< QFileDialog > selectInputDirectoryDialogV;
};

}

#endif
