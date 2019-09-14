//---------------------------------------------------------------------------//
// Copyright (c) 2018-2019 Nil Foundation AG
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nil.foundation>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_HASH_DETAIL_RIPEMD_POLICY_HPP
#define CRYPTO3_HASH_DETAIL_RIPEMD_POLICY_HPP

#include <array>

#include <nil/crypto3/detail/static_digest.hpp>

#include <nil/crypto3/hash/detail/ripemd/ripemd_functions.hpp>

namespace nil {
    namespace crypto3 {
        namespace hash {
            namespace detail {
                template<std::size_t DigestBits>
                struct basic_ripemd_policy : public ripemd_functions<32> {
                    constexpr static const std::size_t word_bits = ripemd_functions<32>::word_bits;
                    typedef ripemd_functions<32>::word_type word_type;

                    constexpr static const std::size_t block_bits = 512;
                    constexpr static const std::size_t block_words = block_bits / word_bits;
                    typedef std::array<word_type, block_words> block_type;

                    constexpr static const std::size_t digest_bits = DigestBits;
                    typedef static_digest<digest_bits> digest_type;

                    constexpr static const std::uint8_t ieee1363_hash_id = 0x33;
                    typedef std::array<std::uint8_t, 15> pkcs_id_type;

                    constexpr static const pkcs_id_type pkcs_id = {0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2B, 0x0E,
                                                                   0x03, 0x02, 0x1A, 0x05, 0x00, 0x04, 0x14};

                    constexpr static const std::size_t key_indexes_size = 80;
                    typedef std::array<byte_type, key_indexes_size> key_indexes_type;

                    // offsets into X array
                    constexpr static const key_indexes_type r1 = {
                        0,  1, 2,  3, 4,  5,  6, 7,  8, 9,  10, 11, 12, 13, 14, 15, 7,  4,  13, 1,
                        10, 6, 15, 3, 12, 0,  9, 5,  2, 14, 11, 8,  3,  10, 14, 4,  9,  15, 8,  1,
                        2,  7, 0,  6, 13, 11, 5, 12, 1, 9,  11, 10, 0,  8,  12, 4,  13, 3,  7,  15,
                        14, 5, 6,  2, 4,  0,  5, 9,  7, 12, 2,  10, 14, 1,  3,  8,  11, 6,  15, 13};

                    constexpr static const key_indexes_type r2 = {
                        5,  14, 7,  0,  9,  2,  11, 4,  13, 6, 15, 8, 1,  10, 3,  12, 6, 11, 3, 7,
                        0,  13, 5,  10, 14, 15, 8,  12, 4,  9, 1,  2, 15, 5,  1,  3,  7, 14, 6, 9,
                        11, 8,  12, 2,  10, 0,  4,  13, 8,  6, 4,  1, 3,  11, 15, 0,  5, 12, 2, 13,
                        9,  7,  10, 14, 12, 15, 10, 4,  1,  5, 8,  7, 6,  2,  13, 14, 0, 3,  9, 11};

                    // values for rotate left
                    constexpr static const key_indexes_type s1 = {
                        11, 14, 15, 12, 5, 8,  7,  9,  11, 13, 14, 15, 6,  7,  9,  8,  7,  6,  8,  13,
                        11, 9,  7,  15, 7, 12, 15, 9,  11, 7,  13, 12, 11, 13, 6,  7,  14, 9,  13, 15,
                        14, 8,  13, 6,  5, 12, 7,  5,  11, 12, 14, 15, 14, 15, 9,  8,  9,  14, 5,  6,
                        8,  6,  5,  12, 9, 15, 5,  11, 6,  8,  13, 12, 5,  12, 13, 14, 11, 8,  5,  6};

