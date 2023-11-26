#pragma once

#include "VFS/Folder.h"
#include "VFS/FileReader.h"
#include "VFS/FileWriter.h"
#include "Core/Singleton.h"
#include <unordered_map>
#include <string>

#include <filesystem>
namespace fs = std::filesystem;

namespace Trinity
{
	class FileSystem : public Singleton<FileSystem>
	{
	public:

		FileSystem() = default;
		~FileSystem();

		FileSystem(const FileSystem&) = delete;
		FileSystem& operator = (const FileSystem&) = delete;

		FileSystem(FileSystem&&) = delete;
		FileSystem& operator = (FileSystem&&) = delete;

		virtual std::unique_ptr<File> openFile(const std::string& filePath,
			FileOpenMode openMode);

		virtual std::string getFileName(const std::string& filePath, bool withExtension = true) const;
		virtual std::string getDirectory(const std::string& filePath) const;
		virtual std::string getExtension(const std::string& filePath) const;

		virtual std::string combinePath(const std::string& pathA, const std::string& pathB) const;
		virtual std::string relativePath(const std::string& path, const std::string& basePath) const;
		virtual std::string sanitizePath(const std::string& path) const;
		virtual std::string canonicalPath(const std::string& path) const;
		virtual std::string subtractPath(const std::string& pathA, const std::string& pathB);

		virtual bool isExist(const std::string& filePath) const;
		virtual bool isDirectory(const std::string& filePath) const;
		virtual bool hasExtension(const std::string& filePath, const std::string& extension = "") const;

		virtual bool getFiles(const std::string& dir, bool recurse, std::vector<FileEntry>& files) const;
		virtual bool getFiles(const std::string& dir, bool recurse, const std::vector<std::string>& extensions, 
			std::vector<FileEntry>& files) const;

		virtual bool addFolder(const std::string& alias, const std::string& path);
		virtual bool createDirs(const std::string& dir) const;
		virtual bool copyFile(const std::string& from, const std::string& to) const;
		virtual bool copyFiles(const std::string& from, const std::string& to) const;

		virtual std::string readText(const std::string& fileName);
		virtual bool writeText(const std::string& fileName, const std::string& text);

	protected:

		std::unordered_map<std::string, std::unique_ptr<Storage>> mStorages;
	};
}