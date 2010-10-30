#include "cpptempl.h"

namespace cpptempl
{
	//////////////////////////////////////////////////////////////////////////
	// Data class and subclasses
	//////////////////////////////////////////////////////////////////////////
	wstring Data::getvalue()
	{
		throw TemplateException("Data item is not a value") ;
	}

	data_list& Data::getlist()
	{
		throw TemplateException("Data item is not a list") ;
	}
	data_map& Data::getmap()
	{
		throw TemplateException("Data item is not a dictionary") ;
	}
	wstring DataValue::getvalue()
	{
		return m_value ;
	}

	bool DataValue::empty()
	{
		return m_value.empty();
	}
	data_list& DataList::getlist()
	{
		return m_items ;
	}

	bool DataList::empty()
	{
		return m_items.empty();
	}
	data_map& DataMap:: getmap()
	{
		return m_items ;
	}

	bool DataMap::empty()
	{
		return m_items.empty();
	}
	//////////////////////////////////////////////////////////////////////////
	// parse_val
	//////////////////////////////////////////////////////////////////////////
	data_ptr parse_val(wstring key, data_map &data)
	{
		// quoted string
		if (key[0] == L'\"')
		{
			return make_data(boost::trim_copy_if(key, boost::is_any_of(L"\""))) ;
		}
		// check for dotted notation, i.e [foo.bar]
		size_t index = key.find(L".") ;
		if (index == wstring::npos)
		{
			return data[key] ;
		}

		data_ptr item = data[key.substr(0, index)] ;
		return parse_val(key.substr(index+1), item->getmap()) ;
	}

	//////////////////////////////////////////////////////////////////////////
	// Token classes
	//////////////////////////////////////////////////////////////////////////
	wstring TokenText::gettype()
	{
		return L"text" ;
	}
	wstring TokenText::gettext( data_map & )
	{
		return m_text ;
	}

	wstring TokenVar::gettype()
	{
		return L"var" ;
	}
	wstring TokenVar::gettext( data_map &data )
	{
		return parse_val(m_key, data)->getvalue() ;
	}

	TokenFor::TokenFor(wstring expr)
	{
		std::vector<wstring> elements ;
		boost::split(elements, expr, boost::is_space()) ;
		if (elements.size() != 4u)
		{
			throw TemplateException("Invalid syntax in for statement") ;
		}
		m_val = elements[1] ;
		m_key = elements[3] ;
	}

	wstring TokenFor::gettype()
	{
		return L"for" ;
	}

	wstring TokenFor::gettext( data_map &data )
	{
		std::vector<wstring> elements ;
		data_ptr value = parse_val(m_key, data) ;
		data_list &items = value->getlist() ;
		for (size_t i = 0 ; i < items.size() ; ++i)
		{
			data[L"loop"] = make_data(boost::lexical_cast<wstring>(i+1)) ;
			data[L"loop0"] = make_data(boost::lexical_cast<wstring>(i)) ;
			data[m_val] = items[i] ;
			for(size_t j = 0 ; j < m_children.size() ; ++j)
			{
				elements.push_back(m_children[j]->gettext(data)) ;
			}
		}
		return boost::join(elements, L"") ;
	}

	wstring TokenIf::gettype()
	{
		return L"if" ;
	}

	wstring TokenIf::gettext( data_map &data )
	{
		std::vector<wstring> elements ;
		if (is_true(m_expr, data))
		{
			for(size_t j = 0 ; j < m_children.size() ; ++j)
			{
				elements.push_back(m_children[j]->gettext(data)) ;
			}
		}
		return boost::join(elements, L"") ;
	}
	bool TokenIf::is_true( wstring expr, data_map &data )
	{
		std::vector<wstring> elements ;
		boost::split(elements, expr, boost::is_space()) ;

		if (elements[1] == L"not")
		{
			return parse_val(elements[2], data)->empty() ;
		}
		if (elements.size() == 2)
		{
			return ! parse_val(elements[1], data)->empty() ;
		}
		data_ptr lhs = parse_val(elements[1], data) ;
		data_ptr rhs = parse_val(elements[3], data) ;
		if (elements[2] == L"==")
		{
			return lhs->getvalue() == rhs->getvalue() ;
		}
		return lhs->getvalue() != rhs->getvalue() ;
	}

	wstring TokenEnd::gettype()
	{
		return m_type ;
	}

	wstring TokenEnd::gettext( data_map &data )
	{
		return L"" ;
	}

	//////////////////////////////////////////////////////////////////////////
	// tokenize
	//////////////////////////////////////////////////////////////////////////
	token_vector & tokenize(wstring text, token_vector &tokens)
	{
		while(true)
		{
			size_t pos = text.find(L"{") ;
			if (pos == wstring::npos)
			{
				if (! text.empty())
				{
					tokens.push_back(token_ptr(new TokenText(text))) ;
				}
				return tokens ;
			}
			wstring pre_text = text.substr(0, pos) ;
			if (! pre_text.empty())
			{
				tokens.push_back(token_ptr(new TokenText(pre_text))) ;
			}
			text = text.substr(pos+1) ;
			if (text.empty())
			{
				tokens.push_back(token_ptr(new TokenText(L"{"))) ;
				return tokens ;
			}

			// variable
			if (text[0] == L'$')
			{
				pos = text.find(L"}") ;
				if (pos != wstring::npos)
				{
					tokens.push_back(token_ptr (new TokenVar(text.substr(1, pos-1)))) ;
					text = text.substr(pos+1) ;
				}
			}
			// control statement
			else if (text[0] == L'%')
			{
				pos = text.find(L"}") ;
				if (pos != wstring::npos)
				{
					wstring expression = boost::trim_copy(text.substr(1, pos-2)) ;
					text = text.substr(pos+1) ;
					if (boost::starts_with(expression, L"for"))
					{
						tokens.push_back(token_ptr (new TokenFor(expression))) ;
					}
					else if (boost::starts_with(expression, L"if"))
					{
						tokens.push_back(token_ptr (new TokenIf(expression))) ;
					}
					else
					{
						tokens.push_back(token_ptr (new TokenEnd(boost::trim_copy(expression)))) ;
					}
				}
			}
			else
			{
				tokens.push_back(token_ptr(new TokenText(L"{"))) ;
			}
		}
		return tokens ;
	}

}
