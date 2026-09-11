// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Utils::MediaSetCompiler.
 **/

#ifndef ARINC_665_UTILS_MEDIASETCOMPILER_HPP
#define ARINC_665_UTILS_MEDIASETCOMPILER_HPP

#include <arinc_665/utils/Utils.hpp>

#include <arinc_665/media/Media.hpp>

#include <helper/RawData.hpp>

#include <filesystem>
#include <functional>

namespace Arinc665::Utils {

/**
 * @brief ARINC 665 %Media Set Compiler.
 *
 * This class is responsible for managing the creation of media, directories, and files, as well as handling the reading
 * and writing of specific file types within the target.
 **/
class ARINC_665_EXPORT MediaSetCompiler
{
  public:
    /**
     * @brief Handler, which is called to generate the given Medium.
     *
     * @param[in] mediumNumber
     *   Medium Number to Create.
     **/
    using CreateMediumHandler = std::function< void( const MediumNumber &mediumNumber ) >;

    /**
     * @brief Handler, which is called to generate the given Directory.
     *
     * @param[in] directory
     *   Directory to create.
     **/
    using CreateDirectoryHandler =
      std::function< void( const MediumNumber &mediumNumber, const Media::ConstDirectoryPtr &directory ) >;

    /**
     * @brief Handler, which checks the existence of a file within the Source.
     *
     * This callback is used to check if a file exists on sources.
     * This is used for Load Header and Batch Files when the file creation policy is set to
     * @p FileCreationPolicy::NoneExisting.
     *
     * @param[in] file
     *   File to check existence for.
     *
     * @return if the File exist on sources.
     **/
    using CheckFileExistenceHandler = std::function< bool( const Media::ConstFilePtr &file ) >;

    /**
     * @brief Handler, which is called to generate the given File from Source.
     *
     * How the file is generated is not known to the exporter itself.
     *
     * @param[in] file
     *   File to be created.
     **/
    using CreateFileHandler = std::function< void( const Media::ConstFilePtr &file ) >;

    /**
     * @brief Handler, which is called to write the given File on the Target.
     *
     * This handler is also used to write files, which are not represented by Arinc665::Media classes.
     * Therefore, a basic representation is used.
     *
     * @param[in] mediumNumber
     *   Medium number
     * @param[in] path
     *   Relative path on Medium.
     * @param[in] file
     *   File data (binary).
     **/
    using WriteFileHandler = std::function< void(
      const MediumNumber &mediumNumber,
      const std::filesystem::path &path,
      const Helper::ConstRawDataSpan &file ) >;

    /**
     * @brief Handler, which is called to read a File from the Target.
     *
     * This handler is also used to read files, which are not represented by Arinc665::Media classes.
     * Therefore, a basic representation is used.
     * This operation is used for checksum and check value calculation.
     *
     * @param[in] mediumNumber
     *   Medium Number
     * @param[in] path
     *   Relative Path on Medium.
     *
     * @return File Data (Read as binary).
     **/
    using ReadFileHandler =
      std::function< Helper::RawData( const MediumNumber &mediumNumber, const std::filesystem::path &path ) >;

    /**
     * @brief Creates the ARINC 665 %Media Set Compiler Instance.
     *
     * @return ARINC 665 %Media Set Compiler Instance
     **/
    [[nodiscard]] static MediaSetCompilerPtr create();

    //! Destructor
    virtual ~MediaSetCompiler() = default;

    /**
     * @name Configuration Methods.
     * @{
     **/

    /**
     * @brief Sets the %Media Set to compile.
     *
     * @param[in] mediaSet
     *   Media Set, which shall be compiled.
     *
     * @return @p *this for chaining.
     **/
    virtual MediaSetCompiler& mediaSet( Media::ConstMediaSetPtr mediaSet ) = 0;

    /**
     * @brief Sets the Create Medium Handler.
     *
     * @param[in] createMediumHandler
     *   Called to create the given medium.
     *
     * @return @p *this for chaining.
     **/
    virtual MediaSetCompiler& createMediumHandler( CreateMediumHandler createMediumHandler ) = 0;

    /**
     * @brief Sets the Create Directory Handler.
     *
     * @param[in] createDirectoryHandler
     *   Called to create the given directory.
     *
     * @return @p *this for chaining.
     **/
    virtual MediaSetCompiler& createDirectoryHandler( CreateDirectoryHandler createDirectoryHandler ) = 0;

    /**
     * @brief Sets the Check File Existence Handler.
     *
     * @param[in] checkFileExistenceHandler
     *   Called to check if a file exists.
     *
     * @return @p *this for chaining.
     **/
    virtual MediaSetCompiler& checkFileExistenceHandler( CheckFileExistenceHandler checkFileExistenceHandler ) = 0;

    /**
     * @brief Sets the Create File Handler.
     *
     * @param[in] createFileHandler
     *   When a file needs to be generated, this handler is called.
     *
     * @return @p *this for chaining.
     **/
    virtual MediaSetCompiler& createFileHandler( CreateFileHandler createFileHandler ) = 0;

    /**
     * @brief Sets the Write File Handler.
     *
     * @param[in] writeFileHandler
     *   Writes a given file to the output media set.
     *
     * @return *this for chaining.
     **/
    virtual MediaSetCompiler& writeFileHandler( WriteFileHandler writeFileHandler ) = 0;

    /**
     * @brief Sets the Read File Handler.
     *
     * @param[in] readFileHandler
     *   Reads a given file from the output media set.
     *   Used for CRC calculation.
     *
     * @return *this for chaining.
     **/
    virtual MediaSetCompiler& readFileHandler( ReadFileHandler readFileHandler ) = 0;

    /**
     * @brief Sets the ARINC 665 Version Flag.
     *
     * @param[in] version
     *   ARINC 665 version used for exporting.
     *
     * @return *this for chaining.
     **/
    virtual MediaSetCompiler& arinc665Version( SupportedArinc665Version version ) = 0;

    /**
     * @brief Sets the Create Batch Files Flag.
     *
     * @param[in] createBatchFiles
     *   Defines if Batch Files are created by exporter or pre-existing ones are used.
     *
     * @return *this for chaining.
     **/
    virtual MediaSetCompiler& createBatchFiles( FileCreationPolicy createBatchFiles ) = 0;

    /**
     * @brief Sets the Create Load Header Files Flag.
     *
     * @param[in] createLoadHeaderFiles
     *   Defines if Load Header Files are created by exporter or pre-existing ones are used.
     *
     * @return *this for chaining.
     **/
    virtual MediaSetCompiler& createLoadHeaderFiles( FileCreationPolicy createLoadHeaderFiles ) = 0;

    /** @} **/

    /**
     * @brief Executes the ARINC 665 %Media Set Compiler.
     *
     * All parameters must have been set previously.
     *
     * @throw Arinc665Exception
     *   When compilation fails
     **/
    virtual void operator()() = 0;
};

}

#endif
