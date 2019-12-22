//---------------------------------------------------------------------------//
// Copyright (c) 2018-2019 Nil Foundation AG
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nil.foundation>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_MAC_VALUE_HPP
#define CRYPTO3_MAC_VALUE_HPP

#include <type_traits>
#include <iterator>

#include <boost/assert.hpp>
#include <boost/concept_check.hpp>

#include <boost/range/concepts.hpp>

namespace nil {
    namespace crypto3 {
        namespace mac {
            namespace detail {
                template<typename MacAccumulator>
                struct ref_cipher_impl {
                    typedef MacAccumulator accumulator_set_type;
                    typedef
                        typename boost::mpl::front<typename accumulator_set_type::features_type>::type accumulator_type;

                    typedef typename MacAccumulator::mode_type mode_type;
                    typedef typename mode_type::encoder_type cipher_type;

                    ref_cipher_impl(const accumulator_set_type &acc) : accumulator_set(acc) {
                    }

                    accumulator_set_type &accumulator_set;
                };

                template<typename MacAccumulator>
                struct value_cipher_impl {
                    typedef MacAccumulator accumulator_set_type;
                    typedef
                        typename boost::mpl::front<typename accumulator_set_type::features_type>::type accumulator_type;

                    typedef typename MacAccumulator::mode_type mode_type;
                    typedef typename mode_type::encoder_type cipher_type;

                    value_cipher_impl(const accumulator_set_type &acc) : accumulator_set(acc) {
                    }

                    mutable accumulator_set_type accumulator_set;
                };

                template<typename MacStateImpl>
                struct range_cipher_impl : public MacStateImpl {
                    typedef MacStateImpl cipher_state_impl_type;

                    typedef typename cipher_state_impl_type::accumulator_type accumulator_type;
                    typedef typename cipher_state_impl_type::accumulator_set_type accumulator_set_type;

                    typedef typename cipher_state_impl_type::mode_type mode_type;
                    typedef typename cipher_state_impl_type::cipher_type cipher_type;

                    typedef typename boost::mpl::apply<accumulator_set_type, accumulator_type>::type::result_type
                        result_type;

                    template<typename SinglePassRange>
                    range_cipher_impl(const SinglePassRange &range, const accumulator_set_type &ise) :
                        MacStateImpl(ise) {
                        BOOST_RANGE_CONCEPT_ASSERT((boost::SinglePassRangeConcept<const SinglePassRange>));

                        typedef
                            typename std::iterator_traits<typename SinglePassRange::iterator>::value_type value_type;
                        BOOST_STATIC_ASSERT(std::numeric_limits<value_type>::is_specialized);
                        typedef typename cipher_type::template stream_processor<
                            mode_type, accumulator_set_type,
                            std::numeric_limits<value_type>::digits + std::numeric_limits<value_type>::is_signed>::type
                            stream_processor;

                        stream_processor(this->accumulator_set)(range.begin(), range.end());
                    }

                    template<typename InputIterator>
                    range_cipher_impl(InputIterator first, InputIterator last, const accumulator_set_type &ise) :
                        MacStateImpl(ise) {
                        BOOST_CONCEPT_ASSERT((boost::InputIteratorConcept<InputIterator>));

                        typedef typename std::iterator_traits<InputIterator>::value_type value_type;
                        BOOST_STATIC_ASSERT(std::numeric_limits<value_type>::is_specialized);
                        typedef typename cipher_type::template stream_processor<
                            mode_type, accumulator_set_type,
                            std::numeric_limits<value_type>::digits + std::numeric_limits<value_type>::is_signed>::type
                            stream_processor;

                        stream_processor(this->accumulator_set)(first, last);
                    }

                    template<typename T, std::size_t Size>
                    inline operator std::array<T, Size>() const {
                        result_type result =
                            boost::accumulators::extract_result<accumulator_type>(this->accumulator_set);
                        std::array<T, Size> out;
                        std::copy(result.begin(), result.end(), out.end());
                        return out;
                    }

