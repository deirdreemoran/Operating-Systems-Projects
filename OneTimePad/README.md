# One-Time Pad
Encrypts and decrypts messages using a one-time pad-like system.\
Multi-processing code with socket-based inter-process communication.

### Files
keygen.c:  generates a key of specified size\
otp_dec_d.c:   decoding server\
otp_dec.c: decoding client\
otp_enc_d.c:   encoding server\
otp_enc.c:  encoding client\
compileall: bash script for compilation\
plaintext: message to encrypt\
mykey: encrypted key\
myciphertext: encrypted message\
decrypted_message: decrypted message\

### Usage
$ compileall\
% otp_enc_d [port1] &\
$ otp_dec_d [port2] &\
$ keygen [number of characters] > mykey\
$ otp_enc myplaintext mykey [port1] > myciphertext\
$ otp_dec myciphertext mykey [port2] > decrypted_message
