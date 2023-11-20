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

		std::unique_ptr<File> openFile(const std::string& filePath,
			FileOpenMode openMode);

		std::string getFileName(const std::string& filePath) const;
		std::string getDirectory(const std::string& filePath) const;
		std::string combinePath(const std::string& pathA, const std::string& pathB) const;
		std::string relativePath(const std::string& path, const std::string& basePath) const;
		std::string sanitizePath(const std::string& path) const;
		std::string canonicalPath(const std::string& path) const;
		std::string subtractPath(const std::string& pathA, const std::string& pathB);

		bool isExist(const std::string& filePath) const;
		bool isDirectory(const std::string& filePath) const;
		bool hasExtension(const std::string& filePath, const std::string& extension = "") const;

		bool getFiles(const std::string& dir, bool recurse, std::vector<FileEntry>& files) const;
		bool addFolder(const std::string& alias, const std::string& path);
		bool createDirs(const std::string& dir) const;
		bool copyFile(const std::string& from, const std::string& to) const;
		bool copyFiles(const std::string& from, const std::string& to) const;

		std::string readText(const std::string& fileName);
		bool writeText(const std::string& fileName, const std::string& text);

	protected:

		std::unordered_map<std::string, std::unique_ptr<Storage>> mStorages;
	};
}