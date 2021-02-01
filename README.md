# Encoder_MLC
Encoder for MLC-MSD

The encoder for MLC-MSD environment. 
It is about encoding the classical data (Integer 6-bit digitized data) in at most 3 levels with different encoder/decoder blocks.

# The inputs:
  1- The path to the env_info.txt: 
    It contains the information about the envitonment. Including information are:
      a- Total number of levels, common frame length, number of levels in use
      b- Code rates
  2-The path to the encoder0.
  3-The path to the encoder1.
  4-The path to the encoder2.
  5-path to the rare data. (We are on Reverse reconciliation)
    a- It should read a Rx.h5 and store the integer data.

# Output:
  1- The encoded data (Stored in an h5 file)
  2- The part of the data stransmitted as plain text (Stored in an h5 file)
  
  
  All the methods related to the encoder and the environment are in a class namaed MLC
  All the functions related to the read and write are in a separate liraray
