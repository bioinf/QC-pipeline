#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <exception>
#include <string>

class QcException : std::exception {
public:
	QcException(const std::string m) {
		this->message = m;
	};

	~QcException() throw (){};

	const char* what() const throw () {
		return message.c_str();
	};

private:
	std::string message;
};


#endif /* EXCEPTION_H_ */
