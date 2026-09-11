// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665Qt::Actions::LoadMediaSetManagerAction.
 **/

#ifndef ARINC_665_QT_ACTIONS_LOADMEDIASETMANAGERACTION_HPP
#define ARINC_665_QT_ACTIONS_LOADMEDIASETMANAGERACTION_HPP

#include <arinc_665_qt/actions/Actions.hpp>

#include <arinc_665/utils/Utils.hpp>

#include <filesystem>
#include <memory>

#include <QObject>

namespace Arinc665Qt::Actions {

/**
 * @brief Asynchronous %Media Set Manager Loader Action.
 *
 * Loads the %Media Set Manger in a separate task.
 * For communication with a GUI, signals are provided.
 **/
class ARINC_665_QT_EXPORT LoadMediaSetManagerAction final : public QObject
{
    Q_OBJECT

  public:
    /**
     * @brief Initialises the Action.
     *
     * @param[in] parent
     *   Owning Object
     **/
    explicit LoadMediaSetManagerAction( QObject * parent = nullptr );

    //! Destructor
    ~LoadMediaSetManagerAction() override;

    /**
     * @brief Sets the media set directory.
     *
     * @param[in] mediaSetDirectory
     *   Media Set directory.
     **/
    void mediaSetDirectory( std::filesystem::path mediaSetDirectory );

    /**
     * @brief Set the *Check %Media Set Integrity* parameter.
     *
     * @param checkMediaSetIntegrity
     *   Check %Media Set Directory Parameter.
     **/
    void checkMediaSetIntegrity( bool checkMediaSetIntegrity );

  signals:
    /**
     * @brief Signal emitted when new progress information is available.
     *
     * @param[in] currentMediaSet
     *   Current %Media Set Counter
     * @param[in] numberOfMediaSets
     *   Number of %Media Sets.
     * @param[in] partNumber
     *   Part Number of current %Media Set.
     **/
    void mediaSetManagerLoadProgress( size_t currentMediaSet, size_t numberOfMediaSets, const QString &partNumber );

    /**
     * @brief Signal emitted when the %Media Set Manager is loaded successfully.
     *
     * @param[in] mediaSetManager
     *   %Media Set Manager instance.
     **/
    void mediaSetManagerLoaded( const Arinc665::Utils::MediaSetManagerPtr &mediaSetManager );

    /**
     * @brief Signal emitted when the %Media Set Manager could not be loaded.
     *
     * @param[in] reason
     *   Reason describing the error.
     **/
    void failed( const QString &reason );

  public slots:
    /**
     * @brief Start Loading slot.
     **/
    void start();

  private:
    /**
     * @brief Load progress indicator.
     *
     * Callback which is connected to the %Media Set Manager.
     * This callback emits the Qt signal.
     *
     * @param[in] mediaSet
     *   Media Set information
     * @param[in] partNumber
     *   Media Set Part Number
     * @param[in] medium
     *   Medium information
     **/
    void loadProgress(
      std::pair< std::size_t, std::size_t > mediaSet,
      std::string_view partNumber,
      std::pair< Arinc665::MediumNumber, Arinc665::MediumNumber > medium );

    //! %Media Set Directory
    std::filesystem::path mediaSetDirectoryV;
    //! Check %Media Set Integrity parameter.
    bool checkMediaSetIntegrityV{ true };
};

}

#endif
