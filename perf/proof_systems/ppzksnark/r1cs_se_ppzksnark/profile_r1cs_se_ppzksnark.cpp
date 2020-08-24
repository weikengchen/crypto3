//---------------------------------------------------------------------------//
// Copyright (c) 2018-2020 Mikhail Komarov <nemo@nil.foundation>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#include <cassert>
#include <cstdio>

#include <nil/algebra/common/profiling.hpp>
#include <nil/algebra/common/utils.hpp>

#include <nil/crypto3/zk/snark/default_types/r1cs_se_ppzksnark_pp.hpp>
#include <nil/crypto3/zk/snark/relations/constraint_satisfaction_problems/r1cs/examples/r1cs_examples.hpp>
#include <nil/crypto3/zk/snark/proof_systems/ppzksnark/r1cs_se_ppzksnark/examples/run_r1cs_se_ppzksnark.hpp>

using namespace nil::crypto3::zk::snark;

int main(int argc, const char *argv[]) {
    if (argc == 2 && strcmp(argv[1], "-v") == 0) {
        algebra::print_compilation_info();
        return 0;
    }

    if (argc != 3 && argc != 4) {
        printf("usage: %s num_constraints input_size [Fr|bytes]\n", argv[0]);
        return 1;
    }
    const int num_constraints = atoi(argv[1]);
    int input_size = atoi(argv[2]);
    if (argc == 4) {
        assert(strcmp(argv[3], "Fr") == 0 || strcmp(argv[3], "bytes") == 0);
        if (strcmp(argv[3], "bytes") == 0) {
            input_size = (8 * input_size + (typename algebra::default_ec_pp::scalar_field_type::capacity()) - 1) /
                         typename algebra::default_ec_pp::scalar_field_type::capacity();
        }
    }

    algebra::enter_block("Generate R1CS example");
    r1cs_example<typename default_r1cs_se_ppzksnark_pp::scalar_field_type> example =
        generate_r1cs_example_with_field_input<typename default_r1cs_se_ppzksnark_pp::scalar_field_type>(num_constraints, input_size);
    algebra::leave_block("Generate R1CS example");

    algebra::print_header("(enter) Profile R1CS SEppzkSNARK");
    const bool test_serialization = true;
    run_r1cs_se_ppzksnark<default_r1cs_se_ppzksnark_pp>(example, test_serialization);
    algebra::print_header("(leave) Profile R1CS SEppzkSNARK");
}
