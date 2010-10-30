#include "cpptempl.h"

#include <boost/test/unit_test.hpp>

#ifdef _WIN32
#pragma warning( disable : 4996 ) // doesn't like wcstombs
#endif
// Allow streaming of wstring's to std::ostream
namespace std {
	inline std::ostream& operator<<(std::ostream& out, const std::wstring& value)
	{
		size_t num_chars = wcstombs( NULL, value.c_str(), 0 );
		char *buf = new char[ num_chars ];
		wcstombs( buf, value.c_str(), num_chars );
		out << buf ;
		delete[] buf;
		return out;
	}
}
#ifdef _WIN32
#pragma warning( default : 4996 )
#endif

BOOST_AUTO_TEST_SUITE( TestData )

	using namespace cpptempl ;
	
	// DataMap
	BOOST_AUTO_TEST_CASE(test_DataMap_getvalue)
	{
		data_map items ;
		data_ptr data(new DataMap(items)) ;

		BOOST_CHECK_THROW( data->getvalue(), TemplateException ) ;
	}
	BOOST_AUTO_TEST_CASE(test_DataMap_getlist_throws)
	{
		data_map items ;
		data_ptr data(new DataMap(items)) ;

		BOOST_CHECK_THROW( data->getlist(), TemplateException ) ;
	}
	BOOST_AUTO_TEST_CASE(test_DataMap_getitem_throws)
	{
		data_map items ;
		items[L"key"] = data_ptr(new DataValue(L"foo")) ;
		data_ptr data(new DataMap(items)) ;

		BOOST_CHECK_EQUAL( data->getmap()[L"key"]->getvalue(), L"foo" ) ;
	}
	// DataList
	BOOST_AUTO_TEST_CASE(test_DataList_getvalue)
	{
		data_list items ;
		data_ptr data(new DataList(items)) ;

		BOOST_CHECK_THROW( data->getvalue(), TemplateException ) ;
	}
	BOOST_AUTO_TEST_CASE(test_DataList_getlist_throws)
	{
		data_list items ;
		items.push_back(data_ptr(new DataValue(L"bar"))) ;
		data_ptr data(new DataList(items)) ;

		BOOST_CHECK_EQUAL( data->getlist().size(), 1u ) ;
	}
	BOOST_AUTO_TEST_CASE(test_DataList_getitem_throws)
	{
		data_list items ;
		data_ptr data(new DataList(items)) ;

		BOOST_CHECK_THROW( data->getmap(), TemplateException ) ;
	}
	// DataValue
	BOOST_AUTO_TEST_CASE(test_DataValue_getvalue)
	{
		data_ptr data(new DataValue(L"foo")) ;

		BOOST_CHECK_EQUAL( data->getvalue(), L"foo" ) ;
	}
	BOOST_AUTO_TEST_CASE(test_DataValue_getlist_throws)
	{
		data_ptr data(new DataValue(L"foo")) ;

		BOOST_CHECK_THROW( data->getlist(), TemplateException ) ;
	}
	BOOST_AUTO_TEST_CASE(test_DataValue_getitem_throws)
	{
		data_ptr data(new DataValue(L"foo")) ;

		BOOST_CHECK_THROW( data->getmap(), TemplateException ) ;
	}
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE( test_parse_val )

	using namespace cpptempl ;
	BOOST_AUTO_TEST_CASE(test_quoted)
	{
		data_map data ;
		data[L"foo"] = data_ptr(new DataValue(L"bar")) ;
		data_ptr value = parse_val(L"\"foo\"", data) ;

		BOOST_CHECK_EQUAL( value->getvalue(), L"foo" ) ;
	}
	BOOST_AUTO_TEST_CASE(test_value)
	{
		data_map data ;
		data[L"foo"] = data_ptr(new DataValue(L"bar")) ;
		data_ptr value = parse_val(L"foo", data) ;

		BOOST_CHECK_EQUAL( value->getvalue(), L"bar" ) ;
	}
	BOOST_AUTO_TEST_CASE(test_list)
	{
		data_map data ;
		data_list items ;
		items.push_back(data_ptr(new DataValue(L"bar"))) ;
		data[L"foo"] = data_ptr(new DataList(items)) ;
		data_ptr value = parse_val(L"foo", data) ;

		BOOST_CHECK_EQUAL( value->getlist().size(), 1u ) ;
	}
	BOOST_AUTO_TEST_CASE(test_dotted)
	{
		data_map data ;
		data_map subdata ;
		subdata[L"b"] = data_ptr(new DataValue(L"c")) ;
		data[L"a"] = data_ptr(new DataMap(subdata)) ;
		data_ptr value = parse_val(L"a.b", data) ;

		BOOST_CHECK_EQUAL( value->getvalue(), L"c" ) ;
	}
	BOOST_AUTO_TEST_CASE(test_double_dotted)
	{
		data_map data ;
		data_map sub_data ;
		data_map sub_sub_data ;
		sub_sub_data[L"c"] = data_ptr(new DataValue(L"d")) ;
		sub_data[L"b"] = data_ptr(new DataMap(sub_sub_data)) ;
		data[L"a"] = data_ptr(new DataMap(sub_data)) ;
		data_ptr value = parse_val(L"a.b.c", data) ;

		BOOST_CHECK_EQUAL( value->getvalue(), L"d" ) ;
	}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( TestToken )

	using namespace cpptempl ;

	// TokenVar
	BOOST_AUTO_TEST_CASE(TestTokenVarType)
	{
		TokenVar token(L"foo") ;
		BOOST_CHECK_EQUAL( token.gettype(), L"var" ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenVar)
	{
		TokenVar token(L"foo") ;
		data_map data ;
		data[L"foo"] = make_data(L"bar") ;
		BOOST_CHECK_EQUAL( token.gettext(data), L"bar" ) ;
	}

	// TokenText
	BOOST_AUTO_TEST_CASE(TestTokenTextType)
	{
		TokenText token(L"foo") ;
		BOOST_CHECK_EQUAL( token.gettype(), L"text" ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenText)
	{
		TokenText token(L"foo") ;
		data_map data ;
		data[L"foo"] = make_data(L"bar") ;
		BOOST_CHECK_EQUAL( token.gettext(data), L"foo" ) ;
	}

	// TokenFor
	BOOST_AUTO_TEST_CASE(TestTokenForBadSyntax)
	{
		token_vector children ;
		BOOST_CHECK_THROW(TokenFor token(L"foo", children), TemplateException ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenForType)
	{
		token_vector children ;
		TokenFor token(L"for item in items", children) ;
		BOOST_CHECK_EQUAL( token.gettype(), L"for" ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenForTextEmpty)
	{
		token_vector children ;
		TokenFor token(L"for item in items", children) ;
		data_map data ;
		data_list items ;
		items.push_back(make_data(L"first")); 
		data[L"items"] = make_data(items) ;
		BOOST_CHECK_EQUAL( token.gettext(data), L"" ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenForTextOneVar)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenVar(L"item"))) ;
		TokenFor token(L"for item in items", children) ;
		data_map data ;
		data_list items ;
		items.push_back(make_data(L"first ")); 
		items.push_back(make_data(L"second ")); 
		data[L"items"] = make_data(items) ;
		BOOST_CHECK_EQUAL( token.gettext(data), L"first second " ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenForTextOneVarLoop)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenVar(L"loop"))) ;
		TokenFor token(L"for item in items", children) ;
		data_map data ;
		data_list items ;
		items.push_back(make_data(L"first ")); 
		items.push_back(make_data(L"second ")); 
		data[L"items"] = make_data(items) ;
		BOOST_CHECK_EQUAL( token.gettext(data), L"12" ) ;
	}	
	BOOST_AUTO_TEST_CASE(TestTokenForLoopTextVar)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenVar(L"loop"))) ;
		children.push_back(token_ptr(new TokenText(L". "))) ;
		children.push_back(token_ptr(new TokenVar(L"item"))) ;
		children.push_back(token_ptr(new TokenText(L" "))) ;
		TokenFor token(L"for item in items", children) ;
		data_map data ;
		data_list items ;
		items.push_back(make_data(L"first")); 
		items.push_back(make_data(L"second")); 
		data[L"items"] = make_data(items) ;
		BOOST_CHECK_EQUAL( token.gettext(data), L"1. first 2. second " ) ;
	}	
	BOOST_AUTO_TEST_CASE(TestTokenForTextOneText)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenText(L"{--}"))) ;
		TokenFor token(L"for item in items", children) ;
		data_map data ;
		data_list items ;
		items.push_back(make_data(L"first ")); 
		items.push_back(make_data(L"second ")); 
		data[L"items"] = make_data(items) ;
		BOOST_CHECK_EQUAL( token.gettext(data), L"{--}{--}" ) ;
	}

	//////////////////////////////////////////////////////////////////////////
	// TokenIf
	//////////////////////////////////////////////////////////////////////////

	BOOST_AUTO_TEST_CASE(TestTokenIfType)
	{
		token_vector children ;
		TokenIf token(L"if items", children) ;
		BOOST_CHECK_EQUAL( token.gettype(), L"if" ) ;
	}
	// if not empty
	BOOST_AUTO_TEST_CASE(TestTokenIfTrueText)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenText(L"{--}"))) ;
		TokenIf token(L"if item", children) ;
		data_map data ;
		data[L"item"] = make_data(L"foo") ;
		BOOST_CHECK_EQUAL( token.gettext(data), L"{--}" ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenIfTrueVar)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenVar(L"item"))) ;
		TokenIf token(L"if item", children) ;
		data_map data ;
		data[L"item"] = make_data(L"foo") ;
		BOOST_CHECK_EQUAL( token.gettext(data), L"foo" ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenIfFalse)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenText(L"{--}"))) ;
		TokenIf token(L"if item", children) ;
		data_map data ;
		data[L"item"] = make_data(L"") ;
		BOOST_CHECK_EQUAL( token.gettext(data), L"") ;
	}

	// ==
	BOOST_AUTO_TEST_CASE(TestTokenIfEqualsTrue)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenVar(L"item"))) ;
		TokenIf token(L"if item == \"foo\"", children) ;
		data_map data ;
		data[L"item"] = make_data(L"foo") ;
		BOOST_CHECK_EQUAL( token.gettext(data), L"foo" ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenIfEqualsFalse)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenVar(L"item"))) ;
		TokenIf token(L"if item == \"bar\"", children) ;
		data_map data ;
		data[L"item"] = make_data(L"foo") ;
		BOOST_CHECK_EQUAL( token.gettext(data), L"" ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenIfEqualsTwoVarsTrue)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenVar(L"item"))) ;
		TokenIf token(L"if item == foo", children) ;
		data_map data ;
		data[L"item"] = make_data(L"x") ;
		data[L"foo"] = make_data(L"x") ;
		BOOST_CHECK_EQUAL( token.gettext(data), L"x" ) ;
	}

	// !=
	BOOST_AUTO_TEST_CASE(TestTokenIfNotEqualsTrue)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenVar(L"item"))) ;
		TokenIf token(L"if item != \"foo\"", children) ;
		data_map data ;
		data[L"item"] = make_data(L"foo") ;
		BOOST_CHECK_EQUAL( token.gettext(data), L"" ) ;
	}
	BOOST_AUTO_TEST_CASE(TestTokenIfNotEqualsFalse)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenVar(L"item"))) ;
		TokenIf token(L"if item != \"bar\"", children) ;
		data_map data ;
		data[L"item"] = make_data(L"foo") ;
		BOOST_CHECK_EQUAL( token.gettext(data), L"foo" ) ;
	}

	// not
	BOOST_AUTO_TEST_CASE(TestTokenIfNotTrueText)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenText(L"{--}"))) ;
		TokenIf token(L"if not item", children) ;
		data_map data ;
		data[L"item"] = make_data(L"foo") ;
		BOOST_CHECK_EQUAL( token.gettext(data), L"") ;
	}

	BOOST_AUTO_TEST_CASE(TestTokenIfNotFalseText)
	{
		token_vector children ;
		children.push_back(token_ptr(new TokenText(L"{--}"))) ;
		TokenIf token(L"if not item", children) ;
		data_map data ;
		data[L"item"] = make_data(L"") ;
		BOOST_CHECK_EQUAL( token.gettext(data), L"{--}") ;
	}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( TestTokenize )

	BOOST_AUTO_TEST_CASE(test_empty)
	{
		std::wstring text = L"" ;
		cpptempl::token_vector tokens ;
		cpptempl::tokenize(text, tokens) ;

		BOOST_CHECK_EQUAL( 0u, tokens.size() ) ;
	}

BOOST_AUTO_TEST_SUITE_END()

