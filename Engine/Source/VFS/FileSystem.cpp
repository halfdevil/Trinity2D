#include "VFS/FileSystem.h"
#include "Core/Debugger.h"
#include "Core/Logger.h"

namespace Trinity
{
	FileSystem::~FileSystem()
	{
	}

	std::string FileSystem::getFileName(const std::string& filePath, bool withExtension) const
	{
		fs::path fileName = fs::path(filePath).filename();
		if (!withExtension)
		{
			fileName.replace_extension();
		}

		return fileName.string();
	}

	std::string FileSystem::getDirectory(const std::string& filePath) const
	{
		fs::path dir(filePath);
		dir.remove_filename();

		return dir.string();
	}

	std::string FileSystem::getExtension(const std::string& filePath) const
	{
		return fs::path(filePath).extension().string();
	}

	std::string FileSystem::combinePath(const std::string& pathA, const std::string& pathB) const
	{
		fs::path fullPath(sanitizePath(pathA));
		fullPath /= sanitizePath(pathB);

		return fullPath.generic_string();
	}

	std::string FileSystem::relativePath(const std::string& path, const std::string& basePath) const
	{
		fs::path relPath = fs::relative(path, basePath);
		return relPath.string();
	}

	std::string FileSystem::sanitizePath(const std::string& path) const
	{
		std::string p = path;
		std::replace(p.begin(), p.end(), '\\', '/');

		return fs::path(p).generic_string();
	}

	std::string FileSystem::canonicalPath(const std::string& path) const
	{
		return fs::weakly_canonical(path).string();
	}

	std::string FileSystem::subtractPath(const std::string& pathA, const std::string& pathB)
	{
		return fs::path(pathA).lexically_relative(pathB).string();
	}

	bool FileSystem::isExist(const std::string& filePath) const
	{
		for (auto& it : mStorages)
		{
			std::string alias = it.first;
			std::string path = filePath;

			if (path.starts_with(alias))
			{
				return it.second->isExist(filePath);
			}
		}

		return false;
	}

	bool FileSystem::isDirectory(const std::string& filePath) const
	{
		for (auto& it : mStorages)
		{
			std::string alias = it.first;
			std::string path = filePath;

			if (path.starts_with(alias))
			{
				return it.second->isDirectory(filePath);
			}
		}

		return false;
	}

	bool FileSystem::hasExtension(const std::string& filePath, const std::string& extension) const
	{
		fs::path path(filePath);

		if (!extension.empty())
		{
			return path.extension() == extension;
		}

		return path.has_extension();
	}

	bool FileSystem::getFiles(const std::string& dir, bool recurse, std::vector<FileEntry>& files) const
	{
		for (auto& it : mStorages)
		{
			std::string alias = it.first;
			std::string path = dir;

			if (path.starts_with(alias))
			{
				return it.second->getFiles(dir, recurse, files);
			}
		}

		return false;
	}

	bool FileSystem::getFiles(const std::string& dir, bool recurse, const std::vector<std::string>& extensions, 
		std::vector<FileEntry>& files) const
	{
		for (auto& it : mStorages)
		{
			std::string alias = it.first;
			std::string path = dir;

			if (path.starts_with(alias))
			{
				return it.second->getFiles(dir, recurse, extensions, files);
			}
		}

		return false;
	}

	bool FileSystem::addFolder(const std::string& alias, const std::string& path)
	{
		auto folder = std::make_unique<Folder>();
		if (!folder->create(alias, path))
		{
			LogError("Folder::create() failed!!");
			return false;
		}

		mStorages.insert({ alias, std::move(folder) });
		return true;
	}

	bool FileSystem::createDirs(const std::string& dir) const
	{
		for (auto& it : mStorages)
		{
			std::string alias = it.first;
			std::string path = dir;

			if (path.starts_with(alias))
			{
				return it.second->createDir(dir);
			}
		}

		return false;
	}

	bool FileSystem::copyFile(const std::string& from, const std::string& to) const
	{
		for (auto& it : mStorages)
		{
			std::string alias = it.first;
			std::string path = from;

			if (path.starts_with(alias))
			{
				return it.second->copyFile(from, to);
			}
		}

		return false;
	}

	bool FileSystem::copyFiles(const std::string& from, const std::string& to) const
	{
		for (auto& it : mStorages)
		{
			std::string alias = it.first;
			std::string path = from;

			if (path.starts_with(alias))
			{
				return it.second->copyFiles(from, to);
			}
		}

		return false;
	}

	std::unique_ptr<File> FileSystem::openFile(const std::string& filePath, FileOpenMode openMode)
	{
		for (auto& it : mStorages)
		{
			std::string alias = it.first;
			std::string path = filePath;

			if (path.starts_with(alias))
			{
				return it.second->openFile(filePath, openMode);
			}
		}

		return nullptr;
	}

	std::string FileSystem::readText(const std::string& fileName)
	{
		auto file = openFile(fileName, FileOpenMode::OpenRead);
		if (!file)
		{
			LogError("Error opening file: %s", fileName.c_str());
			return {};
		}

		FileReader reader{ *file };
		return reader.readAsString();
	}

	bool FileSystem::writeText(const std::string& fileName, const std::string& text)
	{
		auto file = openFile(fileName, FileOpenMode::OpenWrite);
		if (!file)
		{
			LogError("Error opening file: %s", fileName.c_str());
			return false;
		}

		FileWriter writer(*file);
		if (!writer.writeAsString(text))
		{
			LogError("FileWriter::writeAsString() failed for file: '%s'", fileName.c_str());
			return false;
		}

		return true;
	}
}