#include "Core/Resource.h"

namespace Trinity
{
	void Resource::setName(const std::string& name)
	{
		mName = name;
	}

	void Resource::setFileName(const std::string& fileName)
	{
		mFileName = fileName;
	}
}