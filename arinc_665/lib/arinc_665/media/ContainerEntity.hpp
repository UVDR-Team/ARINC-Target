// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Class Arinc665::Media::ContainerEntity.
 **/

#ifndef ARINC_665_MEDIA_CONTAINERENTITY_HPP
#define ARINC_665_MEDIA_CONTAINERENTITY_HPP

#include <arinc_665/media/Media.hpp>
#include <arinc_665/media/Base.hpp>

#include <arinc_665/MediumNumber.hpp>

#include <filesystem>
#include <string>
#include <string_view>

namespace Arinc665::Media {

/**
 * @brief %Media Set Container.
 *
 * Contains common operations needed for containers.
 * Direct children are:
 * - @ref MediaSet, and
 * - @ref Directory.
 **/
class ARINC_665_EXPORT ContainerEntity : public Base
{
  public:
    /**
     * @name Medium Number
     *
     * A %Media Set consists of 1 up to 255 %Media.
     * The default medium number when a child does not define any.
     * @{
     **/

    /**
     * @brief Effective default medium number.
     *
     * @return effective default medium number
     **/
    [[nodiscard]] MediumNumber effectiveDefaultMediumNumber() const;

    /**
     * @brief Returns the Default Medium Number, where the file is located on.
     *
     * @return Medium Number, where the file is located on.
     **/
    [[nodiscard]] OptionalMediumNumber defaultMediumNumber() const;

    /**
     * @brief Updates the default medium number
     *
     * @param[in] defaultMediumNumber
     *   Default medium number.
     *   If not provided, the default medium number of the parent container will be used.
     **/
    void defaultMediumNumber( OptionalMediumNumber defaultMediumNumber );

    /**
     * @brief Get the Number of the last Medium within the container.
     *
     * If the container is the %Media Set, this value is the number of media.
     *
     * @return Last medium within container.
     **/
    [[nodiscard]] MediumNumber lastMediumNumber() const;

    /** @} **/

    /**
     * @brief Indicates, if the container has child elements.
     *
     * @param[in] mediumNumber
     *   Medium number, to filter.
     *   If not provided, no filtering is performed.
     *
     * @return if there are subdirectories or files.
     **/
    [[nodiscard]] bool hasChildren( OptionalMediumNumber mediumNumber = {} ) const;

    /**
     * @name Subdirectories
     * @{
     **/

    /**
     * @brief Return the number of subdirectories.
     *
     * @param[in] mediumNumber
     *   Medium number, to filter.
     *   If not provided, no filtering is performed.
     *
     * @return Number of subdirectories.
     **/
    [[nodiscard]] size_t numberOfSubdirectories( OptionalMediumNumber mediumNumber = {} ) const;

    /**
     * @brief Returns all subdirectories within the current container.
     *
     * @param[in] mediumNumber
     *   Medium number, to filter.
     *   If not provided, no filtering is performed.
     *
     * @return All subdirectories contained in the current container.
     **/
    [[nodiscard]] ConstDirectories subdirectories( OptionalMediumNumber mediumNumber = {} ) const;

    //! @copydoc subdirectories(OptionalMediumNumber) const
    [[nodiscard]] Directories subdirectories( OptionalMediumNumber mediumNumber = {} );

    /**
     * @brief Returns the subdirectory with the given name within this container.
     *
     * @param[in] name
     *   Name of the requested subdirectory.
     *
     * @return Subdirectory with the given name.
     * @retval {}
     *   If no such subdirectory exists.
     **/
    [[nodiscard]] ConstDirectoryPtr subdirectory( std::string_view name ) const;

    //! @copydoc subdirectory(std::string_view) const
    [[nodiscard]] DirectoryPtr subdirectory( std::string_view name );

    /**
     * @brief Returns the subdirectory at given path
     *
     * If the path is absolute, the request is redirected to the media set.
     * The path `/` will return the media set.
     *
     * @param[in] path
     *   Path of directory.
     *
     * @return Directory at @p path
     * @retval {}
     *   If path does not reference a directory or path is empty.
     **/
    [[nodiscard]] ConstContainerEntityPtr subdirectory( const std::filesystem::path &path ) const;

