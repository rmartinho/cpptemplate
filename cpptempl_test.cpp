#include "cpptempl.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE( TestData )

	BOOST_AUTO_TEST_CASE(test_gettext_throws)
	{
		cpptempl::Data data ;

		BOOST_CHECK_THROW( data.gettext(), cpptempl::TemplateException ) ;
	}

	BOOST_AUTO_TEST_CASE(test_getlist_throws)
	{
		cpptempl::Data data ;

		std::vector<std::wstring> items ;
		BOOST_CHECK_THROW( data.getlist(items), cpptempl::TemplateException ) ;
	}

	BOOST_AUTO_TEST_CASE(test_getitem_throws)
	{
		cpptempl::Data data ;

		BOOST_CHECK_THROW( data.getitem(), cpptempl::TemplateException ) ;
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