                    template<typename T, std::size_t Size>
                    inline operator boost::array<T, Size>() const {
                        result_type result =
                            boost::accumulators::extract_result<accumulator_type>(this->accumulator_set);
                        boost::array<T, Size> out;
                        std::copy(result.begin(), result.end(), out.end());
                        return out;
                    }

                    template<typename OutputRange>
                    operator OutputRange() const {
                        result_type result =
                            boost::accumulators::extract_result<accumulator_type>(this->accumulator_set);
                        return OutputRange(result.cbegin(), result.cend());
                    }

                    operator result_type() const {
                        return boost::accumulators::extract_result<accumulator_type>(this->accumulator_set);
                    }

                    operator accumulator_set_type() const {
                        return this->accumulator_set;
                    }

#ifdef CRYPTO3_ASCII_STRING_CODEC_OUTPUT

                    template<typename Char, typename CharTraits, typename Alloc>
                    operator std::basic_string<Char, CharTraits, Alloc>() const {
                        return std::to_string(
                            boost::accumulators::extract_result<accumulator_type>(this->accumulator_set));
                    }

#endif
                };

                template<typename MacStateImpl, typename OutputIterator>
                struct itr_cipher_impl : public MacStateImpl {
                private:
                    mutable OutputIterator out;

                public:
                    typedef MacStateImpl cipher_state_impl_type;

                    typedef typename cipher_state_impl_type::accumulator_type accumulator_type;
                    typedef typename cipher_state_impl_type::accumulator_set_type accumulator_set_type;

                    typedef typename cipher_state_impl_type::mode_type mode_type;
                    typedef typename cipher_state_impl_type::cipher_type cipher_type;

                    typedef typename boost::mpl::apply<accumulator_set_type, accumulator_type>::type::result_type
                        result_type;

                    template<typename SinglePassRange>
                    itr_cipher_impl(const SinglePassRange &range, OutputIterator out, const accumulator_set_type &ise) :
                        MacStateImpl(ise), out(std::move(out)) {
                        BOOST_CONCEPT_ASSERT((boost::SinglePassRangeConcept<const SinglePassRange>));

                        typedef
                            typename std::iterator_traits<typename SinglePassRange::iterator>::value_type value_type;
                        BOOST_STATIC_ASSERT(std::numeric_limits<value_type>::is_specialized);
                        typedef typename cipher_type::template stream_processor<
                            mode_type, accumulator_set_type,
                            std::numeric_limits<value_type>::digits + std::numeric_limits<value_type>::is_signed>::type
                            stream_processor;

                        stream_processor(this->accumulator_set)(range.begin(), range.end());
                    }

                    template<typename InputIterator>
                    itr_cipher_impl(InputIterator first, InputIterator last, OutputIterator out,
                                    const accumulator_set_type &ise) :
                        MacStateImpl(ise),
                        out(std::move(out)) {
                        BOOST_CONCEPT_ASSERT((boost::InputIteratorConcept<InputIterator>));

                        typedef typename std::iterator_traits<InputIterator>::value_type value_type;
                        BOOST_STATIC_ASSERT(std::numeric_limits<value_type>::is_specialized);
                        typedef typename cipher_type::template stream_processor<
                            mode_type, accumulator_set_type,
                            std::numeric_limits<value_type>::digits + std::numeric_limits<value_type>::is_signed>::type
                            stream_processor;

                        stream_processor(this->accumulator_set)(first, last);
                    }

                    operator OutputIterator() const {
                        result_type result =
                            boost::accumulators::extract_result<accumulator_type>(this->accumulator_set);

                        return std::move(result.cbegin(), result.cend(), out);
                    }
                };
            }    // namespace detail
        }        // namespace mac
    }            // namespace crypto3
}    // namespace nil

#endif    // CRYPTO3_CODEC_POSTPROCESSOR_HPP