    //! @copydoc subdirectory(const std::filesystem::path&) const
    [[nodiscard]] ContainerEntityPtr subdirectory( const std::filesystem::path &path );

    /**
     * @brief Adds a subdirectory with the given name.
     *
     * It is a failure to try to create an already existing directory.
     *
     * @param[in] name
     *   Name of the requested subdirectory.
     *
     * @return Created subdirectory.
     *
     * @throw Arinc665Exception()
     *   If directory or name with this name already exists.
     **/
    [[nodiscard]] DirectoryPtr addSubdirectory( std::string name );

    /**
     * @brief Removes the subdirectory with the given name within this entity.
     *
     * It is a failure to try to delete a non-existing directory.
     *
     * @param[in] name
     *   name of the requested subdirectory to be deleted.
     *
     * @throw Arinc665Exception()
     *   If directory does not exist.
     **/
    void removeSubdirectory( std::string_view name );

    /**
     * @brief Removes the given subdirectory within this entity.
     *
     * It is a failure to try to delete a non-existing directory.
     *
     * @param[in] subDirectory
     *   Subdirectory to be deleted.
     *
     * @throw Arinc665Exception()
     *   when the directory does not exist.
     **/
    void removeSubdirectory( const DirectoryPtr &subDirectory );

    /** @} **/

    /**
     * @name Files
     * All files, including regular files, loads, and batches.
     * @{
     **/

    /**
     * @brief Returns the number of files within this container.
     *
     * @param[in] mediumNumber
     *   Medium number, to filter.
     *   If not provided, no filtering is performed.
     *
     * @return Number of files within this container.
     **/
    size_t numberOfFiles( OptionalMediumNumber mediumNumber = {} ) const;

    /**
     * @brief Recursively returns number of files.
     *
     * @param[in] mediumNumber
     *   Medium number, to filter.
     *   If not provided, no filtering is performed.
     *
     * @return Number of files in container and its subdirectories.
     **/
    [[nodiscard]] size_t recursiveNumberOfFiles( OptionalMediumNumber mediumNumber = {} ) const;

    /**
     * @brief Returns all files present in the given container.
     *
     * @param[in] mediumNumber
     *   Medium number, to filter.
     *   If not provided, no filtering is performed.
     *
     * @return All files of the current container.
     **/
    [[nodiscard]] ConstFiles files( OptionalMediumNumber mediumNumber = {} ) const;

    //! @copydoc files(OptionalMediumNumber) const
    [[nodiscard]] Files files( OptionalMediumNumber mediumNumber = {} );

    /**
     * @brief Recursively returns all files.
     *
     * @return All files in @p container and its subdirectories.
     **/
    [[nodiscard]] ConstFiles recursiveFiles( OptionalMediumNumber mediumNumber = {} ) const;

    //! @copydoc recursiveFiles(OptionalMediumNumber) const
    [[nodiscard]] Files recursiveFiles( OptionalMediumNumber mediumNumber = {} );

    /**
     * @brief Returns the files with the given name.
     *
     * The lookup is started recursively within the current container.
     * The file type is not relevant (file can be load header file, batch file, or other file).
     *
     * @param[in] filename
     *   Name of the requested file.
     * @param[in] mediumNumber
     *   Medium number, to filter.
     *   If not provided, no filtering is performed.
     *
     * @return Files with the given name.
     * @retval {}
     *   If no such files exists.
     **/
    [[nodiscard]] ConstFiles recursiveFiles( std::string_view filename, OptionalMediumNumber mediumNumber = {} ) const;

    //! @copydoc recursiveFiles(std::string_view,OptionalMediumNumber) const
    [[nodiscard]] Files recursiveFiles( std::string_view filename, OptionalMediumNumber mediumNumber = {} );

