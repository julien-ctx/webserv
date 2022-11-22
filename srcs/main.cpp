# include <string>
# include <iostream>
# include <cstring>
class b
{
private:
	std::string s;
public:
	b() : s("bruh"){}
	~b()
	{
		std::cout << "issou" << std::endl;
	}
};

class a
{
	private:
		b b;
	public:
		a() : b() {}
		~a()
		{
			std::cout << "Daft punk" << std::endl;
		}
};

int main(int ac, char **av)
{
	(void)ac;
	(void)av;
	// a hihi;
	// unsigned int k= 0x304E;
	// char i = k;
	std::cout << "🤓" << " hell"<< std::endl;
	return 0;
}


// void	Document::_parseEscapedUnicode(string_type::iterator& it, string_type& raw_str, size_type lineNum) const
// {
// 	if (!isxdigit(*(it + 2)) || !isxdigit(*(it + 3)) || !isxdigit(*(it + 4)) || !isxdigit(*(it + 5)))
// 		throw (parse_error("Illegal unicode escape sequence", lineNum));

// 	const long code = std::strtol(string_type(it + 2, it + 6).c_str(), NULL, 16);
// 	assert(code >= 0);
// 	if (code <= 0x7F) //0 to 127
// 	{
// 		raw_str.replace(it, it + 6, 1, static_cast<char>(code));
// 	}
// 	else if (code >= 0x0080 && code <= 0x07FF) // Two point unicode 128 to 2048
// 	{
// 		unsigned char unicode[3] = { 0b11000000, 0b10000000, 0x00 };
// 		unicode[1] |= (code)		& 0b111111;
// 		unicode[0] |= (code >> 6)	& 0b011111;
// 		raw_str.replace(it, it + 6, reinterpret_cast<const char *>(unicode));
// 	}
// 	else // Three point unicode  //2048 to FFFF
// 	{
// 		unsigned char unicode[4] = { 0b11100000, 0b10000000, 0b10000000, 0x00 };
// 		unicode[2] |= (code)		& 0b111111;
// 		unicode[1] |= (code >> 6)	& 0b111111;
// 		unicode[0] |= (code >> 12)	& 0b001111;
// 		raw_str.replace(it, it + 6, reinterpret_cast<const char *>(unicode));
// 	}
// }