#include "tinyxml2.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined( WIN32 )
	#include <crtdbg.h>
	_CrtMemState startMemState;
	_CrtMemState endMemState;
#endif

using namespace tinyxml2;
int gPass = 0;
int gFail = 0;

bool XMLTest (const char* testString, const char* expected, const char* found, bool echo=true )
{
	bool pass = !strcmp( expected, found );
	if ( pass )
		printf ("[pass]");
	else
		printf ("[fail]");

	if ( !echo )
		printf (" %s\n", testString);
	else
		printf (" %s [%s][%s]\n", testString, expected, found);

	if ( pass )
		++gPass;
	else
		++gFail;
	return pass;
}


bool XMLTest( const char* testString, int expected, int found, bool echo=true )
{
	bool pass = ( expected == found );
	if ( pass )
		printf ("[pass]");
	else
		printf ("[fail]");

	if ( !echo )
		printf (" %s\n", testString);
	else
		printf (" %s [%d][%d]\n", testString, expected, found);

	if ( pass )
		++gPass;
	else
		++gFail;
	return pass;
}


int main( int argc, const char* argv )
{
	#if defined( WIN32 )
		_CrtMemCheckpoint( &startMemState );
	#endif	

#if 0 
	{
		static const char* test = "<!--hello world\n"
			                      "          line 2\r"
			                      "          line 3\r\n"
			                      "          line 4\n\r"
			                      "          line 5\r-->";

		XMLDocument doc;
		doc.Parse( test );
		doc.Print();
	}
#endif
#if 0
	{
		static const char* test[] = {	"<element />",
									    "<element></element>",
										"<element><subelement/></element>",
									    "<element><subelement></subelement></element>",
									    "<element><subelement><subsub/></subelement></element>",
									    "<!--comment beside elements--><element><subelement></subelement></element>",
									    "<!--comment beside elements, this time with spaces-->  \n <element>  <subelement> \n </subelement> </element>",
									    "<element attrib1='foo' attrib2=\"bar\" ></element>",
									    "<element attrib1='foo' attrib2=\"bar\" ><subelement attrib3='yeehaa' /></element>",
										"<element>Text inside element.</element>",
										"<element><b></b></element>",
										"<element>Text inside and <b>bolded</b> in the element.</element>",
										"<outer><element>Text inside and <b>bolded</b> in the element.</element></outer>",
										"<element>This &amp; That.</element>",
										"<element attrib='This&lt;That' />",
										0
		};
		for( int i=0; test[i]; ++i ) {
			XMLDocument doc;
			doc.Parse( test[i] );
			doc.Print();
			printf( "----------------------------------------------\n" );
		}
	}
#endif
#if 0
	{
		static const char* test = "<element>Text before.</element>";
		XMLDocument doc;
		doc.Parse( test );
		XMLElement* root = doc.FirstChildElement();
		XMLElement* newElement = doc.NewElement( "Subelement" );
		root->InsertEndChild( newElement );
		doc.Print();
	}
	{
		XMLDocument* doc = new XMLDocument();
		static const char* test = "<element><sub/></element>";
		doc->Parse( test );
		delete doc;
	}
#endif
	{
		// Test: Programmatic DOM
		// Build:
		//		<element>
		//			<!--comment-->
		//			<sub attrib="1" />
		//			<sub attrib="2" />
		//			<sub attrib="3" >& Text!</sub>
		//		<element>

		XMLDocument* doc = new XMLDocument();
		XMLNode* element = doc->InsertEndChild( doc->NewElement( "element" ) );

		XMLElement* sub[3] = { doc->NewElement( "sub" ), doc->NewElement( "sub" ), doc->NewElement( "sub" ) };
		for( int i=0; i<3; ++i ) {
			sub[i]->SetAttribute( "attrib", i );
		}
		element->InsertEndChild( sub[2] );
		XMLNode* comment = element->InsertFirstChild( doc->NewComment( "comment" ) );
		element->InsertAfterChild( comment, sub[0] );
		element->InsertAfterChild( sub[0], sub[1] );
		sub[2]->InsertFirstChild( doc->NewText( "& Text!" ));
		doc->Print();
		XMLTest( "Programmatic DOM", "comment", doc->FirstChildElement( "element" )->FirstChild()->Value() );
		XMLTest( "Programmatic DOM", "0", doc->FirstChildElement( "element" )->FirstChildElement()->Attribute( "attrib" ) );
		XMLTest( "Programmatic DOM", 2, doc->FirstChildElement()->LastChildElement( "sub" )->IntAttribute( "attrib" ) );
		XMLTest( "Programmatic DOM", "& Text!", 
				 doc->FirstChildElement()->LastChildElement( "sub" )->FirstChild()->ToText()->Value() );

		// And now deletion:
		element->DeleteChild( sub[2] );
		doc->DeleteNode( comment );

		element->FirstChildElement()->SetAttribute( "attrib", true );
		element->LastChildElement()->DeleteAttribute( "attrib" );

		XMLTest( "Programmatic DOM", true, doc->FirstChildElement()->FirstChildElement()->BoolAttribute( "attrib" ) );
		int value = 10;
		int result = doc->FirstChildElement()->LastChildElement()->QueryIntAttribute( "attrib", &value );
		XMLTest( "Programmatic DOM", result, NO_ATTRIBUTE );
		XMLTest( "Programmatic DOM", value, 10 );

		doc->Print();

		XMLStreamer streamer;
		doc->Print( &streamer );
		printf( "%s", streamer.CStr() );

		delete doc;
	}

	#if defined( WIN32 )
		_CrtMemCheckpoint( &endMemState );  
		//_CrtMemDumpStatistics( &endMemState );

		_CrtMemState diffMemState;
		_CrtMemDifference( &diffMemState, &startMemState, &endMemState );
		_CrtMemDumpStatistics( &diffMemState );
	#endif

	printf ("\nPass %d, Fail %d\n", gPass, gFail);
	return 0;
}