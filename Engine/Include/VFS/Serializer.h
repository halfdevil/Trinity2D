#pragma once

#include "nlohmann/json.hpp"
using json = nlohmann::json;

namespace Trinity
{
	class FileReader;
	class FileWriter;
	class ResourceCache;

	class Serializer
	{
	public:

		virtual bool read(FileReader& reader, ResourceCache& cache) = 0;
		virtual bool write(FileWriter& writer) = 0;

		virtual bool read(json& object, ResourceCache& cache) = 0;
		virtual bool write(json& object) = 0;
	};
}