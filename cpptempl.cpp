#include "cpptempl.h"

namespace cpptempl
{
	//////////////////////////////////////////////////////////////////////////
	// Data class and subclasses
	//////////////////////////////////////////////////////////////////////////
	std::wstring Data::getvalue()
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
	std::wstring DataValue::getvalue()
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
	data_ptr parse_val(std::wstring key, data_map &data)
	{
		// quoted string
		if (key[0] == L'\"')
		{
			return make_data(boost::trim_copy_if(key, boost::is_any_of(L"\""))) ;
		}
		// check for dotted notation, i.e [foo.bar]
		size_t index = key.find(L".") ;
		if (index == std::wstring::npos)
		{
			return data[key] ;
		}

		data_ptr item = data[key.substr(0, index)] ;
		return parse_val(key.substr(index+1), item->getmap()) ;
	}

	//////////////////////////////////////////////////////////////////////////
	// Token classes
	//////////////////////////////////////////////////////////////////////////
	std::wstring TokenText::gettype()
	{
		return L"text" ;
	}
	std::wstring TokenText::gettext( data_map & )
	{
		return m_text ;
	}

	std::wstring TokenVar::gettype()
	{
		return L"var" ;
	}
	std::wstring TokenVar::gettext( data_map &data )
	{
		return parse_val(m_key, data)->getvalue() ;
	}

	//////////////////////////////////////////////////////////////////////////
	// tokenize
	//////////////////////////////////////////////////////////////////////////
	token_vector & tokenize(std::wstring text, token_vector &tokens)
	{
		return tokens ;
	}

	TokenFor::TokenFor(std::wstring expr, token_vector &children) :
		m_children(children)
	{
		std::vector<std::wstring> elements ;
		boost::split(elements, expr, boost::is_space()) ;
		if (elements.size() != 4u)
		{
			throw TemplateException("Invalid syntax in for statement") ;
		}
		m_val = elements[1] ;
		m_key = elements[3] ;
	}

	std::wstring TokenFor::gettype()
	{
		return L"for" ;
	}

	std::wstring TokenFor::gettext( data_map &data )
	{
		std::vector<std::wstring> elements ;
		data_ptr value = parse_val(m_key, data) ;
		data_list &items = value->getlist() ;
		for (size_t i = 0 ; i < items.size() ; ++i)
		{
			data[L"loop"] = make_data(boost::lexical_cast<std::wstring>(i+1)) ;
			data[L"loop0"] = make_data(boost::lexical_cast<std::wstring>(i)) ;
			data[m_val] = items[i] ;
			for(size_t j = 0 ; j < m_children.size() ; ++j)
			{
				elements.push_back(m_children[j]->gettext(data)) ;
			}
		}
		return boost::join(elements, L"") ;
	}

	std::wstring TokenIf::gettype()
	{
		return L"if" ;
	}

	std::wstring TokenIf::gettext( data_map &data )
	{
		std::vector<std::wstring> elements ;
		if (is_true(m_expr, data))
		{
			for(size_t j = 0 ; j < m_children.size() ; ++j)
			{
				elements.push_back(m_children[j]->gettext(data)) ;
			}
		}
		return boost::join(elements, L"") ;
	}
	bool TokenIf::is_true( std::wstring expr, data_map &data )
	{
		std::vector<std::wstring> elements ;
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
}
