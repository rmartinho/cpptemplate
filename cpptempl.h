#pragma once

#include <string>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

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
		virtual bool empty() = 0 ;
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
		bool empty();
	};

	class DataList : public Data
	{
		data_list m_items ;
	public:
		DataList(data_list &items) : m_items(items){}
		data_list& getlist() ;
		bool empty();
	};

	class DataMap : public Data
	{
		data_map m_items ;
	public:
		DataMap(data_map &items) : m_items(items){}
		data_map& getmap();
		bool empty();
	};

	// convenience functions for making data objects
	inline data_ptr make_data(std::wstring val)
	{
		return data_ptr(new DataValue(val)) ;
	}
	inline data_ptr make_data(data_list &val)
	{
		return data_ptr(new DataList(val)) ;
	}
	inline data_ptr make_data(data_map &val)
	{
		return data_ptr(new DataMap(val)) ;
	}
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
	class TokenText : public Token
	{
		std::wstring m_text ;
	public:
		TokenText(std::wstring text) : m_text(text){}
		std::wstring gettype();
		std::wstring gettext(data_map &data);
	};
	class TokenVar : public Token
	{
		std::wstring m_key ;
	public:
		TokenVar(std::wstring key) : m_key(key){}
		std::wstring gettype();
		std::wstring gettext(data_map &data);
	};
	class TokenFor : public Token
	{
		std::wstring m_key ;
		std::wstring m_val ;
		token_vector m_children ;
	public:
		TokenFor(std::wstring expr, token_vector &children);
		std::wstring gettype();
		std::wstring gettext(data_map &data);
	};
	class TokenIf : public Token
	{
		std::wstring m_expr ;
		token_vector m_children ;
	public:
		TokenIf(std::wstring expr, token_vector &children) : m_expr(expr), m_children(children){}
		std::wstring gettype();
		std::wstring gettext(data_map &data);
		bool is_true(std::wstring expr, data_map &data);
	};

	token_vector & tokenize(std::wstring text, token_vector &tokens) ;
}
