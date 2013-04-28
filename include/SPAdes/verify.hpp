//***************************************************************************
//* Copyright (c) 2011-2013 Saint-Petersburg Academic University
//* All Rights Reserved
//* See file LICENSE for details.
//****************************************************************************

#pragma once
#include "boost/current_function.hpp"
#include <sstream>
#include <iostream>
#include "stacktrace.hpp"

struct assertion_failed_exception : public std::exception {
public:
	assertion_failed_exception() {
		this->message = "";
	}

	assertion_failed_exception(const std::string m) {
		this->message = m;
	}
	~assertion_failed_exception() throw (){};

	const char* what() const throw () {
		return message.c_str();
	}

private:
	std::string message;
};

#define VERIFY(expr) 																							\
if (!(expr)) {																									\
	std::stringstream ss;																						\
	ss << "Verification of expression '" << #expr << "' failed in function '" <<  BOOST_CURRENT_FUNCTION << 	\
	"'. In file '" << __FILE__ << " on line " << __LINE__ << "'." << std::endl;									\
	ss << print_stacktrace();																						    \
	throw((assertion_failed_exception(ss.str())));																		\
}

#define VERIFY_MSG(expr, msg) 																					\
if (!(expr)) {																									\
	std::stringstream ss;																						\
	ss << "Verification of expression '" << #expr << "' failed in function '" <<  BOOST_CURRENT_FUNCTION << 	\
	"'. In file '" << __FILE__ << "' on line " << __LINE__ << ". Message '" << msg << "'." << std::endl;		\
	ss << print_stacktrace();																						    \
	throw ((assertion_failed_exception(ss.str())));																		\
}
