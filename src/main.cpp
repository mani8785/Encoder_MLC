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
                x_A(cc) = XAI(i + cc);
                x_B(cc) = XBI(i + cc);
            }
            else
            {
                x_A(cc) = 0;
                x_B(cc) = 0;
            }
        }
        // ------------------------ Store data for later use
        x_A_mat.set_row(ifc, x_A); // store X_A
        x_B_mat.set_row(ifc, x_B); // store x_B

        // ------------------------ SNR estimation
        double sss = mlc_env.calc_snr_numerical(&x_A, &x_B);
        double sigma2 = pow(sss, -1);
        N0 = 2 * sigma2;
        // N0 = 2 * (1.889822 * 1.889822); //  abs(varB - varA);
        snr_lin = pow(N0 / 2, -1);
        snr_dB = 10 * log10(snr_lin);
        // ------------------------ Normalization
        varA = variance(x_A);
        x_A /= sqrt(varA); // Normalization
        varB = variance(x_B);
        x_B /= sqrt(varB); // Normalization
        // ------------------------ Digitization
        qxA = Quantizer.encode(x_A); // quantizer
        qxB = Quantizer.encode(x_B); // quantizer
        for (int cc = 0; cc < info_l1.fl; cc++)
        {
            qxA_bin.set_row(cc, dec2bin(info_adc.m, qxA(cc))); // left-msb representation
            qxB_bin.set_row(cc, dec2bin(info_adc.m, qxB(cc))); // left-msb representation
        }
        bin_b_level_1 = qxB_bin.get_col(0); // just for test
        bin_b_level_2 = qxB_bin.get_col(1);
        bin_b_level_3 = qxB_bin.get_col(2);
        
        if (mlc_env.get_num_level_in_use() == 1)
        {
            // ----------------------- MLC-MSD one level
            bvec encded_data, decoded_data;
            encded_data.set_length(info_l1.pl, false);
            decoded_data.set_length(info_l1.fl, false);
            bmat plain_texts;
            plain_texts.set_size(info_l1.fl, info_adc.m - 1, false);
            mlc_env.encoder_one_level(&qxB_bin, &info_l1, &info_adc, &plain_texts, &encded_data);
            mlc_env.decoder_one_level(&qxA, &qxA_bin, &plain_texts, &encded_data, N0, 1, &info_l1, &info_adc, &decoded_data);
            berc1.count(bin_b_level_1, decoded_data);
            ferc1.count(bin_b_level_1, decoded_data);
            mlc_env.display_efficiency(&berc1, &ferc1, snr_lin, N0, &info_l1, &ldpc1, 1);
        }
        else if (mlc_env.get_num_level_in_use() == 2)
        {
            // ----------------------- MLC-MSD two levels
            bvec enc_data_1, enc_data_2, dec_data_1, dec_data_2;
            enc_data_1.set_length(info_l1.pl, false);
            dec_data_1.set_length(info_l1.fl, false);
            enc_data_2.set_length(info_l2.pl, false);
            dec_data_2.set_length(info_l2.fl, false);
            bmat plain_texts_new;
            plain_texts_new.set_size(info_l1.fl, info_adc.m - 2, false);
            mlc_env.encoder_two_levels(&qxB_bin, &info_l1, &info_l2, &info_adc, &plain_texts_new, &enc_data_1, &enc_data_2);
            // mlc_env.decoder_two_levels(&qxA, &qxA_bin, &plain_texts_new, &enc_data_1, &enc_data_2, N0, &info_l1, &info_l2, &info_adc, &dec_data_1, &dec_data_2);
            mlc_env.decoder_two_levels_new(&qxA, &qxA_bin, &plain_texts_new, &enc_data_1, &enc_data_2, N0, &info_l1, &info_l2, &info_adc, &dec_data_1, &dec_data_2);
            berc1.count(bin_b_level_1, dec_data_1);
            ferc1.count(bin_b_level_1, dec_data_1);
            berc2.count(bin_b_level_2, dec_data_2);
            ferc2.count(bin_b_level_2, dec_data_2);
            mlc_env.display_efficiency(&berc1, &ferc1, snr_lin, N0, &info_l1, &ldpc1, 1);
            mlc_env.display_efficiency(&berc2, &ferc2, snr_lin, N0, &info_l2, &ldpc2, 2);
        }
        else if (mlc_env.get_num_level_in_use() == 3)
        {
            // ----------------------- MLC-MSD three levels
            bvec enc_data_1, enc_data_2, enc_data_3, dec_data_1, dec_data_2, dec_data_3;
            enc_data_1.set_length(info_l1.pl, false);
            dec_data_1.set_length(info_l1.fl, false);
            enc_data_2.set_length(info_l2.pl, false);
            dec_data_2.set_length(info_l2.fl, false);
            enc_data_3.set_length(info_l3.pl, false);
            dec_data_3.set_length(info_l3.fl, false);
            bmat plain_texts_2_to_0;
            plain_texts_2_to_0.set_size(info_l1.fl, info_adc.m - 3, false);
            mlc_env.encoder_three_levels(&qxB_bin, &info_l1, &info_l2, &info_l3, &info_adc, &plain_texts_2_to_0, &enc_data_1, &enc_data_2, &enc_data_3);
            mlc_env.decoder_three_levels(&qxA, &qxA_bin, &plain_texts_2_to_0, &enc_data_1, &enc_data_2, &enc_data_3, N0, &info_l1, &info_l2, &info_l3, &info_adc, &dec_data_1, &dec_data_2, &dec_data_3);
            berc1.count(bin_b_level_1, dec_data_1);
            ferc1.count(bin_b_level_1, dec_data_1);
            berc2.count(bin_b_level_2, dec_data_2);
            ferc2.count(bin_b_level_2, dec_data_2);
            berc3.count(bin_b_level_3, dec_data_3);
            ferc3.count(bin_b_level_3, dec_data_3);
            mlc_env.display_efficiency(&berc1, &ferc1, snr_lin, N0, &info_l1, &ldpc1, 1);
            mlc_env.display_efficiency(&berc2, &ferc2, snr_lin, N0, &info_l2, &ldpc2, 2);
            mlc_env.display_efficiency(&berc3, &ferc3, snr_lin, N0, &info_l3, &ldpc3, 3);
        }
        else
        {
            it_error("We do not support more than 3 levels");
        }
        

    } // end for frames i

    printf("\n# END of Simulation\n");

    //   cout << "# The number of iterations " << Iter_number_frame << endl;
    //   it_file my_file("real_data.it");
    //   my_file << Name("I_x_A") << x_A_mat;
    //   my_file << Name("I_x_B") << x_B_mat;
    //   my_file << Name("Iter") << Iter_number_frame;

    return 0;
}

