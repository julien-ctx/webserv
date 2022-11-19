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
	a hihi;
	return 0;
}
