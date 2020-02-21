#include "../nwunsch.hpp"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"


SCENARIO ("Align simple strings", "[nwunsch]") {
    GIVEN ("Two simple char strings") {
        std::string y = "aaabbbccd";
        std::string x = "aabcd";
        std::string res;

        WHEN ("Aligned a shorter string against the longer one") {
            NeedlemanWunsch nw;
            nw.align (x.begin (), x.end (), y.begin (), y.end (), std::back_inserter (res), 
                [](char a, char b) {
                    return a == b ? 1 : -1;
                });

            THEN ("Aligned string must match the expected value") {
                REQUIRE (res == "*aa**b*cd");
            }
        }

        WHEN ("Aligned a longer string against the shorter one") {
            NeedlemanWunsch nw;
            nw.align (y.begin (), y.end (), x.begin (), x.end (), std::back_inserter (res),
                [](char a, char b) {
                    return a == b ? 1 : -1;
                });

            THEN ("Aligned string must match the expected value") {
                REQUIRE (res == x);
            }
        }
    }

    GIVEN ("string and wstring") {
        std::wstring y = L"1234567890qwerty12";
        std::string x = "5wer";
        std::string res;

        WHEN ("Called with two strings of different type") {
            NeedlemanWunsch nw;
            nw.align (x.begin (), x.end (), y.begin (), y.end (), std::back_inserter (res), 
                [](char a, wchar_t b) {
                    return a == b ? 1 : -1;
                });

            THEN ("Aligned string must match the expected value") {
                REQUIRE (res == "****5******wer****");
            }
        }
    }
}

SCENARIO ("Align strings with rule sequence", "[nwunsch]") {
    struct Ref {
        bool is_digit;

        Ref (bool x) : is_digit (x) {}
    };

    GIVEN ("A basic string and a ref sequence") {
        std::vector<Ref> y = { true, false, false, true, true, false };
        std::wstring x = L"aa12bcd";
        std::wstring res;

        WHEN ("Aligned a string against the rule") {
            NeedlemanWunsch nw;
            nw.align (x.begin (), x.end (), y.begin (), y.end (), std::back_inserter (res),
                [](wchar_t a, const Ref & b) {
                    const bool res = iswdigit (a);
                    return b.is_digit == res ? 2 : -2;
                });

            THEN ("Aligned string must match the expected value") {
                REQUIRE (res == L"*aa12d");
            }
        }
    }
}