// ======================================================================== other methods

// ================================================================================================= DECODER WITH SYNDROME

int LDPC_Code::bp_decode_s(const QLLRvec &LLRin, const bvec &LLRsynd_hard, QLLRvec &LLRout)
{
    // Note the IT++ convention that a sure zero corresponds to
    // LLR=+infinity
    it_assert(H_defined, "LDPC_Code::bp_decode(): Parity check matrix not "
                         "defined");
    it_assert((LLRin.size() == nvar) && (sumX1.size() == nvar) && (sumX2.size() == ncheck), "LDPC_Code::bp_decode(): Wrong "
                                                                                            "input dimensions");

    // if (pisc && syndrome_check(LLRin)) {
    //   LLRout = LLRin;
    //   return 0;
    // }

    //My own syndrome check
    //   QLLRvec LLRsynd = QLLR_MAX*LLRsynd1;
    QLLRvec llrsynd_a;
    bvec s_bin_a, s_bin_b;
    int e_cnt = 0;

    bvec keya_bin = LLRin > 0;
    ivec sign_a = 2 * to_ivec(keya_bin) - 1;
    llrsynd_a = soft_syndrome_check(sign_a);
    s_bin_a = llrsynd_a > 0;
    s_bin_b = LLRsynd_hard;
    e_cnt = 0;
    for (int i = 0; i < LLRsynd_hard.length(); ++i)
    {
        if ((s_bin_a(i) + s_bin_b(i)))
            e_cnt++;
    }

    if (e_cnt < 1)
    {
        LLRout = LLRin;
        return 0;
    }

    LLRout.set_size(LLRin.size());

    // allocate temporary variables used for the check node update
    ivec jj(max_cnd);
    QLLRvec m(max_cnd);
    QLLRvec ml(max_cnd);
    QLLRvec mr(max_cnd);

    // initial step
    for (int i = 0; i < nvar; i++)
    {
        int index = i;
        for (int j = 0; j < sumX1(i); j++)
        {
            mvc[index] = LLRin(i);
            index += nvar;
        }
    }

    bool is_valid_codeword = false;
    int iter = 0;
    do
    {
        iter++;
        //     it_info_no_endl_debug(".");
        //     if (nvar >= 100000) { it_info_no_endl_debug("."); }
        // --------- Step 1: check to variable nodes ----------
        for (int j = 0; j < ncheck; j++)
        {
            // The check node update calculations are hardcoded for degrees
            // up to 6.  For larger degrees, a general algorithm is used.
            switch (sumX2(j))
            {
            case 0:
                it_error("LDPC_Code::bp_decode(): sumX2(j)=0");
            case 1:
                it_error("LDPC_Code::bp_decode(): sumX2(j)=1");
            case 2:
            {
                mcv[j + ncheck] = mvc[jind[j]];
                mcv[j] = mvc[jind[j + ncheck]];

                // Hosma new changes
                if (LLRsynd_hard(j)) // (LLRsynd(j) > 0)
                {
                    mcv[j] *= -1;
                    mcv[j + ncheck] *= -1;
                }

                break;
            }
            case 3:
            {
                int j0 = j;
                QLLR m0 = mvc[jind[j0]];
                int j1 = j0 + ncheck;
                QLLR m1 = mvc[jind[j1]];
                int j2 = j1 + ncheck;
                QLLR m2 = mvc[jind[j2]];
                mcv[j0] = llrcalc.Boxplus(m1, m2);
                mcv[j1] = llrcalc.Boxplus(m0, m2);
                mcv[j2] = llrcalc.Boxplus(m0, m1);
                // Hosma new changes
                if (LLRsynd_hard(j)) //(LLRsynd(j) > 0)
                {
                    mcv[j0] *= -1;
                    mcv[j1] *= -1;
                    mcv[j2] *= -1;
                }

                break;
            }
            case 4:
            {
                int j0 = j;
                QLLR m0 = mvc[jind[j0]];
                int j1 = j0 + ncheck;
                QLLR m1 = mvc[jind[j1]];
                int j2 = j1 + ncheck;
                QLLR m2 = mvc[jind[j2]];
                int j3 = j2 + ncheck;
                QLLR m3 = mvc[jind[j3]];
                QLLR m01 = llrcalc.Boxplus(m0, m1);
                QLLR m23 = llrcalc.Boxplus(m2, m3);
                mcv[j0] = llrcalc.Boxplus(m1, m23);
                mcv[j1] = llrcalc.Boxplus(m0, m23);
                mcv[j2] = llrcalc.Boxplus(m01, m3);
                mcv[j3] = llrcalc.Boxplus(m01, m2);
                // Hosma new changes
                if (LLRsynd_hard(j)) //(LLRsynd(j) > 0)
                {
                    mcv[j0] *= -1;
                    mcv[j1] *= -1;
                    mcv[j2] *= -1;
                    mcv[j3] *= -1;
                }

                break;
            }
            case 5:
            {
                int j0 = j;
                QLLR m0 = mvc[jind[j0]];
                int j1 = j0 + ncheck;
                QLLR m1 = mvc[jind[j1]];
                int j2 = j1 + ncheck;
                QLLR m2 = mvc[jind[j2]];
                int j3 = j2 + ncheck;
                QLLR m3 = mvc[jind[j3]];
                int j4 = j3 + ncheck;
                QLLR m4 = mvc[jind[j4]];
                QLLR m01 = llrcalc.Boxplus(m0, m1);
                QLLR m02 = llrcalc.Boxplus(m01, m2);
                QLLR m34 = llrcalc.Boxplus(m3, m4);
                QLLR m24 = llrcalc.Boxplus(m2, m34);
                mcv[j0] = llrcalc.Boxplus(m1, m24);
                mcv[j1] = llrcalc.Boxplus(m0, m24);
                mcv[j2] = llrcalc.Boxplus(m01, m34);
                mcv[j3] = llrcalc.Boxplus(m02, m4);
                mcv[j4] = llrcalc.Boxplus(m02, m3);

                // Hosma new changes
                if (LLRsynd_hard(j)) //(LLRsynd(j) > 0)
                {
                    mcv[j0] *= -1;
                    mcv[j1] *= -1;
                    mcv[j2] *= -1;
                    mcv[j3] *= -1;
                    mcv[j4] *= -1;
                }

                break;
            }
            case 6:
            {
                int j0 = j;
                QLLR m0 = mvc[jind[j0]];
                int j1 = j0 + ncheck;
                QLLR m1 = mvc[jind[j1]];
                int j2 = j1 + ncheck;
                QLLR m2 = mvc[jind[j2]];
                int j3 = j2 + ncheck;
                QLLR m3 = mvc[jind[j3]];
                int j4 = j3 + ncheck;
                QLLR m4 = mvc[jind[j4]];
                int j5 = j4 + ncheck;
                QLLR m5 = mvc[jind[j5]];
                QLLR m01 = llrcalc.Boxplus(m0, m1);
                QLLR m23 = llrcalc.Boxplus(m2, m3);
                QLLR m45 = llrcalc.Boxplus(m4, m5);
                QLLR m03 = llrcalc.Boxplus(m01, m23);
                QLLR m25 = llrcalc.Boxplus(m23, m45);
                QLLR m0145 = llrcalc.Boxplus(m01, m45);
                mcv[j0] = llrcalc.Boxplus(m1, m25);
                mcv[j1] = llrcalc.Boxplus(m0, m25);
                mcv[j2] = llrcalc.Boxplus(m0145, m3);
                mcv[j3] = llrcalc.Boxplus(m0145, m2);
                mcv[j4] = llrcalc.Boxplus(m03, m5);
                mcv[j5] = llrcalc.Boxplus(m03, m4);
                // Hosma new changes
                if (LLRsynd_hard(j)) //(LLRsynd(j) > 0)
                {
                    mcv[j0] *= -1;
                    mcv[j1] *= -1;
                    mcv[j2] *= -1;
                    mcv[j3] *= -1;
                    mcv[j4] *= -1;
                    mcv[j5] *= -1;
                }
                break;
            }
            default:
            {
                int nodes = sumX2(j);
                if (nodes > max_cnd)
                {
                    std::ostringstream m_sout;
                    m_sout << "check node degrees >" << max_cnd << " not supported in this version";
                    it_error(m_sout.str());
                }

                nodes--;
                jj[0] = j;
                m[0] = mvc[jind[jj[0]]];
                for (int i = 1; i <= nodes; i++)
                {
                    jj[i] = jj[i - 1] + ncheck;
                    m[i] = mvc[jind[jj[i]]];
                }

                // compute partial sums from the left and from the right
                ml[0] = m[0];
                mr[0] = m[nodes];
                for (int i = 1; i < nodes; i++)
                {
                    ml[i] = llrcalc.Boxplus(ml[i - 1], m[i]);
                    mr[i] = llrcalc.Boxplus(mr[i - 1], m[nodes - i]);
                }

                // merge partial sums
                mcv[jj[0]] = mr[nodes - 1];
                mcv[jj[nodes]] = ml[nodes - 1];
                for (int i = 1; i < nodes; i++)
                    mcv[jj[i]] = llrcalc.Boxplus(ml[i - 1], mr[nodes - 1 - i]);

                if (LLRsynd_hard(j)) //(LLRsynd(j) > 0)
                {
                    for (int i = 0; i <= nodes; i++)
                        mcv[jj[i]] *= -1;
                }
            }
            } // switch statement
        }

        // step 2: variable to check nodes
        for (int i = 0; i < nvar; i++)
        {
            switch (sumX1(i))
            {
            case 0:
                it_error("LDPC_Code::bp_decode(): sumX1(i)=0");
            case 1:
            {
                //         This case is rare but apparently occurs for codes used in
                //           the DVB-T2 standard.

                QLLR m0 = mcv[iind[i]];
                mvc[i] = LLRin(i);
                LLRout(i) = LLRin(i) + m0;
                break;
            }
            case 2:
            {
                QLLR m0 = mcv[iind[i]];
                int i1 = i + nvar;
                QLLR m1 = mcv[iind[i1]];
                mvc[i] = LLRin(i) + m1;
                mvc[i1] = LLRin(i) + m0;
                LLRout(i) = mvc[i1] + m1;
                break;
            }
            case 3:
            {
                int i0 = i;
                QLLR m0 = mcv[iind[i0]];
                int i1 = i0 + nvar;
                QLLR m1 = mcv[iind[i1]];
                int i2 = i1 + nvar;
                QLLR m2 = mcv[iind[i2]];
                LLRout(i) = LLRin(i) + m0 + m1 + m2;
                mvc[i0] = LLRout(i) - m0;
                mvc[i1] = LLRout(i) - m1;
                mvc[i2] = LLRout(i) - m2;
                break;
            }
            case 4:
            {
                int i0 = i;
                QLLR m0 = mcv[iind[i0]];
                int i1 = i0 + nvar;
                QLLR m1 = mcv[iind[i1]];
                int i2 = i1 + nvar;
                QLLR m2 = mcv[iind[i2]];
                int i3 = i2 + nvar;
                QLLR m3 = mcv[iind[i3]];
                LLRout(i) = LLRin(i) + m0 + m1 + m2 + m3;
                mvc[i0] = LLRout(i) - m0;
                mvc[i1] = LLRout(i) - m1;
                mvc[i2] = LLRout(i) - m2;
                mvc[i3] = LLRout(i) - m3;
                break;
            }
            default:
            { // differential update
                QLLR mvc_temp = LLRin(i);
                int index_iind = i; // tracks i+jp*nvar
                for (int jp = 0; jp < sumX1(i); jp++)
                {
                    mvc_temp += mcv[iind[index_iind]];
                    index_iind += nvar;
                }
                LLRout(i) = mvc_temp;
                index_iind = i; // tracks i+j*nvar
                for (int j = 0; j < sumX1[i]; j++)
                {
                    mvc[index_iind] = mvc_temp - mcv[iind[index_iind]];
                    index_iind += nvar;
                }
            }
            }
        }

        // if (psc && syndrome_check(LLRout)) {
        //   is_valid_codeword = true;
        //   break;
        // }

        bvec key_out_bin = LLRout > 0;
        ivec sign_out = 2 * to_ivec(key_out_bin) - 1;
        QLLRvec llrsynd_a_new = soft_syndrome_check(sign_out);
        //     QLLRvec llrsynd_a_new =  soft_syndrome_check(LLRout);
        bvec s_bin_a_new = (llrsynd_a_new > 0);
        e_cnt = 0;
        for (int i = 0; i < LLRsynd_hard.length(); ++i)
        {
            if ((s_bin_a_new(i) + s_bin_b(i)))
                e_cnt++;
        }
        if (e_cnt < 1)
        {
            is_valid_codeword = true;
            break;
        }
    } while (iter < max_iters);

    //   if (nvar >= 100000) { it_info_debug(""); }
    return (is_valid_codeword ? iter : -iter);
}
