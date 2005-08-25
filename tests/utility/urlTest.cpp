//
// VMime library (http://www.vmime.org)
// Copyright (C) 2002-2005 Vincent Richard <vincent@vincent-richard.net>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include "tests/testUtils.hpp"

#include "vmime/utility/url.hpp"
#include "vmime/utility/urlUtils.hpp"


#define VMIME_TEST_SUITE         urlTest
#define VMIME_TEST_SUITE_MODULE  "Utility"


VMIME_TEST_SUITE_BEGIN

	VMIME_TEST_LIST_BEGIN
		VMIME_TEST(testParse1)
		VMIME_TEST(testParse2)
		VMIME_TEST(testParse3)
		VMIME_TEST(testParse4)
		VMIME_TEST(testGenerate)
		VMIME_TEST(testUtilsEncode)
		VMIME_TEST(testUtilsDecode)
	VMIME_TEST_LIST_END


	static const bool parseHelper(vmime::utility::url& u, const vmime::string& str)
	{
		try
		{
			u = vmime::utility::url(str);
		}
		catch (vmime::exceptions::malformed_url)
		{
			return false;
		}

		return true;
	}


	void testParse1()
	{
		// Test some valid constructions
		vmime::utility::url u1("", "");

		VASSERT_EQ("1.1", true, parseHelper(u1, "protocol://user:password@host:12345/path/"));
		VASSERT_EQ("1.2", "protocol", u1.getProtocol());
		VASSERT_EQ("1.3", "user", u1.getUsername());
		VASSERT_EQ("1.4", "password", u1.getPassword());
		VASSERT_EQ("1.5", "host", u1.getHost());
		VASSERT_EQ("1.6", 12345, u1.getPort());
		VASSERT_EQ("1.7", "/path/", u1.getPath());

		vmime::utility::url u2("", "");

		VASSERT_EQ("2.1", true, parseHelper(u2, "protocol://user@host:12345/path/"));
		VASSERT_EQ("2.2", "protocol", u2.getProtocol());
		VASSERT_EQ("2.3", "user", u2.getUsername());
		VASSERT_EQ("2.4", "", u2.getPassword());
		VASSERT_EQ("2.5", "host", u2.getHost());
		VASSERT_EQ("2.6", 12345, u2.getPort());
		VASSERT_EQ("2.7", "/path/", u2.getPath());

		vmime::utility::url u3("", "");

		VASSERT_EQ("3.1", true, parseHelper(u3, "protocol://host:12345/path/"));
		VASSERT_EQ("3.2", "protocol", u3.getProtocol());
		VASSERT_EQ("3.3", "", u3.getUsername());
		VASSERT_EQ("3.4", "", u3.getPassword());
		VASSERT_EQ("3.5", "host", u3.getHost());
		VASSERT_EQ("3.6", 12345, u3.getPort());
		VASSERT_EQ("3.7", "/path/", u3.getPath());

		vmime::utility::url u4("", "");

		VASSERT_EQ("4.1", true, parseHelper(u4, "protocol://host/path/"));
		VASSERT_EQ("4.2", "protocol", u4.getProtocol());
		VASSERT_EQ("4.3", "", u4.getUsername());
		VASSERT_EQ("4.4", "", u4.getPassword());
		VASSERT_EQ("4.5", "host", u4.getHost());
		VASSERT_EQ("4.6", vmime::utility::url::UNSPECIFIED_PORT, u4.getPort());
		VASSERT_EQ("4.7", "/path/", u4.getPath());

		vmime::utility::url u5("", "");

		VASSERT_EQ("5.1", true, parseHelper(u5, "protocol://host/"));
		VASSERT_EQ("5.2", "protocol", u5.getProtocol());
		VASSERT_EQ("5.3", "", u5.getUsername());
		VASSERT_EQ("5.4", "", u5.getPassword());
		VASSERT_EQ("5.5", "host", u5.getHost());
		VASSERT_EQ("5.6", vmime::utility::url::UNSPECIFIED_PORT, u4.getPort());
		VASSERT_EQ("5.7", "", u5.getPath());

		vmime::utility::url u6("", "");

		VASSERT_EQ("6.1", true, parseHelper(u4, "protocol://host/path/file"));
		VASSERT_EQ("6.2", "protocol", u4.getProtocol());
		VASSERT_EQ("6.3", "", u4.getUsername());
		VASSERT_EQ("6.4", "", u4.getPassword());
		VASSERT_EQ("6.5", "host", u4.getHost());
		VASSERT_EQ("6.6", vmime::utility::url::UNSPECIFIED_PORT, u4.getPort());
		VASSERT_EQ("6.7", "/path/file", u4.getPath());
	}

	void testParse2()
	{
		// Now, test some ill-formed URLs

		// -- missing protocol
		vmime::utility::url u1("", "");
		VASSERT_EQ("1", false, parseHelper(u1, "://host"));

		// -- port can contain only digits
		vmime::utility::url u2("", "");
		VASSERT_EQ("2", false, parseHelper(u2, "proto://host:abc123"));

		// -- no host specified
		vmime::utility::url u3("", "");
		VASSERT_EQ("3", false, parseHelper(u3, "proto:///path"));

		// -- no protocol separator (://)
		vmime::utility::url u4("", "");
		VASSERT_EQ("4", false, parseHelper(u4, "protohost/path"));
	}

	void testParse3()
	{
		// Test decoding
		vmime::utility::url u1("", "");

		VASSERT_EQ("1.1", true, parseHelper(u1, "pro%12to://user%34:pass%56word@ho%78st:12345/pa%abth/"));
		VASSERT_EQ("1.2", "pro%12to", u1.getProtocol());  // protocol should not be decoded
		VASSERT_EQ("1.3", "user\x34", u1.getUsername());
		VASSERT_EQ("1.4", "pass\x56word", u1.getPassword());
		VASSERT_EQ("1.5", "ho\x78st", u1.getHost());
		VASSERT_EQ("1.6", 12345, u1.getPort());
		VASSERT_EQ("1.7", "/pa\xabth/", u1.getPath());
	}

	void testParse4()
	{
		// Test parameters
		vmime::utility::url u1("", "");

		VASSERT_EQ("1.1", true, parseHelper(u1, "proto://host/path?p1=v1&p2=v2"));
		VASSERT_EQ("1.2", "v1", u1.getParams().getProperty <vmime::string>("p1"));
		VASSERT_EQ("1.3", "v2", u1.getParams().getProperty <vmime::string>("p2"));
		VASSERT_EQ("1.4", "/path", u1.getPath());

		vmime::utility::url u2("", "");

		VASSERT_EQ("2.1", true, parseHelper(u2, "proto://host/path?p1=v1&p2"));
		VASSERT_EQ("2.2", "v1", u2.getParams().getProperty <vmime::string>("p1"));
		VASSERT_EQ("2.3", "p2", u2.getParams().getProperty <vmime::string>("p2"));
		VASSERT_EQ("2.4", "/path", u2.getPath());

		vmime::utility::url u3("", "");

		VASSERT_EQ("3.1", true, parseHelper(u3, "proto://host/?p1=v1&p2=v2"));
		VASSERT_EQ("3.2", "v1", u3.getParams().getProperty <vmime::string>("p1"));
		VASSERT_EQ("3.3", "v2", u3.getParams().getProperty <vmime::string>("p2"));
		VASSERT_EQ("3.4", "", u3.getPath());

		vmime::utility::url u4("", "");

		VASSERT_EQ("4.1", true, parseHelper(u4, "proto://host/path?p1=%3D&%3D=v2"));
		VASSERT_EQ("4.2", "=", u4.getParams().getProperty <vmime::string>("p1"));
		VASSERT_EQ("4.3", "v2", u4.getParams().getProperty <vmime::string>("="));
		VASSERT_EQ("4.4", "/path", u4.getPath());
	}

	void testGenerate()
	{
		vmime::utility::url u1("proto", "host", 12345, "path", "user", "password");
		VASSERT_EQ("1", "proto://user:password@host:12345/path",
			static_cast <vmime::string>(u1));

		vmime::utility::url u2("proto", "host");
		VASSERT_EQ("2", "proto://host", static_cast <vmime::string>(u2));

		vmime::utility::url u3("proto", "host");
		u3.getParams().setProperty("p1", "v1");
		VASSERT_EQ("3.1", "proto://host/?p1=v1",
			static_cast <vmime::string>(u3));
		u3.getParams().setProperty("p2", "v2");
		VASSERT_EQ("3.2", "proto://host/?p1=v1&p2=v2",
			static_cast <vmime::string>(u3));
		u3.getParams().setProperty("&", "=");
		VASSERT_EQ("3.3", "proto://host/?p1=v1&p2=v2&%26=%3D",
			static_cast <vmime::string>(u3));
	}

	void testUtilsEncode()
	{
		VASSERT_EQ("1", "%01", vmime::utility::urlUtils::encode("\x01"));
		VASSERT_EQ("2", "%20", vmime::utility::urlUtils::encode(" "));
		VASSERT_EQ("3", "%FF", vmime::utility::urlUtils::encode("\xff"));
		VASSERT_EQ("4", "a", vmime::utility::urlUtils::encode("a"));
	}

	void testUtilsDecode()
	{
		for (int i = 0 ; i < 255 ; ++i)
		{
			std::ostringstream ossTest;
			ossTest << "%" << "0123456789ABCDEF"[i / 16]
				       << "0123456789ABCDEF"[i % 16];

			std::ostringstream ossNum;
			ossNum << i;

			vmime::string res;
			res += static_cast <unsigned char>(i);

			VASSERT_EQ(ossNum.str(), res,
				vmime::utility::urlUtils::decode(ossTest.str()));
		}

	}

VMIME_TEST_SUITE_END

