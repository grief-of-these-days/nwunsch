#pragma once
#include <vector>
#include <algorithm>
#include <functional>
#include <type_traits>


class NeedlemanWunsch {
public:
    /**
     * @brief This routine is a modified version of Needleman-Wunsch algorithm
     *        that aligns the sequence defined by char_begin and char_end iterators
     *        against the reference sequence defined by ref_begin and ref_end, removing
     *        some elements, if nessesary. Reference sequence stays unchanged, while
     *        the aligned source sequence is written into dest iterator.
     *
     * @param char_begin Source sequence start iterator.
     * @param char_end Source sequence end iterator.
     * @param ref_begin Reference sequence start iterator.
     * @param ref_end Reference sequence end iterator.
     * @param dest Destination (bidirectional) iterator. Dest container must have the same capacity as the reference sequence.
     * @param f Match function, taking source char and reference char.
     */
    template <class CharIter, class RefIter, class DestIter, class Score>
    void align (
        CharIter char_begin, CharIter char_end,
        RefIter ref_begin, RefIter ref_end,
        DestIter dest_begin,
        Score f);

protected:
    /// Insertion and deletion penalty.
    static constexpr int d = -1;

    /// Char to insert in a source sequence as default.
    template<typename T>
    static constexpr T defChar () {
        return T{};
    }

    std::vector<int> score_;

    template <class CharIter, class RefIter, class Score>
    void nwscore (CharIter char_begin, CharIter char_end, RefIter ref_begin, RefIter ref_end, Score f);
};

template<>
constexpr char NeedlemanWunsch::defChar () {
    return '*';
}

template<>
constexpr wchar_t NeedlemanWunsch::defChar () {
    return L'*';
}


template <class CharIter, class RefIter, class Score>
void NeedlemanWunsch::nwscore (CharIter char_begin, CharIter char_end, RefIter ref_begin, RefIter ref_end, Score f) {
    const size_t sz_x = std::distance (char_begin, char_end);
    const size_t sz_y = std::distance (ref_begin, ref_end);
    const size_t sz = std::max (sz_x, sz_y) + 1;

    score_.resize (sz * sz);

    for (size_t j = 0; j <= sz_x; j++) {
        score_[j] = static_cast<int>(j * d);
    }

    for (size_t j = 0; j <= sz_y; j++) {
        score_[j * sz] = static_cast<int>(j * d);
    }

    auto x = char_begin;
    for (size_t i = 1; i <= sz_x; i++, x++) {
        auto y = ref_begin;
        for (size_t j = 1; j <= sz_y; j++, y++) {
            score_[j * sz + i] =
                std::max ({
                    score_[(j - 1) * sz + i - 1] + f (*x, *y),
                    score_[j * sz + i - 1] + d,
                    score_[(j - 1) * sz + i] + d });
        }
    }
}

template <class CharIter, class RefIter, class DestIter, class Score>
void NeedlemanWunsch::align (
    CharIter char_begin, CharIter char_end,
    RefIter ref_begin, RefIter ref_end,
    DestIter dest_begin, Score f) {

    using Char = typename std::iterator_traits<CharIter>::value_type;
    using Ref  = typename std::iterator_traits<RefIter>::value_type;

    static_assert (std::is_same<
        typename std::iterator_traits<CharIter>::value_type,
        typename std::iterator_traits<DestIter>::value_type>::value,
        "Source and destination sequences must have the same data type.");

    static_assert (std::is_convertible<Score, std::function<int(const Char&, const Ref&)>>::value,
        "Score callback must have a correct signature.");

    static_assert (std::is_same<typename std::result_of<Score(const Char&, const Ref&)>::type, int>::value,
        "Score callback must return int.");

    nwscore (char_begin, char_end, ref_begin, ref_end, f);

    const size_t sz_x = std::distance (char_begin, char_end);
    const size_t sz_y = std::distance (ref_begin, ref_end);
    const size_t sz = std::max (sz_x, sz_y) + 1;

    size_t i = sz_x;
    size_t j = sz_y;
    auto x = std::reverse_iterator<CharIter>(char_begin + sz_x);
    auto y = std::reverse_iterator<RefIter>(ref_begin + sz_y);
    auto dest = std::reverse_iterator<DestIter> (dest_begin + sz_y);
    while (i > 0 && j > 0) {
        const auto score = score_[j * sz + i];
        const auto score_diag = score_[(j - 1) * sz + i - 1];
        const auto score_left = score_[j * sz + i - 1];
        const auto score_up = score_[(j - 1) * sz + i];

        if (score == score_diag + f (*x, *y)) {
            *dest++ = *x;
            if (--i) ++x;
            if (--j) ++y;
        } else
        if (score == score_left + d) {
            if (--i) ++x;
        } else {
            *dest++ = defChar<Char> ();
            if (--j) ++y;
        }
    }

    while (i > 0) {
        if (--i) ++x;
    }

    while (j > 0) {
        *dest++ = defChar<Char> ();
        if (--j) ++y;
    }
}
