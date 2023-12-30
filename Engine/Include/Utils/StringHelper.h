#pragma once

#include <string>
#include <vector>

namespace Trinity
{
	class StringHelper
	{
	public:

		static void ltrim(std::string& s);
		static void rtrim(std::string& s);
		static void trim(std::string& s);

		static std::string toSnakeCase(const std::string& s);
		static std::vector<std::string> split(const std::string& s, char delim);
		static std::string join(const std::vector<std::string>& splits, char delim);
	};
}