    /**
     * @brief Returns the file with the given name within this container.
     *
     * The file type is not relevant (file can be load header file, batch file, or other file).
     *
     * @param[in] filename
     *   Name of the requested file.
     *
     * @return File with the given name.
     * @retval {}
     *   If no such file exists.
     **/
    [[nodiscard]] ConstFilePtr file( std::string_view filename ) const;

    //! @copydoc file(const std::string_view) const
    [[nodiscard]] FilePtr file( std::string_view filename );

    /**
     * @brief Returns the file at given path.
     *
     * If @p path is empty an empty file pointer is returned.
     * If @p path has a parent path (absolute or relative), the request is forwarded to this container.
     *
     * @param[in] path
     *   Path of file.
     *
     * @return File at @p path
     * @retval {}
     *   If path does not reference a directory or is empty.
     **/
    [[nodiscard]] ConstFilePtr file( const std::filesystem::path &path ) const;

    //! @copydoc file(const std::filesystem::path&) const
    [[nodiscard]] FilePtr file( const std::filesystem::path &path );

    /**
     * @brief Removes the file with the given name.
     *
     * A non-existing file is handled as failure.
     *
     * @param[in] filename
     *   Filename of the file to be deleted.
     *
     * @throw Arinc665Exception() if file does not exist.
     **/
    void removeFile( std::string_view filename );

    /**
     * @brief Removes the given file.
     *
     * A non-existing file (in this directory) is handled as failure.
     *
     * @param[in] file
     *   File to be deleted.
     *
     * @throw Arinc665Exception() if file does not exist.
     **/
    void removeFile( const ConstFilePtr &file );

    /** @} **/

    /**
     * @name Regular Files
     * @{
     **/

    /**
     * @brief Return the number of Regular %Files optionally filtered by medium number.
     *
     * @return Number of regular files.
     **/
    [[nodiscard]] size_t numberOfRegularFiles( OptionalMediumNumber mediumNumber = {} ) const;

    /**
     * @brief Recursively returns number of regular files optionally filtered by medium number.
     *
     * @return Number of regular files in container and its subdirectories.
     **/
    [[nodiscard]] size_t recursiveNumberOfRegularFiles( OptionalMediumNumber mediumNumber = {} ) const;

    /**
     * @brief Return Regular %Files optionally filtered by medium number.
     *
     * @param[in] mediumNumber
     *   Medium number, to filter.
     *   If not provided, no filtering is performed.
     *
     * @return Regular files contained within this container.
     **/
    [[nodiscard]] ConstRegularFiles regularFiles( OptionalMediumNumber mediumNumber = {} ) const;

    //! @copydoc regularFiles(OptionalMediumNumber) const
    [[nodiscard]] RegularFiles regularFiles( OptionalMediumNumber mediumNumber = {} );

    /**
     * @brief Recursively returns all regular files optionally filtered by medium number.
     *
     * @param[in] mediumNumber
     *   Medium number, to filter.
     *   If not provided, no filtering is performed.
     *
     * @return All regular files in @p container and its subdirectories.
     **/
    [[nodiscard]] ConstRegularFiles recursiveRegularFiles( OptionalMediumNumber mediumNumber = {} ) const;

    //! @copydoc recursiveRegularFiles(OptionalMediumNumber) const
    [[nodiscard]] RegularFiles recursiveRegularFiles( OptionalMediumNumber mediumNumber = {} );

    /**
     * @brief Returns the regular files with the given name optionally filtered by medium number.
     *
     * @param[in] filename
     *   Name of the requested regular file.
     * @param[in] mediumNumber
     *   Medium number, to filter.
     *   If not provided, no filtering is performed.
     *
     * @return The regular files with the given name.
     * @retval {}
     *   If no such files exists.
     **/
    [[nodiscard]] ConstRegularFiles recursiveRegularFiles(
      std::string_view filename,
      OptionalMediumNumber mediumNumber = {} ) const;

    //! @copydoc recursiveRegularFiles(std::string_view,OptionalMediumNumber) const
    [[nodiscard]] RegularFiles recursiveRegularFiles(
      std::string_view filename,
      OptionalMediumNumber mediumNumber = {} );

