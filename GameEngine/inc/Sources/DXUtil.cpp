#include "enginepch.h"
#include "DXUtil.h"

namespace GameEngine
{
	using namespace std;

	void Tokenize(vector<string>& tokens, string& line, string delimeter)
	{
		size_t pos;
		while((pos = line.find_first_of(delimeter)) != line.npos) {
			if(line[0] == '\"') {
				pos = line.find_first_of('\"', 1);
				if(pos > 1) {
					string t = line.substr(1, pos - 1).data();
					tokens.push_back(t);
				}
				line = line.substr(pos + 1);
				continue;
			}
			if(pos > 0) {
				string t = line.substr(0, pos).data();
				tokens.push_back(t);
			}
			line.swap(line.substr(pos + 1));
		}

		if(line.length() > 0) {
			if(line[0] == '\"') {
				pos = line.find_first_of('\"', 1);
				if(pos > 1) {
					string t = line.substr(0, pos).data();
					tokens.push_back(t);
				}
			}
			else {
				string t = line.substr(0, pos).data();
				tokens.push_back(t);
			}
		}
	}
}