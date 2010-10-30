#pragma once

#include <string>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace cpptempl
{
	using std::wstring ;
	// various typedefs
	class Data ;
	typedef boost::shared_ptr<Data> data_ptr ;
	typedef std::vector<data_ptr> data_list ;
	typedef std::map<wstring, data_ptr> data_map ;

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
	class Token
	{
	public:
		virtual TokenType gettype() = 0 ;
		virtual wstring gettext(data_map &data) = 0 ;
		virtual void set_children(token_vector &children);
		virtual token_vector & get_children();
	};
	class TokenText : public Token
	{
		wstring m_text ;
	public:
		TokenText(wstring text) : m_text(text){}
		TokenType gettype();
		wstring gettext(data_map &data);
	};
	class TokenVar : public Token
	{
		wstring m_key ;
	public:
		TokenVar(wstring key) : m_key(key){}
		TokenType gettype();
		wstring gettext(data_map &data);
	};
	class TokenFor : public Token
	{
		wstring m_key ;
		wstring m_val ;
		token_vector m_children ;
	public:
		TokenFor(wstring expr);
		TokenType gettype();
		wstring gettext(data_map &data);
		void set_children(token_vector &children);
		token_vector &get_children();
	};
	class TokenIf : public Token
	{
		wstring m_expr ;
		token_vector m_children ;
	public:
		TokenIf(wstring expr) : m_expr(expr){}
		TokenType gettype();
		wstring gettext(data_map &data);
		bool is_true(wstring expr, data_map &data);
		void set_children(token_vector &children);
		token_vector &get_children();
	};
	class TokenEnd : public Token // end of control block
	{
		wstring m_type ;
	public:
		TokenEnd(wstring text) : m_type(text){}
		TokenType gettype();
		wstring gettext(data_map &data);
	};

	void parse_tree(token_vector &tokens, token_vector &tree, TokenType until=TOKEN_TYPE_NONE) ;
	token_vector & tokenize(wstring text, token_vector &tokens) ;

	// The big daddy. Pass in the template and data, 
	// and get out a completed doc.
	wstring parse(wstring templ_text, data_map &data);
}