    /**
     * @brief Returns the Regular File with the given filename.
     *
     * @param[in] filename
     *   Load filename
     *
     * @return Load with the given filename.
     * @retval {}
     *   If load with given name does not exist.
     **/
    [[nodiscard]] ConstRegularFilePtr regularFile( std::string_view filename ) const;

    //! @copydoc regularFile(std::string_view) const
    [[nodiscard]] RegularFilePtr regularFile( std::string_view filename );

    /**
     * @brief Returns the regular file at given path.
     *
     * If @p path is empty, no file is returned.
     * If @p path has parent path, the request is forwarded to the container identified by parent path.
     *
     * @param[in] path
     *   Path of file.
     *
     * @return File at @p path
     * @retval {}
     *   If path does not reference a regular file or is empty.
     **/
    [[nodiscard]] ConstRegularFilePtr regularFile( const std::filesystem::path &path ) const;

    //! @copydoc regularFile(const std::filesystem::path&) const
    [[nodiscard]] RegularFilePtr regularFile( const std::filesystem::path &path );

    /**
     * @brief Adds a Regular %File into this container.
     *
     * @param[in] filename
     *   Filename of the file to be created.
     * @param[in] mediumNumber
     *   Medium number, where the file is located onto.
     *   If not provided the default medium number of the owning container is used.
     *
     * @return Created file.
     *
     * @throw Arinc665Exception
     *   When file already exist.
     *
     * @sa RegularFile
     **/
    [[nodiscard]] RegularFilePtr addRegularFile( std::string filename, OptionalMediumNumber mediumNumber = {} );

    /** @} **/

    /**
     * @name Loads
     * @{
     **/

    /**
     * @brief Return the number of Loads.
     *
     * @return Number of Loads.
     **/
    [[nodiscard]] size_t numberOfLoads( OptionalMediumNumber mediumNumber = {} ) const;

    /**
     * @brief Recursively returns number of Loads.
     *
     * @return Number of Loads in container and its subdirectories.
     **/
    [[nodiscard]] size_t recursiveNumberOfLoads( OptionalMediumNumber mediumNumber = {} ) const;

    /**
     * @brief Return loads within container.
     *
     * No subdirectories are entered.
     * The lookup can be restricted to a specific medium.
     *
     * @param[in] mediumNumber
     *   Medium Number to restrict the lookup
     *
     * @return Loads contained within this container.
     **/
    [[nodiscard]] ConstLoads loads( OptionalMediumNumber mediumNumber = {} ) const;

    //! @copydoc loads(OptionalMediumNumber) const
    [[nodiscard]] Loads loads( OptionalMediumNumber mediumNumber = {} );

    /**
     * @brief Recursively returns all Loads.
     *
     * The lookup can be restricted to a specific medium.
     *
     * @param[in] mediumNumber
     *   Medium Number to restrict the lookup
     *
     * @return All Loads in container and its subdirectories.
     **/
    [[nodiscard]] ConstLoads recursiveLoads( OptionalMediumNumber mediumNumber = {} ) const;

    //! @copydoc recursiveLoads(OptionalMediumNumber) const
    [[nodiscard]] Loads recursiveLoads( OptionalMediumNumber mediumNumber = {} );

    /**
     * @brief Returns the Loads with the given name.
     *
     * @param[in] filename
     *   Name of the requested Load.
     * @param[in] mediumNumber
     *   Medium number, to filter.
     *   If not provided, no filtering is performed.
     *
     * @return Loads with the given name.
     * @retval {}
     *   If no loads with given filename exists.
     **/
    [[nodiscard]] ConstLoads recursiveLoads( std::string_view filename, OptionalMediumNumber mediumNumber = {} ) const;

    //! @copydoc recursiveLoads(std::string_view,OptionalMediumNumber) const
    [[nodiscard]] Loads recursiveLoads( std::string_view filename, OptionalMediumNumber mediumNumber = {} );

