#pragma once

#include <exception>
#include <string>

namespace Inc
{

class IncError : public std::exception {
public:
	explicit IncError(const std::string &msg) : msg_(msg) {}
	const char *what() const throw() override { return msg_.c_str(); }

private:
	std::string msg_;
};

}
