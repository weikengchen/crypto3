//---------------------------------------------------------------------------//
// Copyright (c) 2020 Ilias Khairullin <ilias@nil.foundation>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_HASH_POSEIDON_POLICY_HPP
#define CRYPTO3_HASH_POSEIDON_POLICY_HPP

#include <nil/crypto3/detail/static_digest.hpp>

#include <boost/static_assert.hpp>
#include <boost/assert.hpp>

#include <array>
#include <utility>

namespace nil {
    namespace crypto3 {
        namespace hashes {
            namespace detail {

                /*!
                * @brief Poseidon internal parameters
                * @tparam FieldT
                * @tparam t arity of Poseidon permutation in Field elements
                * @tparam c capacity of sponge construction
                * @tparam DigestBits
                * @tparam M desired security level in bits
                * @tparam strength 
                */
                // at this moment only for bls12-381 - filecoin oriented implementation
                template<
                    typename FieldT,
                    std::size_t t,
                    std::size_t c,
                    std::size_t DigestBits,
                    std::size_t M = 128,
                    bool strength = true
                >
                struct poseidon_policy {

                    BOOST_STATIC_ASSERT_MSG(t > c, "t should consist of capacity and rate");

                    constexpr static std::size_t const digest_bits = DigestBits;

                    constexpr static std::size_t const state_bits = t * FieldT::modulus_bits;
                    constexpr static std::size_t const state_words = t;
                    typedef std::array<FieldT, state_words> state_type;

                    constexpr static std::size_t const block_bits = (t - c) * FieldT::modulus_bits;
                    constexpr static std::size_t const block_words = t - c;
                    typedef std::array<FieldT, block_words> block_type;

                    constexpr static std::size_t const sec_level = M;

                    struct iv_generator {
                        // TODO: return-value seems not to be const in reality
                        // TODO: maybe it would be done in constexpr way
                        state_type const &operator()() const {
                            static state_type const H0 = [](){
                                state_type H;
                                H.fill(FieldT(0));
                                return std::move(H);
                            }();
                            return H0;
                        }
                    };

                };      // struct poseidon_policy
            }       // detail
        }       // hashes
    }       // crypto3
}       // nil

#endif      // CRYPTO3_HASH_POSEIDON_POLICY_HPP
