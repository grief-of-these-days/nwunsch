#include "../nwunsch.hpp"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"


SCENARIO ("Align simple strings", "[nwunsch]") {
    GIVEN ("Two simple char strings") {
        NeedlemanWunsch nw;

        std::string y = "aaabbbccd";
        std::string x = "aabcd";

        WHEN ("Aligned a shorter string against the longer one") {
            std::string res (y.size(), 0);
            nw.align (x.cbegin (), x.cend (), y.cbegin (), y.cend (), res.begin(), 
                [](char a, char b) {
                    return a == b ? 1 : -1;
                });

            THEN ("Aligned string must match the expected value") {
                REQUIRE (res == "*aa**b*cd");
            }
        }

        WHEN ("Aligned a longer string against the shorter one") {
            std::string res (x.size (), 0);
            nw.align (y.cbegin (), y.cend (), x.cbegin (), x.cend (), res.begin(),
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

        WHEN ("Called with two strings of different type") {
            NeedlemanWunsch nw;
            std::string res (y.size(), 0);
            nw.align (x.begin (), x.end (), y.begin (), y.end (), res.begin(), 
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

        WHEN ("Aligned a string against the rule") {
            NeedlemanWunsch nw;
            std::wstring res (y.size(), 0);
            nw.align (x.begin (), x.end (), y.begin (), y.end (), res.begin(),
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


SCENARIO ("Align multiple strings with wildcards", "[nwunsch]") {
    GIVEN ("A list of basic strings") {
        const std::vector<std::string> list = {
            "*12*bc777*",
            "a1***b771*",
            "a2**bc77*7",
            "*3**c*77**"
        };

        WHEN ("Aligned all strings in order") {
            NeedlemanWunsch nw;

            std::string res;
            for (const auto& y : list) {
                if (res.empty ())
                    res = y;
                else {
                    std::string y0 (y.size(), 0);
                    std::string y1 (res.size(), 0);

                    y0.resize (
                        std::distance (
                            y0.begin(), 
                            std::remove_copy (
                                y.cbegin (), y.cend (),
                                y0.begin (), '*')));

                    nw.align (
                        y0.begin (), y0.end (),
                        res.begin (), res.end (),
                        y1.begin (),
                        [](char a, char b) {
                            if (a == '*' || b == '*')
                                return 1;
                            else
                                return a == b ? 2 : -2;
                        });

                    std::transform (
                        res.cbegin (), res.cend (),
                        y1.cbegin (),
                        res.begin (),
                        [](char a, char b) {
                            return a == '*' ? b : a;
                        });
                }
            }

            THEN ("The final string must match the expected value") {
                REQUIRE (res == "a123bc7771");
            }
        }
    }
}
