#pragma once

#include <string>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>

namespace cpptempl
{
	// various typedefs
	class Data ;
	typedef boost::shared_ptr<Data> data_ptr ;
	typedef std::vector<data_ptr> data_list ;
	typedef std::map<std::wstring, data_ptr> data_map ;

	class Token ;
	typedef boost::shared_ptr<Token> token_ptr ;
	typedef std::vector<token_ptr> token_vector ;

	// Custom exception class for library errors
	class TemplateException : public std::exception
	{
	public:
		TemplateException(std::string reason) : std::exception(reason.c_str()){}
	};

	// Data types used in templates
	class Data
	{
	public:
		virtual std::wstring getvalue();
		virtual data_list& getlist();
		virtual data_map& getmap() ;
	};

	class DataValue : public Data
	{
		std::wstring m_value ;
	public:
		DataValue(std::wstring value) : m_value(value){}
		std::wstring getvalue();
	};

	class DataList : public Data
	{
		data_list m_items ;
	public:
		DataList(data_list &items) : m_items(items){}
		data_list& getlist() ;
	};

	class DataMap : public Data
	{
		data_map m_items ;
	public:
		DataMap(data_map &items) : m_items(items){}
		data_map& getmap();
	};

	// get a data value from a data map
	// e.g. foo.bar => data["foo"]["bar"]
	data_ptr parse_val(std::wstring key, data_map &data) ;

	// Template tokens
	class Token
	{
	public:
		virtual std::wstring gettype() = 0 ;
		virtual std::wstring gettext(data_map &data) = 0 ;
	};
	class TokenValue : public Token
	{
		std::wstring m_key ;
	public:
		std::wstring gettype();
		std::wstring gettext(data_map &data);
	};

	token_vector & tokenize(std::wstring text, token_vector &tokens) ;
}
