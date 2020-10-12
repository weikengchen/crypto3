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
// @file Test program that exercises the SEppzkSNARK (first generator, then
// prover, then verifier) on a synthetic R1CS instance.
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_ZK_SET_MEMBERSHIP_PROOF_VARIABLE_HPP
#define CRYPTO3_ZK_SET_MEMBERSHIP_PROOF_VARIABLE_HPP

#include <nil/crypto3/zk/snark/set_commitment.hpp>
#include <nil/crypto3/zk/snark/component.hpp>
#include <nil/crypto3/zk/snark/components/hashes/hash_io.hpp>
#include <nil/crypto3/zk/snark/components/merkle_tree/merkle_authentication_path_variable.hpp>

namespace nil {
    namespace crypto3 {
        namespace zk {
            namespace snark {

                template<typename FieldType, typename Hash>
                class set_membership_proof_variable : public component<FieldType> {
                public:
                    blueprint_variable_vector<FieldType> address_bits;
                    std::shared_ptr<merkle_authentication_path_variable<FieldType, Hash>> merkle_path;

                    const std::size_t max_entries;
                    const std::size_t tree_depth;

                    set_membership_proof_variable(blueprint<FieldType> &pb, const std::size_t max_entries) :
                                                            component<FieldType>(pb), max_entries(max_entries), 
                                                            tree_depth(static_cast<std::size_t>(std::ceil(std::log2(max_entries)))) 
                    {
                        if (tree_depth > 0) {
                            address_bits.allocate(pb, tree_depth);
                            merkle_path.reset(new merkle_authentication_path_variable<FieldType, Hash>(pb, tree_depth));
                        }
                    }

                    void generate_r1cs_constraints() {
                        if (tree_depth > 0) {
                            for (std::size_t i = 0; i < tree_depth; ++i) {
                                generate_boolean_r1cs_constraint<FieldType>(this->pb, address_bits[i]);
                            }
                            merkle_path->generate_r1cs_constraints();
                        }
                    }
                    void generate_r1cs_witness(const set_membership_proof &proof) {
                        if (tree_depth > 0) {
                            address_bits.fill_with_bits_of_field_element(this->pb, typename FieldType::value_type(proof.address));
                            merkle_path->generate_r1cs_witness(proof.address, proof.merkle_path);
                        }
                    }

                    set_membership_proof get_membership_proof() const {
                        set_membership_proof result;

                        if (tree_depth == 0) {
                            result.address = 0;
                        } else {
                            result.address = address_bits.get_field_element_from_bits(this->pb).as_ulong();
                            result.merkle_path = merkle_path->get_authentication_path(result.address);
                        }

                        return result;
                    }

                    static r1cs_variable_assignment<FieldType>
                        as_r1cs_variable_assignment(const set_membership_proof &proof) {

                        blueprint<FieldType> pb;
                        const std::size_t max_entries = (1ul << (proof.merkle_path.size()));
                        set_membership_proof_variable<FieldType, Hash> proof_variable(pb, max_entries, "proof_variable");
                        proof_variable.generate_r1cs_witness(proof);

                        return pb.full_variable_assignment();
                    }
                };

            }    // namespace snark
        }        // namespace zk
    }            // namespace crypto3
}    // namespace nil

#endif    // CRYPTO3_ZK_SET_MEMBERSHIP_PROOF_VARIABLE_HPP
