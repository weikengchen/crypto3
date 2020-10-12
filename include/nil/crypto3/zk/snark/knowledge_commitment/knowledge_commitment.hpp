//---------------------------------------------------------------------------//
// Copyright (c) 2018-2020 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2020 Nikita Kaskov <nbering@nil.foundation>
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

#ifndef CRYPTO3_ZK_KNOWLEDGE_COMMITMENT_HPP_
#define CRYPTO3_ZK_KNOWLEDGE_COMMITMENT_HPP_

#include <nil/crypto3/algebra/fields/field.hpp>

#include <nil/crypto3/zk/snark/sparse_vector.hpp>

namespace nil {
    namespace crypto3 {
        namespace zk {
            namespace snark {

                /********************** Knowledge commitment *********************************/

                /**
                 * A knowledge commitment is a pair (g,h) where g is in T1 and h in T2,
                 * and T1 and T2 are groups (written additively).
                 *
                 * Such pairs form a group by defining:
                 * - "zero" = (0,0)
                 * - "one" = (1,1)
                 * - a * (g,h) + b * (g',h') := ( a * g + b * g', a * h + b * h').
                 */
                template<typename T1, typename T2>
                struct knowledge_commitment {

                    T1 g;
                    T2 h;

                    knowledge_commitment() = default;
                    knowledge_commitment(const knowledge_commitment &other) = default;
                    knowledge_commitment(knowledge_commitment &&other) = default;
                    knowledge_commitment(const T1 &g, const T2 &h) : g(g), h(h) {
                    }

                    knowledge_commitment &operator=(const knowledge_commitment &other) = default;
                    knowledge_commitment &operator=(knowledge_commitment &&other) noexcept = default;
                    knowledge_commitment operator+(const knowledge_commitment &other) const {
                        return knowledge_commitment(this->g + other.g, this->h + other.h);
                    }
                    knowledge_commitment mixed_add(const knowledge_commitment &other) const {
                        return knowledge_commitment(this->g.mixed_add(other.g), this->h.mixed_add(other.h));
                    }
                    knowledge_commitment doubled() const {
                        return knowledge_commitment(this->g.doubled(), this->h.doubled());
                    }

                    knowledge_commitment to_special() {
                        return knowledge_commitment(this->g.to_special(), this->h.to_special());
                    }
                    bool is_special() const {
                        return this->g->is_special() && this->h->is_special();
                    }

                    bool is_zero() const {
                        return (g.is_zero() && h.is_zero());
                    }
                    bool operator==(const knowledge_commitment &other) const {
                        return (this->g == other.g && this->h == other.h);
                    }
                    bool operator!=(const knowledge_commitment &other) const {
                        return !((*this) == other);
                    }

                    static knowledge_commitment zero() {
                        return knowledge_commitment(T1::zero(), T2::zero());
                    }
                    static knowledge_commitment one() {
                        return knowledge_commitment(T1::one(), T2::one());
                    }

                    static std::size_t size_in_bits() {
                        return T1::size_in_bits() + T2::size_in_bits();
                    }

                    static void batch_to_special_all_non_zeros(std::vector<knowledge_commitment> &vec) {
                        // it is guaranteed that every vec[i] is non-zero,
                        // but, for any i, *one* of vec[i].g and vec[i].h might still be zero,
                        // so we still have to handle zeros separately

                        // we separately process g's first, then h's
                        // to lower memory consumption
                        std::vector<T1> g_vec;
                        g_vec.reserve(vec.size());

                        for (std::size_t i = 0; i < vec.size(); ++i) {
                            if (!vec[i].g.is_zero()) {
                                g_vec.emplace_back(vec[i].g);
                            }
                        }

                        T1::batch_to_special_all_non_zeros(g_vec);
                        auto g_it = g_vec.begin();
                        T1 T1_zero_special = T1::zero().to_special();

                        for (std::size_t i = 0; i < vec.size(); ++i) {
                            if (!vec[i].g.is_zero()) {
                                vec[i].g = *g_it;
                                ++g_it;
                            } else {
                                vec[i].g = T1_zero_special;
                            }
                        }

                        g_vec.clear();

                        // exactly the same thing, but for h:
                        std::vector<T2> h_vec;
                        h_vec.reserve(vec.size());

                        for (std::size_t i = 0; i < vec.size(); ++i) {
                            if (!vec[i].h.is_zero()) {
                                h_vec.emplace_back(vec[i].h);
                            }
                        }

                        T2::batch_to_special_all_non_zeros(h_vec);
                        auto h_it = h_vec.begin();
                        T2 T2_zero_special = T2::zero().to_special();

                        for (std::size_t i = 0; i < vec.size(); ++i) {
                            if (!vec[i].h.is_zero()) {
                                vec[i].h = *h_it;
                                ++h_it;
                            } else {
                                vec[i].h = T2_zero_special;
                            }
                        }

                        h_vec.clear();
                    }
                };

                template<typename T1, typename T2, typename Backend,
                         boost::multiprecision::expression_template_option ExpressionTemplates>
                knowledge_commitment<T1, T2>
                    operator*(const boost::multiprecision::number<Backend, ExpressionTemplates> &lhs,
                              const knowledge_commitment<T1, T2> &rhs) {
                    return knowledge_commitment<T1, T2>(lhs * rhs.g, lhs * rhs.h);
                }

                template<typename T1, typename T2, typename FieldType>
                knowledge_commitment<T1, T2> operator*(const typename FieldType::value_type &lhs,
                                                       const knowledge_commitment<T1, T2> &rhs) {
                    return lhs * rhs;
                }

                /******************** Knowledge commitment vector ****************************/

                /**
                 * A knowledge commitment vector is a sparse vector of knowledge commitments.
                 */
                template<typename T1, typename T2>
                using knowledge_commitment_vector = sparse_vector<knowledge_commitment<T1, T2>>;

            }    // namespace snark
        }        // namespace zk
    }            // namespace crypto3
}    // namespace nil

#endif    // KNOWLEDGE_COMMITMENT_HPP_
