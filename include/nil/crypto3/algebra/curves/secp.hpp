//---------------------------------------------------------------------------//
// Copyright (c) 2020-2021 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2020-2021 Nikita Kaskov <nbering@nil.foundation>
//
// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_ALGEBRA_CURVES_SECP_HPP
#define CRYPTO3_ALGEBRA_CURVES_SECP_HPP

#include <nil/crypto3/algebra/curves/detail/secp/secp_k1/basic_policy.hpp>
#include <nil/crypto3/algebra/curves/detail/secp/secp_r1/basic_policy.hpp>

#include <nil/crypto3/algebra/curves/detail/secp/secp_k1/g1.hpp>
#include <nil/crypto3/algebra/curves/detail/secp/secp_r1/g1.hpp>

#include <nil/crypto3/detail/literals.hpp>

namespace nil {
    namespace crypto3 {
        namespace algebra {
            namespace curves {
                template<std::size_t Version>
                class secp_k1 {
                    using policy_type = detail::secp_k1_basic_policy<Version>;

                public:
                    typedef typename policy_type::base_field_type base_field_type;
                    typedef typename policy_type::scalar_field_type scalar_field_type;
                    typedef typename policy_type::number_type number_type;
                    typedef typename policy_type::extended_number_type extended_number_type;

                    constexpr static const number_type p = policy_type::p;    ///< base field characteristic
                    constexpr static const number_type q =
                        policy_type::q;    ///< scalar field characteristic (order of the group of points)

                    typedef typename detail::secp_k1_g1<Version> g1_type;
                };

                template<std::size_t Version>
                class secp_r1 {
                    using policy_type = detail::secp_r1_basic_policy<Version>;

                public:
                    typedef typename policy_type::base_field_type base_field_type;
                    typedef typename policy_type::scalar_field_type scalar_field_type;
                    typedef typename policy_type::number_type number_type;
                    typedef typename policy_type::extended_number_type extended_number_type;

                    constexpr static const number_type p = policy_type::p;    ///< base field characteristic
                    constexpr static const number_type q =
                        policy_type::q;    ///< scalar field characteristic (order of the group of points)

                    typedef typename detail::secp_r1_g1<Version> g1_type;
                };

                template<std::size_t Version>
                class secp_r2;

                //                typedef secp_k1<160> secp160k1;
                //                typedef secp_r1<160> secp160r1;
                //                typedef secp_r2<160> secp160r2;
                //                typedef secp_k1<192> secp192k1;
                //                typedef secp_r1<192> secp192r1;
                //                typedef secp_k1<224> secp224k1;
                //                typedef secp_r1<224> secp224r1;
                typedef secp_k1<256> secp256k1;
                typedef secp_r1<256> secp256r1;
            }    // namespace curves
        }        // namespace algebra
    }            // namespace crypto3
}    // namespace nil

#endif    // CRYPTO3_ALGEBRA_CURVES_SECP_HPP