    /**
     * @brief Returns the load with the given filename.
     *
     * @param[in] filename
     *   Load filename
     *
     * @return Load with the given filename.
     * @retval {}
     *   If load does not exist.
     **/
    [[nodiscard]] ConstLoadPtr load( std::string_view filename ) const;

    //! @copydoc load(std::string_view) const
    [[nodiscard]] LoadPtr load( std::string_view filename );

    /**
     * @brief Returns the load at given path.
     *
     * If @p path is empty, no load is returned.
     * If @p path has parent path, the request is forwarded to the container identified by parent path.
     *
     * @param[in] path
     *   Path of file.
     *
     * @return Load at @p path
     * @retval {}
     *   If @p path does not reference a load or is empty.
     **/
    [[nodiscard]] ConstLoadPtr load( const std::filesystem::path &path ) const;

    //! @copydoc file(const std::filesystem::path&) const
    [[nodiscard]] LoadPtr load( const std::filesystem::path &path );

    /**
     * @brief Creates a load with the given filename.
     *
     * @param[in] filename
     *   %Load filename.
     * @param[in] mediumNumber
     *   Medium number, where the file is located onto.
     *   If not provided the default medium number of the owning container is used.
     *
     * @return Created load.
     *
     * @throw Arinc665Exception
     *   When a load with given filename already exist.
     *
     * @sa Load
     **/
    [[nodiscard]] LoadPtr addLoad( std::string filename, OptionalMediumNumber mediumNumber = {} );

    /** @} **/

    /**
     * @name Batches
     * @{
     **/

    /**
     * @brief Return the number of Batches.
     *
     * @return Number of Batches.
     **/
    [[nodiscard]] size_t numberOfBatches( OptionalMediumNumber mediumNumber = {} ) const;

    /**
     * @brief Recursively returns number of Batches.
     *
     * @return Number of Batches in container and its subdirectories.
     **/
    [[nodiscard]] size_t recursiveNumberOfBatches( OptionalMediumNumber mediumNumber = {} ) const;

    /**
     * @brief Return batches.
     *
     * @return Batches contained within this container.
     **/
    [[nodiscard]] ConstBatches batches( OptionalMediumNumber mediumNumber = {} ) const;

    //! @copydoc batches(OptionalMediumNumber) const
    [[nodiscard]] Batches batches( OptionalMediumNumber mediumNumber = {} );

    /**
     * @brief Recursively returns all Batches.
     *
     * @param[in] mediumNumber
     *   Medium number, to filter.
     *   If not provided, no filtering is performed.
     *
     * @return All Batches in container and its subdirectories.
     **/
    [[nodiscard]] ConstBatches recursiveBatches( OptionalMediumNumber mediumNumber = {} ) const;

    //! @copydoc recursiveBatches(OptionalMediumNumber) const
    [[nodiscard]] Batches recursiveBatches( OptionalMediumNumber mediumNumber = {} );

    /**
     * @brief Returns the Batches with the given name.
     *
     * @param[in] filename
     *   Name of the requested Batch.
     * @param[in] mediumNumber
     *   Medium number, to filter.
     *   If not provided, no filtering is performed.
     *
     * @return Batches with the given name.
     * @retval {}
     *   If no such batches exists.
     **/
    [[nodiscard]] ConstBatches recursiveBatches(
      std::string_view filename,
      OptionalMediumNumber mediumNumber = {} ) const;

    //! @copydoc recursiveBatches(std::string_view,OptionalMediumNumber) const
    [[nodiscard]] Batches recursiveBatches( std::string_view filename, OptionalMediumNumber mediumNumber = {} );

    /**
     * @brief Returns the batch with the given filename.
     *
     * @param[in] filename
     *   %Batch filename
     *
     * @return %Batch with the given filename.
     * @retval {}
     *   If batch does not exist.
     **/
    [[nodiscard]] ConstBatchPtr batch( std::string_view filename ) const;

