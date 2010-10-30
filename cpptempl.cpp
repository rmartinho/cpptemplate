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
	data_list& DataList::getlist()
	{
		return m_items ;
	}

	data_map& DataMap:: getmap()
	{
		return m_items ;
	}

	//////////////////////////////////////////////////////////////////////////
	// parse_val
	//////////////////////////////////////////////////////////////////////////
	data_ptr parse_val(std::wstring key, data_map &data)
	{
		size_t index = key.find(L".") ;
		if (index == std::wstring::npos)
		{
			return data[key] ;
		}

		std::wstring subkey = key.substr(0, index) ;
		data_ptr item = data[subkey] ;
		return parse_val(key.substr(index+1), item->getmap()) ;
	}

	//////////////////////////////////////////////////////////////////////////
	// Token classes
	//////////////////////////////////////////////////////////////////////////
	std::wstring TokenValue::gettype()
	{
		return L"val" ;
	}

	std::wstring TokenValue::gettext( data_map &data )
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

}