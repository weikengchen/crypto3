//---------------------------------------------------------------------------//
// Copyright (c) 2018-2019 Nil Foundation AG
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2019 Aleksey Moskvin <zerg1996@yandex.ru>
// Copyright (c) 2020 Nikita Kaskov <nbering@nil.foundation>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_ACCUMULATORS_HASH_HPP
#define CRYPTO3_ACCUMULATORS_HASH_HPP

#include <boost/parameter/value_type.hpp>

#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/framework/depends_on.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>

#include <boost/container/static_vector.hpp>

#include <nil/crypto3/detail/make_array.hpp>
#include <nil/crypto3/detail/static_digest.hpp>
#include <nil/crypto3/detail/endian_shift.hpp>
#include <nil/crypto3/detail/unbounded_shift.hpp>
#include <nil/crypto3/detail/inject.hpp>

#include <nil/crypto3/hash/accumulators/bits_count.hpp>

#include <nil/crypto3/hash/accumulators/parameters/bits.hpp>
#include <nil/crypto3/hash/accumulators/parameters/salt.hpp>

#include <boost/accumulators/statistics/count.hpp>

namespace nil {
    namespace crypto3 {
        namespace accumulators {
            namespace impl {
                template<typename Hash>
                struct hash_impl : boost::accumulators::accumulator_base {
                protected:
                    typedef Hash hash_type;
                    typedef typename hash_type::construction::type construction_type;
                    typedef typename hash_type::construction::params_type params_type;


                    typedef typename params_type::digest_endian endian_type;


                    constexpr static const std::size_t word_bits = construction_type::word_bits;
                    typedef typename construction_type::word_type word_type;

                    constexpr static const std::size_t state_bits = construction_type::state_bits;
                    constexpr static const std::size_t state_words = construction_type::state_words;
                    typedef typename construction_type::state_type state_type;

                    constexpr static const std::size_t block_bits = construction_type::block_bits;
                    constexpr static const std::size_t block_words = construction_type::block_words;
                    typedef typename construction_type::block_type block_type;

                    constexpr static const std::size_t length_bits = params_type::length_bits;
                    // FIXME: do something more intelligent than capping at 64
                    constexpr static const std::size_t length_type_bits =
                        length_bits < word_bits ? word_bits : length_bits > 64 ? 64 : length_bits;
                    typedef typename boost::uint_t<length_type_bits>::least length_type;
                    constexpr static const std::size_t length_words = length_bits / word_bits;
                    BOOST_STATIC_ASSERT(!length_bits || length_bits % word_bits == 0);


                    typedef ::nil::crypto3::detail::injector<endian_type, word_bits, block_words, block_bits> injector;

                public:
                    typedef typename hash_type::digest_type result_type;

                    // The constructor takes an argument pack.
                    hash_impl(boost::accumulators::dont_care) : total_seen(0) {
                    }

                    template<typename ArgumentPack>
                    inline void operator()(const ArgumentPack &args) {
                        total_seen = extract::bits_count(args);
                        resolve_type(args[boost::accumulators::sample],
                                     args[::nil::crypto3::accumulators::bits | std::size_t()]);
                    }

                    inline result_type result(boost::accumulators::dont_care) const {
                        construction_type res = construction;
                        return res.digest(cache, total_seen);
                    }

                protected:

                    inline void resolve_type(const block_type &value, std::size_t bits) {
                        process(value, bits);
                    }

                    inline void resolve_type(const word_type &value, std::size_t bits) {
                        process(value, bits);
                    }