    //! @copydoc batch(std::string_view) const
    [[nodiscard]] BatchPtr batch( std::string_view filename );

    /**
     * @brief Returns the batch at given path.
     *
     * If @p path is empty, no batch is returned.
     * If @p path has parent path, the request is forwarded to the container identified by parent path.
     *
     * @param[in] path
     *   Path of file.
     *
     * @return %Batch at @p path
     * @retval {}
     *   If path does not reference a batch or is empty.
     **/
    [[nodiscard]] ConstBatchPtr batch( const std::filesystem::path &path ) const;

    //! @copydoc file(const std::filesystem::path&) const
    [[nodiscard]] BatchPtr batch( const std::filesystem::path &path );

    /**
     * @brief Creates a batch with the given filename.
     *
     * @param[in] filename
     *   %Batch filename.
     * @param[in] mediumNumber
     *   Medium number, where the file is located onto.
     *   If not provided, the default medium number of the owning container is used.
     *
     * @return Created batch.
     *
     * @throw Arinc665Exception
     *   When a file with given filename already exist.
     *
     * @sa Batch
     **/
    [[nodiscard]] BatchPtr addBatch( std::string filename, OptionalMediumNumber mediumNumber = {} );

    /** @} **/

  protected:
    /**
     * @brief Constructs Container
     *
     * @param[in] defaultMediumNumber
     *   Default Medium Number
     **/
    explicit ContainerEntity( OptionalMediumNumber defaultMediumNumber = {} );

    /**
     * @brief Return the number of files (real file, load, batch) with the specified file type.
     *
     * @param[in] fileType
     *   %File type to search for.
     * @param[in] mediumNumber
     *   Medium number, where the file is located onto.
     *   If not provided, the default medium number of the owning container is used.
     *
     * @return Number of files of specific type.
     **/
    [[nodiscard]] size_t numberOfFiles( FileType fileType, OptionalMediumNumber mediumNumber ) const;

    /**
     * @brief Return the files (real file, load, batch) with the specified file type.
     *
     * @tparam FilesT
     *   Files List Type
     * @tparam fileType
     *   File type to search for.
     *
     * @return Files (real file, load, batch) with the specified file.
     **/
    template< typename FilesT, FileType fileType >
    [[nodiscard]] FilesT filesPerType( OptionalMediumNumber mediumNumber ) const;

    //! @copydoc filesPerType(OptionalMediumNumber) const
    template< typename FilesT, FileType fileType >
    [[nodiscard]] FilesT filesPerType( OptionalMediumNumber mediumNumber );

    /**
     * @brief Return the file (real file, load, batch) with the specified file type.
     *
     * @tparam FilesT
     *   Files List Type
     * @tparam fileType
     *   File type to search for.
     *
     * @param[in] filename
     *   Filename
     *
     * @return File (real file, load, batch) with the given filename.
     **/
    template< typename FilesT, FileType fileType >
    [[nodiscard]] FilesT filePerType( std::string_view filename ) const;

    //! @copydoc filePerType(std::string_view) const
    template< typename FilesT, FileType fileType >
    [[nodiscard]] FilesT filePerType( std::string_view filename );

    /**
     * @brief Return the file (real file, load, batch) with the specified file type at the given path.
     *
     * If @p path is empty, no file is returned.
     * If @p path has parent path, the request is forwarded to the container identified by parent path.
     *
     * @param[in] path
     *   Path of file.
     *
     * @return File at @p path
     * @retval {}
     *   If path does not reference a directory
     **/
    template< typename FilesT, FileType fileType >
    [[nodiscard]] FilesT filePerType( const std::filesystem::path &path ) const;

    //! @copydoc filePerType(const std::filesystem::path&) const
    template< typename FilesT, FileType fileType >
    [[nodiscard]] FilesT filePerType( const std::filesystem::path &path );

  private:
    //! Default Medium Number
    OptionalMediumNumber defaultMediumNumberV;
    //! Subdirectories
    Directories subdirectoriesV;
    //! Files
    Files filesV;
};

}

#endif