                    constexpr static const key_indexes_type s2 = {
                        8,  9,  9,  11, 13, 15, 15, 5, 7,  7,  8,  11, 14, 14, 12, 6,  9,  13, 15, 7,
                        12, 8,  9,  11, 7,  7,  12, 7, 6,  15, 13, 11, 9,  7,  15, 11, 8,  6,  6,  14,
                        12, 13, 5,  14, 13, 13, 7,  5, 15, 5,  8,  11, 14, 14, 6,  14, 6,  9,  12, 9,
                        12, 5,  15, 8,  8,  5,  12, 9, 12, 5,  14, 6,  8,  13, 6,  5,  15, 13, 11, 11};
                };

                template<std::size_t DigestSize>
                constexpr typename basic_ripemd_policy<DigestSize>::key_indexes_type const
                    basic_ripemd_policy<DigestSize>::r1;

                template<std::size_t DigestSize>
                constexpr typename basic_ripemd_policy<DigestSize>::key_indexes_type const
                    basic_ripemd_policy<DigestSize>::r2;

                template<std::size_t DigestSize>
                constexpr typename basic_ripemd_policy<DigestSize>::key_indexes_type const
                    basic_ripemd_policy<DigestSize>::s1;

                template<std::size_t DigestSize>
                constexpr typename basic_ripemd_policy<DigestSize>::key_indexes_type const
                    basic_ripemd_policy<DigestSize>::s2;

                template<std::size_t DigestBits>
                struct ripemd_policy : public basic_ripemd_policy<DigestBits> {};

                template<>
                struct ripemd_policy<128> : public basic_ripemd_policy<128> {
                    constexpr static const std::size_t word_bits = basic_ripemd_policy<128>::word_bits;
                    typedef basic_ripemd_policy<128>::word_type word_type;

                    constexpr static const std::size_t state_words = 4;
                    constexpr static const std::size_t state_bits = state_words * word_bits;
                    typedef std::array<word_type, state_words> state_type;

                    struct iv_generator {
                        state_type const &operator()() const {
                            constexpr static const state_type H0 = {{0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476}};
                            return H0;
                        }
                    };
                };

                template<>
                struct ripemd_policy<160> : public basic_ripemd_policy<160> {
                    constexpr static const std::size_t word_bits = basic_ripemd_policy<160>::word_bits;
                    typedef basic_ripemd_policy<160>::word_type word_type;

                    constexpr static const std::size_t state_words = 5;
                    constexpr static const std::size_t state_bits = state_words * word_bits;
                    typedef std::array<word_type, state_words> state_type;

                    struct iv_generator {
                        state_type const &operator()() const {
                            static state_type const H0 = {{0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0}};
                            return H0;
                        }
                    };
                };

                template<>
                struct ripemd_policy<256> : public basic_ripemd_policy<256> {
                    constexpr static const std::size_t word_bits = basic_ripemd_policy<256>::word_bits;
                    typedef basic_ripemd_policy<256>::word_type word_type;

                    constexpr static const std::size_t state_words = 8;
                    constexpr static const std::size_t state_bits = state_words * word_bits;
                    typedef std::array<word_type, state_words> state_type;

                    struct iv_generator {
                        state_type const &operator()() const {
                            static state_type const H0 = {{0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0x76543210,
                                                           0xfedcba98, 0x89abcdef, 0x01234567}};
                            return H0;
                        }
                    };
                };

                template<>
                struct ripemd_policy<320> : public basic_ripemd_policy<320> {
                    constexpr static const std::size_t word_bits = basic_ripemd_policy<320>::word_bits;
                    typedef basic_ripemd_policy<320>::word_type word_type;

                    constexpr static const std::size_t state_words = 10;
                    constexpr static const std::size_t state_bits = state_words * word_bits;
                    typedef std::array<word_type, state_words> state_type;

                    struct iv_generator {
                        state_type const &operator()() const {
                            static state_type const H0 = {{0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0,
                                                           0x76543210, 0xfedcba98, 0x89abcdef, 0x01234567, 0x3c2d1e0f}};
                            return H0;
                        }
                    };
                };
            }    // namespace detail
        }        // namespace hash
    }            // namespace crypto3
}    // namespace nil

#endif    // CRYPTO3_HASH_DETAIL_RIPEMD_POLICY_HPP
