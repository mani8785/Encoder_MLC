/*
Author: Hosma
Company: DTU QPIT
Title: Reverse reconciliation, MAP decoder, Sign-Magnitude
Details: load real data and test the decoder.
*/
#include <iostream>
#include <fstream> // header to read file
#include <itpp/itcomm.h>
#include <itpp/srccode/vq.h>
#include <itpp/comm/ldpc.h>
#include <iomanip>
#include <itpp/comm/modulator.h>
#include "mlc_msd.h"

// ================================================ Main function
int main(int argc, char **argv)
{
    // ------------------------------  load codec and  create LDPC decoder
    string codec_name1 = "R0.50_n512000.it"; // "R0.50_n512000.it";
    string codec_name2 = "R0.05_n512000.peg";  // "R0.50_n512000.it";
    string codec_name3 = "R0.05_n512000.peg";

    // string codec_name1 = "skip";
    // string codec_name2 = "R0.50_n384000.it";
    // string codec_name3 = "R0.02_n384000.peg";

    // string codec_name1 = "R0.88_n1e6.it"; // "R0.92_n1e6.it";  skip
    // string codec_name2 = "R0.88_n1e6.it"; // R0.50_n1e6.it
    // string codec_name3 = "R0.88_n1e6.it";
    // ------------------------------ Load continous data
    string data_in = "I_AB_-10dB.it";
    // =============================== Construct the MLC-MSD Environment
    MLCMSD mlc_env;
    mlc_env.set_num_level_in_use(1); // two levels in use
    // mlc_env.set_common_frame_length(384000); // 1e6
    mlc_env.set_common_frame_length(512000); // 1e6 512000

    vec XAI, XBI;
    int leng;
    mlc_env.load_test_data(data_in, &leng, &XAI, &XBI);
    // ------------------------------ ADC parameters
    QUANTIZER_INFO info_adc;    // struct
    Scalar_Quantizer Quantizer; // quantizer constructor
    Quantizer.set_levels(info_adc.levels);
    // ------------------------------ Level 1
    LDPC_Code ldpc1;                        // empty constructor
    ldpc1 = mlc_env.fill_ldpc(codec_name1); // argv[1])
    int max_iter_LDPC1 = 50;                 // maximum number of iteration for the LDPC decoder.
    int ittt;                               // actual number of iterations decoder uses.
                                            // ittt <= max_iter_ldpc
    ldpc1.set_exit_conditions(max_iter_LDPC1, false, false);
    LEVEL_INFO info_l1;
    mlc_env.initialize_struct(&info_l1, &ldpc1, 1, leng, codec_name1);
    // ------------------------------ Level 2
    LDPC_Code ldpc2;                        // empty constructor
    ldpc2 = mlc_env.fill_ldpc(codec_name2); // argv[1])
    int max_iter_LDPC2 = 50; 
    ldpc2.set_exit_conditions(max_iter_LDPC2, false, false);
    LEVEL_INFO info_l2;
    mlc_env.initialize_struct(&info_l2, &ldpc2, 2, leng, codec_name2);
    // ------------------------------ Level 3
    LDPC_Code ldpc3;                        // empty constructor
    ldpc3 = mlc_env.fill_ldpc(codec_name3); // argv[1])
    ldpc3.set_exit_conditions(max_iter_LDPC2, false, false);
    LEVEL_INFO info_l3;
    mlc_env.initialize_struct(&info_l3, &ldpc3, 3, leng, codec_name3);
    // ================================== Compatibility test
    mlc_env.update_level_info(&info_l1, &ldpc1, 1, leng, codec_name1);
    mlc_env.update_level_info(&info_l2, &ldpc2, 2, leng, codec_name2);
    mlc_env.update_level_info(&info_l3, &ldpc3, 3, leng, codec_name3);
    mlc_env.check_structure(&info_l1, &info_l2, &info_l3);
    // ================================== Define new wariables
    // Vectors
    vec x_A, x_B;
    ivec qxA, qxB;
    x_A.set_length(info_l1.fl, false);
    x_B.set_length(info_l1.fl, false);
    // x_A : Alice's continous data
    // x_B : Bob's continous data
    // qxA      : the quantized version of Alice's data
    // qxB      : the quantized version of Bob's data
    // Matrices
    // Stores the binary equivalent of the qxA and qxB left-msb format
    bmat qxA_bin, qxB_bin;
    qxA_bin.set_size(info_l1.fl, info_adc.m, false);
    qxB_bin.set_size(info_l1.fl, info_adc.m, false);
    // qxA_bin    : Binary equivalent of the quantized version of Alice's data
    // qxB_bin    : Binary equivalent of the quantized version of Bob's data
    mat x_B_mat, x_A_mat;   // to store the data
    ivec Iter_number_frame; // iteration for each frame
    int number_of_frames = leng / info_l1.fl + 1;
    x_B_mat.set_size(number_of_frames, info_l1.fl, false); // All the data for all frames
    x_A_mat.set_size(number_of_frames, info_l1.fl, false);
    Iter_number_frame.set_length(number_of_frames, false);
    int ifc = 0;
    // Boolean
    // =========== Binary hard vectors boolian 0 or 1
    bvec bin_b_level_1, bin_b_level_2, bin_b_level_3;

    double N0, varA, varB;
    double snr_lin, snr_dB;
    /*
  N0   : Noise variance,
   varA : Alice's variance of continous normal RV, varB : Alice's variance of continous normal RV
  */
    // ==============================  // ==============================  // Initialize
    //Randomize the random number generator
    RNG_randomize();

    //======================================================================================================
    printf("# ------------------------------------------------------\n");
    printf("# -------------- %8s", "MLC-MSD RECONCILIATION\n");
    printf("# ------------------------------------------------------\n");
    mlc_env.display_quant(&info_adc);
    if (mlc_env.get_num_level_in_use() == 1)
    {
        cout << "# * number of levels in use are: \t" << 1 << endl;
        mlc_env.display_level(&info_l1, true); // short info list is true
    }
    else if (mlc_env.get_num_level_in_use() == 2)
    {
        cout << "# * number of levels in use are: \t" << 2 << endl;
        mlc_env.display_level(&info_l1, true); // short info list is true
        mlc_env.display_level(&info_l2, true);
    }
    else
    {
        cout << "# * number of levels in use are: \t" << 3 << endl;
        mlc_env.display_level(&info_l1, true); // short info list is true
        mlc_env.display_level(&info_l2, true);
        mlc_env.display_level(&info_l3, true);
    }
    

    BERC berc1;  // The Bit Error Rate Counter class- Counters for coded and uncoded BER
    BLERC ferc1; // Counter for coded FER
    BERC berc2;  // The Bit Error Rate Counter class- Counters for coded and uncoded BER
    BLERC ferc2; // Counter for coded FER
    BERC berc3;  // The Bit Error Rate Counter class- Counters for coded and uncoded BER
    BLERC ferc3; // Counter for coded FER
    ferc3.set_blocksize(info_l3.fl);
    ferc2.set_blocksize(info_l2.fl);
    ferc1.set_blocksize(info_l1.fl);
    mlc_env.display_table_title();
    for (int64_t i = 0; i < leng; i += info_l1.fl) // for each frame
    {
        // ------------------------ Zero filling
        for (int cc = 0; cc < info_l1.fl; cc++) //
        {
            if (i + cc < leng)
            {
                x_B(cc) = XBI(i + cc);
            }
            else
            {
                x_B(cc) = 0;
            }
        }
        // ------------------------ Store data for later use
        x_B_mat.set_row(ifc, x_B); // store x_B

        // ------------------------ Normalization
        varB = variance(x_B);
        x_B /= sqrt(varB); // Normalization
        // ------------------------ Digitization
        qxB = Quantizer.encode(x_B); // quantizer
        for (int cc = 0; cc < info_l1.fl; cc++)
        {
            qxB_bin.set_row(cc, dec2bin(info_adc.m, qxB(cc))); // left-msb representation
        }
        bin_b_level_1 = qxB_bin.get_col(0); // just for test
        bin_b_level_2 = qxB_bin.get_col(1);
        bin_b_level_3 = qxB_bin.get_col(2);
        
        if (mlc_env.get_num_level_in_use() == 1)
        {
            // ----------------------- MLC-MSD one level
            bvec encded_data;
            encded_data.set_length(info_l1.pl, false);
            bmat plain_texts;
            plain_texts.set_size(info_l1.fl, info_adc.m - 1, false);
            mlc_env.encoder_one_level(&qxB_bin, &info_l1, &info_adc, &plain_texts, &encded_data);
        }
        else if (mlc_env.get_num_level_in_use() == 2)
        {
            // ----------------------- MLC-MSD two levels
            bvec enc_data_1, enc_data_2;
            enc_data_1.set_length(info_l1.pl, false);
            enc_data_2.set_length(info_l2.pl, false);
            bmat plain_texts_new;
            plain_texts_new.set_size(info_l1.fl, info_adc.m - 2, false);
            mlc_env.encoder_two_levels(&qxB_bin, &info_l1, &info_l2, &info_adc, &plain_texts_new, &enc_data_1, &enc_data_2);
        }
        else if (mlc_env.get_num_level_in_use() == 3)
        {
            // ----------------------- MLC-MSD three levels
            bvec enc_data_1, enc_data_2, enc_data_3;
            enc_data_1.set_length(info_l1.pl, false);
            enc_data_2.set_length(info_l2.pl, false);
            enc_data_3.set_length(info_l3.pl, false);
            bmat plain_texts_2_to_0;
            plain_texts_2_to_0.set_size(info_l1.fl, info_adc.m - 3, false);
            mlc_env.encoder_three_levels(&qxB_bin, &info_l1, &info_l2, &info_l3, &info_adc, &plain_texts_2_to_0, &enc_data_1, &enc_data_2, &enc_data_3);
        }
        else
        {
            it_error("We do not support more than 3 levels");
        }
        

    } // end for frames i

    printf("\n# END of Simulation\n");
    return 0;
}

