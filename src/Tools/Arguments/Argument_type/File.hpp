#ifndef ARGUMENT_TYPE_FILE_HPP_
#define ARGUMENT_TYPE_FILE_HPP_

#include <string>
#include <stdexcept>
#include <fstream>

#include "Argument_type_limited.hpp"

namespace aff3ct
{
namespace tools
{

enum openmode {read, write, read_write};


template <typename T = std::string, typename... Ranges>
class File_type : public Argument_type_limited<T,Ranges...>
{
private:
	const openmode mode;

public:
	template <typename r, typename... R>
	File_type(openmode mode, const r* range, const R*... ranges)
	: Argument_type_limited<T,Ranges...>(generate_title(mode), range, ranges...), mode(mode)
	{ }

	File_type(openmode mode)
	: Argument_type_limited<T,Ranges...>(generate_title(mode)), mode(mode)
	{ }

	virtual ~File_type() {};

	virtual File_type<T,Ranges...>* clone() const
	{
		auto clone = new File_type<T,Ranges...>(mode);

		return dynamic_cast<File_type<T,Ranges...>*>(this->clone_ranges(clone));
	}

	template <typename... NewRanges>
	File_type<T, Ranges..., NewRanges...>*
	clone(NewRanges*... new_ranges)
	{
		auto clone = new File_type<T, Ranges..., NewRanges...>(mode);

		this->clone_ranges(clone);

		clone->template add_ranges(new_ranges...);

		return clone;
	}

	virtual T convert(const std::string& val) const
	{
		return val;
	}

	virtual void check(const std::string& val) const
	{
		auto str_val = this->convert(val);

		if (str_val.empty())
			throw std::runtime_error("shall be a file name");

		switch(mode)
		{
			case openmode::read :
			{
				std::ifstream f(str_val);
				if(f.fail())
					throw std::runtime_error("does not name an existing file");

				break;
			}
			case openmode::write :
			case openmode::read_write : // nothing to check
				break;
		}

		this->check_ranges(str_val);
	}

	static std::string generate_title(openmode mode)
	{
		std::string str = "file [";

		switch(mode)
		{
			case openmode::read :
				str += "read only";
				break;

			case openmode::write :
				str += "write only";
				break;

			case openmode::read_write : // nothing to check
				str += "read/write";
				break;
		}

		str += "]";

		return str;
	}
};

template <typename T = std::string, typename... Ranges>
File_type<T,Ranges...>* File(openmode mode, Ranges*... ranges)
{
	return new File_type<T,Ranges...>(mode, ranges...);
}

}
}
#endif /* ARGUMENT_TYPE_FILE_HPP_ */