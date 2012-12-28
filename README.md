# Gauss Crack

The goal of the program is to find the target of the Gauss Virus so that the encrypted payload can finally be decrypted.

[Download here](https://github.com/downloads/lightswitch05/gaussCrack/GaussCheck.zip) - [Screenshots](https://github.com/lightswitch05/gaussCrack/wiki) can be found on the wiki.

The algorithm steps taken by this application are outlined in the [securelist.com blog](https://www.securelist.com/en/blog/208193781/The_Mystery_of_the_Encrypted_Gauss_Payload)

## Code
* Quick explination of the code:
    * [MainWindow](https://github.com/lightswitch05/gaussCrack/blob/master/application/gaussCrack/mainwindow.cpp)
        * All user input / output
    * [KeyPairQueue](https://github.com/lightswitch05/gaussCrack/blob/master/application/gaussCrack/keypairqueue.cpp)
        * Gets all the environment paths and applicaiton names outlined in validation step 1 and creates a queue of the key pairs
    * [HashThread](https://github.com/lightswitch05/gaussCrack/blob/master/application/gaussCrack/hashthread.cpp)
        * Pulls a keypair from the queue, appends salt, does the actual hashing and comparing
    * [HashGenerator](https://github.com/lightswitch05/gaussCrack/blob/master/application/gaussCrack/hashgenerator.cpp)
        * Contains and manages multiple HashThreads. Intermediate between HashThread and MainWindow


Copyright (c) 2012, Daniel White. Released under the [MIT License](https://github.com/lightswitch05/gaussCrack/blob/master/MIT-LICENSE)
