// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Qt::Actions::OpenMediaSetManagerAction.
 **/

#ifndef ARINC_665_QT_ACTIONS_OPENMEDIASETMANAGERACTION_HPP
#define ARINC_665_QT_ACTIONS_OPENMEDIASETMANAGERACTION_HPP

#include <arinc_665_qt/actions/Actions.hpp>

#include <arinc_665_qt/media/Media.hpp>

#include <arinc_665/utils/Utils.hpp>

#include <QFileDialog>
#include <QProgressDialog>
#include <QWidget>
#include <QString>

#include <memory>

namespace Arinc665Qt::Actions {

/**
 * @brief Open %Media Set Manager Action
 *
 * Asks the User for selecting the %Media Set Manager Directory and tries to open it.
 * During loading of the %Media Set Manager, a progress dialog is shown.
 **/
class ARINC_665_QT_EXPORT OpenMediaSetManagerAction : public QObject
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the %Media Set Manager Action.
     *
     * @param[in] parent
     *   Parent Widget
     **/
    explicit OpenMediaSetManagerAction( QWidget * parent = nullptr );

    //! Destructor
    ~OpenMediaSetManagerAction() override;

  public slots:
    /**
     * @brief Queries for %Media Set Configuration.
     *
     * Ask the user for the location of the %Media Set Manager Configuration and loads them.
     **/
    void open();

  signals:
    /**
     * @brief Signal emitted, when the %Media Set Manager is loaded successfully.
     *
     * @param[in] mediaSetManager
     *   Media Set Manager instance.
     **/
    void mediaSetManagerLoaded( const Arinc665::Utils::MediaSetManagerPtr &mediaSetManager );

    /**
     * @brief Signal emitted when the %Media Set Manager could not be loaded.
     **/
    void failed();

    //! Operation cancelled by the user
    void rejected();

  private slots:
    /**
     * @brief Slot called, when user has selected %Media Set Configuration.
     **/
    void directorySelected();

    /**
     * @brief Slot called for new progress information.
     *
     * Updates the progress dialog.
     *
     * @param[in] currentMediaSet
     *   Current %Media Set Counter
     * @param[in] numberOfMediaSets
     *   Number of %Media Sets.
     * @param[in] partNumber
     *   Part Number of current %Media Set.
     **/
    void mediaSetManagerLoadProgress( size_t currentMediaSet, size_t numberOfMediaSets, const QString &partNumber );

  private:
    //! Select %Media Set Manager Directory Dialog
    std::unique_ptr< QFileDialog > selectMediaSetDirectoryDialogV;
    //! Progress Callback Dialog
    QProgressDialog * progressDialogV{ nullptr };
    //! Load %Media Set Action
    std::unique_ptr< LoadMediaSetManagerAction > loadMediaSetManagerActionV;
};

}

#endif
