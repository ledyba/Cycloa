/*
 * EmulatorException.h
 *
 *  Created on: 2011/08/23
 *      Author: psi
 */

#ifndef EMULATOREXCEPTION_H_
#define EMULATOREXCEPTION_H_

#include <iostream>
#include <string>
#include <sstream>

class EmulatorException
{
	public:
		EmulatorException();
		EmulatorException(const char* fmsg);
		EmulatorException(const std::string& fmsg);
		EmulatorException(const EmulatorException& src);
		~EmulatorException();
	private:
		std::stringstream msg;
	public:
		const std::string getMessage() const;
		template<typename T> EmulatorException& operator<<(T& val)
		{
			this->msg << val;
			return *this;
		}
};

#endif /* EMULATOREXCEPTION_H_ */
