#include <boost/test/unit_test.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "../utilities/CRequest.h"
#include <iomanip>
#include <iostream>

using namespace std;

BOOST_AUTO_TEST_SUITE(CRequestTestSuit)

    BOOST_AUTO_TEST_CASE(TestSend) {
        CRequest request("https://www.google.com");
        request.Send();
        BOOST_CHECK_EQUAL(request.GetResponseCode(), 200);

    }
BOOST_AUTO_TEST_SUITE_END()