                    inline void process(const block_type &value, std::size_t value_seen) {
                        using namespace ::nil::crypto3::detail;

                        std::size_t cached_bits = (total_seen - value_seen) % block_bits;

                        if (cached_bits != 0) {
                            //If there are already any bits in the cache

                            std::size_t needed_to_fill_bits = block_bits - cached_bits;
                            std::size_t new_bits_to_append = (needed_to_fill_bits > value_seen)? value_seen : needed_to_fill_bits;

                            injector::inject(value, new_bits_to_append, cache, cached_bits);
                            // FIXME: this case is never true
                            if (cached_bits == block_bits) {
                                //If there are enough bits in the incoming value to fill the block

                                construction.process_block(cache, total_seen - value_seen + new_bits_to_append);

                                if (value_seen > new_bits_to_append){
                                    //If there are some remaining bits in the incoming value - put them into the cache, which is now empty

                                    std::size_t added_words = new_bits_to_append/word_bits;



                                    std::size_t remaining_bits = value_seen - new_bits_to_append;

                                    std::size_t remaining_in_last_word_bits = (remaining_bits > (word_bits - new_bits_to_append%word_bits) )? word_bits - new_bits_to_append%word_bits : remaining_bits;

                                    remaining_bits -= remaining_in_last_word_bits;

                                    word_type first_word_shifted = value[added_words];

                                    endian_shift<endian_type, word_bits>::to_msb(first_word_shifted, new_bits_to_append%word_bits);
                                    
                                    cached_bits = 0;

                                    injector::inject(first_word_shifted, remaining_in_last_word_bits, cache, cached_bits);

                                    for (std::size_t i = 0; i< (remaining_bits / word_bits); i++){
                                         injector::inject(value[added_words + 1 + i], word_bits, cache, cached_bits);
                                        
                                    }

                                    if(remaining_bits % word_bits){
                                        injector::inject(value[added_words + 1 + remaining_bits / word_bits + (remaining_bits % word_bits? 1 : 0)], remaining_bits % word_bits, cache, cached_bits);
                                    }

                                }

                            }

                        } else {
                            //If there are no bits in the cache
                            if (value_seen == block_bits) {
                                //The incoming value is a full block
                                construction.process_block(value, total_seen);
                            } else {
                                //The incoming value is not a full block
                                std::move(value.begin(), value.begin() + value_seen/word_bits + (value_seen%word_bits? 1 : 0), cache.begin());
                            }
                        }
                    }

                    inline void process(const word_type &value, std::size_t value_seen) {
                        using namespace ::nil::crypto3::detail;

                        std::size_t cached_bits = (total_seen - value_seen) % block_bits;

                        if (cached_bits != 0) {
                            std::size_t needed_to_fill_bits = block_bits - cached_bits;
                            std::size_t new_bits_to_append = (needed_to_fill_bits > value_seen)? value_seen : needed_to_fill_bits;

                            injector::inject(value, new_bits_to_append, cache, cached_bits);

                            if (cached_bits + new_bits_to_append == block_bits) {
                                //If there are enough bits in the incoming value to fill the block
                                construction.process_block(cache, total_seen - value_seen + new_bits_to_append);

                                if (value_seen > new_bits_to_append){
                                    //If there are some remaining bits in the incoming value - put them into the cache, which is now empty
                                    cached_bits = 0;

                                    word_type word_shifted = value;

                                    endian_shift<endian_type, word_bits>::to_msb(word_shifted, new_bits_to_append);

                                    injector::inject(word_shifted, value_seen - new_bits_to_append, cache, cached_bits);
                                }

                            }

                        } else {
                            cache[0] = value;
                        }
                    }

                    std::size_t total_seen;
                    block_type cache;
                    construction_type construction;
                };
            }    // namespace impl

            namespace tag {
                template<typename Hash>
                struct hash : boost::accumulators::depends_on<bits_count> {
                    typedef Hash hash_type;

                    /// INTERNAL ONLY
                    ///

                    typedef boost::mpl::always<accumulators::impl::hash_impl<Hash>> impl;
                };
            }    // namespace tag

            namespace extract {
                template<typename Hash, typename AccumulatorSet>
                typename boost::mpl::apply<AccumulatorSet, tag::hash<Hash>>::type::result_type
                    hash(const AccumulatorSet &acc) {
                    return boost::accumulators::extract_result<tag::hash<Hash>>(acc);
                }
            }    // namespace extract
        }        // namespace accumulators
    }            // namespace crypto3
}    // namespace nil

#endif    // CRYPTO3_ACCUMULATORS_BLOCK_HPP