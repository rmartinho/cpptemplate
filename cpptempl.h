#pragma once

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace cpptempl
{
	typedef std::vector<std::wstring> token_vector ;

	class TemplateException : public std::exception
	{
	public:
		TemplateException(std::string reason) : std::exception(reason.c_str()){}
	};

	class Data
	{
	public:
		virtual std::wstring gettext()
		{
			throw TemplateException("Not implemented") ;
		}
		virtual void getlist(std::vector<std::wstring> &items)
		{
			throw TemplateException("Not implemented") ;
		}
		virtual boost::shared_ptr<Data> getitem()
		{
			throw TemplateException("Not implemented") ;
		}

	};

	class Token
	{
	public:
		virtual std::wstring gettype() = 0 ;
		virtual std::wstring gettext() = 0 ;

	};

	token_vector & tokenize(std::wstring text, token_vector &tokens) ;
}
