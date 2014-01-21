#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-VGA");
    BOOST_CHECK(GetBoolArg("-VGA"));
    BOOST_CHECK(GetBoolArg("-VGA", false));
    BOOST_CHECK(GetBoolArg("-VGA", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-VGAo"));
    BOOST_CHECK(!GetBoolArg("-VGAo", false));
    BOOST_CHECK(GetBoolArg("-VGAo", true));

    ResetArgs("-VGA=0");
    BOOST_CHECK(!GetBoolArg("-VGA"));
    BOOST_CHECK(!GetBoolArg("-VGA", false));
    BOOST_CHECK(!GetBoolArg("-VGA", true));

    ResetArgs("-VGA=1");
    BOOST_CHECK(GetBoolArg("-VGA"));
    BOOST_CHECK(GetBoolArg("-VGA", false));
    BOOST_CHECK(GetBoolArg("-VGA", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noVGA");
    BOOST_CHECK(!GetBoolArg("-VGA"));
    BOOST_CHECK(!GetBoolArg("-VGA", false));
    BOOST_CHECK(!GetBoolArg("-VGA", true));

    ResetArgs("-noVGA=1");
    BOOST_CHECK(!GetBoolArg("-VGA"));
    BOOST_CHECK(!GetBoolArg("-VGA", false));
    BOOST_CHECK(!GetBoolArg("-VGA", true));

    ResetArgs("-VGA -noVGA");  // -VGA should win
    BOOST_CHECK(GetBoolArg("-VGA"));
    BOOST_CHECK(GetBoolArg("-VGA", false));
    BOOST_CHECK(GetBoolArg("-VGA", true));

    ResetArgs("-VGA=1 -noVGA=1");  // -VGA should win
    BOOST_CHECK(GetBoolArg("-VGA"));
    BOOST_CHECK(GetBoolArg("-VGA", false));
    BOOST_CHECK(GetBoolArg("-VGA", true));

    ResetArgs("-VGA=0 -noVGA=0");  // -VGA should win
    BOOST_CHECK(!GetBoolArg("-VGA"));
    BOOST_CHECK(!GetBoolArg("-VGA", false));
    BOOST_CHECK(!GetBoolArg("-VGA", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--VGA=1");
    BOOST_CHECK(GetBoolArg("-VGA"));
    BOOST_CHECK(GetBoolArg("-VGA", false));
    BOOST_CHECK(GetBoolArg("-VGA", true));

    ResetArgs("--noVGA=1");
    BOOST_CHECK(!GetBoolArg("-VGA"));
    BOOST_CHECK(!GetBoolArg("-VGA", false));
    BOOST_CHECK(!GetBoolArg("-VGA", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-VGA", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-VGA", "eleven"), "eleven");

    ResetArgs("-VGA -bar");
    BOOST_CHECK_EQUAL(GetArg("-VGA", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-VGA", "eleven"), "");

    ResetArgs("-VGA=");
    BOOST_CHECK_EQUAL(GetArg("-VGA", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-VGA", "eleven"), "");

    ResetArgs("-VGA=11");
    BOOST_CHECK_EQUAL(GetArg("-VGA", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-VGA", "eleven"), "11");

    ResetArgs("-VGA=eleven");
    BOOST_CHECK_EQUAL(GetArg("-VGA", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-VGA", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-VGA", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-VGA", 0), 0);

    ResetArgs("-VGA -bar");
    BOOST_CHECK_EQUAL(GetArg("-VGA", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-VGA=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-VGA", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-VGA=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-VGA", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--VGA");
    BOOST_CHECK_EQUAL(GetBoolArg("-VGA"), true);

    ResetArgs("--VGA=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-VGA", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noVGA");
    BOOST_CHECK(!GetBoolArg("-VGA"));
    BOOST_CHECK(!GetBoolArg("-VGA", true));
    BOOST_CHECK(!GetBoolArg("-VGA", false));

    ResetArgs("-noVGA=1");
    BOOST_CHECK(!GetBoolArg("-VGA"));
    BOOST_CHECK(!GetBoolArg("-VGA", true));
    BOOST_CHECK(!GetBoolArg("-VGA", false));

    ResetArgs("-noVGA=0");
    BOOST_CHECK(GetBoolArg("-VGA"));
    BOOST_CHECK(GetBoolArg("-VGA", true));
    BOOST_CHECK(GetBoolArg("-VGA", false));

    ResetArgs("-VGA --noVGA");
    BOOST_CHECK(GetBoolArg("-VGA"));

    ResetArgs("-noVGA -VGA"); // VGA always wins:
    BOOST_CHECK(GetBoolArg("-VGA"));
}

BOOST_AUTO_TEST_SUITE_END()