// ======================================================================== other methods

/*
    Encoder functions:
        I generated two new functions using it++ lirary.
        Encoder is actually  a matrix multiplication.
    Input :
        a vector of length n_var
    Output:
        syndrome : a binary vector of length n_check
    Task:
        For a given matrix H and input llr_in
        Step 1: hard_in = llr_in < 0;
        Step 2: synd = hard_in * H.T

*/

bool LDPC_Code::syndrome_check_hosma(const bvec &x, const bvec &synd_orig) const
{
    QLLRvec llr = 1 - 2 * to_ivec(x);
    return syndrome_check_hosma(llr, synd_orig);
};


bool LDPC_Code::syndrome_check_hosma(const QLLRvec &LLR, const bvec &synd_orig) const
{
    // Please note the IT++ convention that a sure zero corresponds to
    // LLR=+infinity
    int i, j, synd, vi;

    for (j = 0; j < ncheck; j++)
    {
        synd = synd_orig(j);
        int vind = j; // tracks j+i*ncheck
        for (i = 0; i < sumX2(j); i++)
        {
            vi = V(vind);
            if (LLR(vi) < 0)
            {
                synd++;
            }
            vind += ncheck;
        }
        if ((synd & 1) == 1)
        {
            return false; // codeword is invalid
        }
    }
    return true; // codeword is valid
};