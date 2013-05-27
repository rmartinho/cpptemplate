/*
cpptempl
=================
This is a template engine for C++.

Syntax
=================
Variables: {$variable_name}
Loops: {% for person in people %}Name: {$person.name}{% endfor %}
If: {% for person.name == "Bob" %}Full name: Robert{% endif %}

Copyright
==================
Author: Ryan Ginstrom
MIT License

Usage
=======================
	wstring text = L"{% if item %}{$item}{% endif %}\n"
		L"{% if thing %}{$thing}{% endif %}" ;
	cpptempl::data_map data ;
	data[L"item"] = cpptempl::make_data(L"aaa") ;
	data[L"thing"] = cpptempl::make_data(L"bbb") ;

	wstring result = cpptempl::parse(text, data) ;

Handy Functions
========================
make_data() : Feed it a string, data_map, or data_list to create a data entry.
Example:
	data_map person ;
	person[L"name"] = make_data(L"Bob") ;
	person[L"occupation"] = make_data(L"Plumber") ;
	data_map data ;
	data[L"person"] = make_data(person) ;
	wstring result = parse(templ_text, data) ;

*/
#pragma once

#ifdef _WIN32
#pragma warning( disable : 4996 ) // 'std::copy': Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'
#pragma warning( disable : 4512 ) // 'std::copy': Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'
#endif

#include <string>
#include <vector>
#include <map>							
#include <boost/shared_ptr.hpp>

#include <iostream>

namespace cpptempl
{
	using std::wstring ;
	// various typedefs

	// data classes
	class Data ;
	typedef boost::shared_ptr<Data> data_ptr ;
	typedef std::vector<data_ptr> data_list ;
	typedef std::map<wstring, data_ptr> data_map ;

	// token classes
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
		virtual wstring getvalue();
		virtual data_list& getlist();
		virtual data_map& getmap() ;
	};

	class DataValue : public Data
	{
		wstring m_value ;
	public:
		DataValue(wstring value) : m_value(value){}
		wstring getvalue();
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
	inline data_ptr make_data(wstring val)
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
	data_ptr parse_val(wstring key, data_map &data) ;

	typedef enum 
	{
		TOKEN_TYPE_NONE,
		TOKEN_TYPE_TEXT,
		TOKEN_TYPE_VAR,
		TOKEN_TYPE_IF,
		TOKEN_TYPE_FOR,
		TOKEN_TYPE_ENDIF,
		TOKEN_TYPE_ENDFOR,
	} TokenType;

	// Template tokens
	// base class for all token types
	class Token
	{
	public:
		virtual TokenType gettype() = 0 ;
		virtual void gettext(std::wostream &stream, data_map &data) = 0 ;
		virtual void set_children(token_vector &children);
		virtual token_vector & get_children();
	};

	// normal text
	class TokenText : public Token
	{
		wstring m_text ;
	public:
		TokenText(wstring text) : m_text(text){}
		TokenType gettype();
		void gettext(std::wostream &stream, data_map &data);
	};

	// variable
	class TokenVar : public Token
	{
		wstring m_key ;
	public:
		TokenVar(wstring key) : m_key(key){}
		TokenType gettype();
		void gettext(std::wostream &stream, data_map &data);
	};

	// for block
	class TokenFor : public Token 
	{
	public:
		wstring m_key ;
		wstring m_val ;
		token_vector m_children ;
		TokenFor(wstring expr);
		TokenType gettype();
		void gettext(std::wostream &stream, data_map &data);
		void set_children(token_vector &children);
		token_vector &get_children();
	};

	// if block
	class TokenIf : public Token
	{
	public:
		wstring m_expr ;
		token_vector m_children ;
		TokenIf(wstring expr) : m_expr(expr){}
		TokenType gettype();
		void gettext(std::wostream &stream, data_map &data);
		bool is_true(wstring expr, data_map &data);
		void set_children(token_vector &children);
		token_vector &get_children();
	};

	// end of block
	class TokenEnd : public Token // end of control block
	{
		wstring m_type ;
	public:
		TokenEnd(wstring text) : m_type(text){}
		TokenType gettype();
		void gettext(std::wostream &stream, data_map &data);
	};

	wstring gettext(token_ptr token, data_map &data) ;

	void parse_tree(token_vector &tokens, token_vector &tree, TokenType until=TOKEN_TYPE_NONE) ;
	token_vector & tokenize(wstring text, token_vector &tokens) ;

	// The big daddy. Pass in the template and data, 
	// and get out a completed doc.
	void parse(std::wostream &stream, wstring templ_text, data_map &data) ;
	wstring parse(wstring templ_text, data_map &data);
}
