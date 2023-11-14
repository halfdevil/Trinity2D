#include "Utils/StringHelper.h"
#include <algorithm> 
#include <cctype>
#include <locale>
#include <sstream>

namespace Trinity
{
	void StringHelper::ltrim(std::string& s)
	{
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
			return !std::isspace(ch);
		}));
	}

	void StringHelper::rtrim(std::string& s)
	{
		s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
			return !std::isspace(ch);
		}).base(), s.end());
	}

	void StringHelper::trim(std::string& s)
	{
		rtrim(s);
		ltrim(s);
	}

	std::string StringHelper::toSnakeCase(const std::string& s)
	{
		std::stringstream result;

		for (const auto ch : s)
		{
			if (std::isalpha(ch))
			{
				if (std::isspace(ch))
				{
					result << "_";
				}
				else
				{
					if (std::isupper(ch))
					{
						result << "_";
					}

					result << (char)std::tolower(ch);
				}
			}
			else
			{
				result << ch;
			}
		}

		return result.str();
	}

	std::vector<std::string> StringHelper::split(const std::string& s, char delim)
	{
		std::vector<std::string> strings;
		std::istringstream f(s);

		std::string str;
		while (std::getline(f, str, delim)) 
		{
			strings.push_back(s);
		}

		return strings;
	}
}