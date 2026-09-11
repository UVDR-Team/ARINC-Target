// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Qt::CompileMediaSetSettingsPage.
 **/

#ifndef ARINC_665_QT_COMPILE_MEDIA_SET_COMPILEMEDIASETSETTINGSPAGE_HPP
#define ARINC_665_QT_COMPILE_MEDIA_SET_COMPILEMEDIASETSETTINGSPAGE_HPP

#include <arinc_665_qt/compile_media_set/CompileMediaSet.hpp>

#include <arinc_665/utils/Utils.hpp>

#include <QWizardPage>
#include <QFileDialog>

#include <memory>
#include <filesystem>

namespace Arinc665Qt {

namespace Ui{
class CompileMediaSetSettingsPage;
}

/**
 * @brief Compile %Media Set Settings Page.
 *
 * Provides the following inputs:
 *  - XML File,
 *  - Input Base Directory,
 *  - ARINC 665 Version,
 *  - Load Header Creation Policy,
 *  - Batch File Creation Policy, and
 *  - Output directory.
 **/
class ARINC_665_QT_EXPORT CompileMediaSetSettingsPage final : public QWizardPage
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Page.
     *
     * @param[in] parent
     *   Widget parent.
     **/
    explicit CompileMediaSetSettingsPage( QWidget * parent = nullptr );

    //! Destructor
    ~CompileMediaSetSettingsPage() override;

    /**
     * @brief Indicates if the page is completed.
     *
     * Checks all input fields for validity.
     *
     * @return if page is completed
     **/
    [[nodiscard]] bool isComplete() const override;

    /**
     * @brief Initialises the page with its defaults.
     *
     * @param[in] defaults
     *   Media Set Defaults
     **/
    void defaults( const Arinc665::Utils::MediaSetDefaults &defaults );

  signals:
    /**
     * @brief Emitted when a ARINC 665 Media Set XML file has been selected.
     *
     * @param[in] xmlFile
     *   Selected ARINC 665 Media Set XML file.
     **/
    void xmlFile( std::filesystem::path xmlFile );

    /**
     * @brief Emitted when an input directory has been selected.
     *
     * @param[in] directory
     *   Selected input directory.
     **/
    void inputDirectory( std::filesystem::path directory );

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
     *   Defines if Batch Files are created by exporter or pre-existing ones are used.
     **/
    void createBatchFiles( Arinc665::Utils::FileCreationPolicy createBatchFiles );

    /**
     * @brief Emitted the Create Load Header Files Flag.
     *
     * @param[in] createLoadHeaderFiles
     *   Defines if Load Header Files are created by exporter or pre-existing
     *   ones are used.
     **/
    void createLoadHeaderFiles( Arinc665::Utils::FileCreationPolicy createLoadHeaderFiles );

    /**
     * @brief Emitted when an output directory has been selected.
     *
     * @param[in] directory
     *   Selected output directory.
     **/
    void outputDirectory( std::filesystem::path directory );

  private slots:
    /**
     * @brief Slot called when output directory has been selected.
     *
     * @param[in] file
     *   Select file.
     **/
    void outputDirectorySelected( const QString &file );

  private:
    //! UI (designer)
    std::unique_ptr< Ui::CompileMediaSetSettingsPage > ui;
    //! Select Output Directory Dialog
    std::unique_ptr< QFileDialog > selectOutputDirectoryDialog;
};

}

#endif
