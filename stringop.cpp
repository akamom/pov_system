#include "stringop.h"

std::string stringop::trimString(const std::string str)
{
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first)
    {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

void stringop::correctPath(std::string &path) {
    // replaces all occurences of \ with /, and removes multiple backslashes
    char lastChar = 'a';
    const char ch1 = '\\';
    const char ch2 = '/';
    for (int i = 0; i < path.length(); ++i) {
        if (lastChar == ch1 && path[i] == ch1){
            // remove double backslashes
            //path[i] = (char)0;
            path.erase(i, 1);
            continue;
        }
        lastChar = path[i];
        if (path[i] == ch1){
            // replace backslash with slash
            path[i] = ch2;
        }
    }
    if (path[path.length()-1] != '/')
        path.append("/");
}

std::vector<std::string> stringop::split(std::string s, std::string delimiter, bool ignore_inside_string){
	const auto string_space_replacement = '$';
    // automatically check if there is a need to check for strings
    if (s.find('"') == std::string::npos)
        ignore_inside_string = true;

	if (!ignore_inside_string)
	{
		// replace space inside string with replacement char
		auto is_inside_string = false;
		for(auto i = 0; i<s.length(); i++)
		{
			const auto c = s[i];
			if (c == '"')
			{
				is_inside_string = !is_inside_string;
				s.erase(i, 1);
			}
			if (c == ' ' && is_inside_string)
				s[i] = string_space_replacement;
		}
	}

	// append delimiter to iterate through all split string not all-1
	s.append(delimiter);
	std::vector<std::string> list;
	size_t pos;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		auto token = s.substr(0, pos);
		if (!token.empty())
		{
			if (!ignore_inside_string)
			{
				// replace replacement char with space again
				size_t space_replace_pos;
				while ((space_replace_pos = token.find(string_space_replacement)) != std::string::npos)
				{
					token[space_replace_pos] = ' ';
				}
			}
			list.push_back(token);
		}
		s.erase(0, pos + delimiter.length());
	}
	return list;
}