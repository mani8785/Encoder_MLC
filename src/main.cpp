/*
    Author: Hosma
    Company: DTU QPIT
    Title: Encoder for Reverse reconciliation,
    Details: 
*/
#include <iostream>
#include <fstream> // header to read file
// #include <itpp/comm/modulator.h>
#include "mlcmsd.h"

// ================================================ Main function
int main(int argc, char **argv)
{
    // string path_to_inputH5 = "/home/hosma/Documents/100-Largefiles/20210115-TxRxSymbols for EC/20km/Rxfile.h5";
    string path_to_inputH5 = "/home/hosma/Documents/100-Largefiles/20210302-CS-Set1-TxRx Symbols/20km/Rxfile.h5";
    string path_to_outptH5 = "/home/hosma/Documents/100-Largefiles/EncDataMLCMSD.h5";
    /*
        To measure time of encoding
    */
    Real_Timer my_timer;
    my_timer.tic();
    double start_time = my_timer.get_time();
    /*
        Construct MLC environment
    */
    MLCMSD mlc_env{}; // This will construct default environment for MLC-MSD
    mlc_env.load_env("struct.txt");
    mlc_env.check_env();
    /*
        Tx data prepration
        Final data will be stored in a 2D h5 file in test folder
    */
    hsize_t TNoEs, TFN;
    mlc_env.Rare_data_prepration(path_to_inputH5, true, TNoEs, TFN);
    /*
        Apply encoding frame by frame and store it in the output file
   */
    path_to_inputH5 = "tempInp2D.h5";
    mlc_env.encoding_main(path_to_inputH5, path_to_outptH5, TNoEs, TFN);
    /*
        Measure throughput 
    */
    double end_time = my_timer.get_time();
    mlc_env.display_elapsed_time(end_time, start_time, TNoEs);

    return 0;
}



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