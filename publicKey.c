#include<stdio.h>
#include<math.h>
#include<locale.h>
#include<time.h>
#include<stdlib.h>
#include<stdbool.h>
#include<limits.h>
#include<string.h>
#include<tommath.h>

#define PRIME_MIN 10000
#define PRIME_MAX 100000

#define DEBUG 0

/*
 * Used for storing the results of an
 * extended euclidean algorithm
 */ typedef struct {
    long long int a;
    long long int x;
    long long int y;
} Result;

/*
 * Stores the private exponent and
 * key size which is the private key.
 */
typedef struct {
    long long int d;
    long long int n;
} PrivateKey;

/*
 * Stores the public exponent and the
 * key size which makes up the public key.
 */
typedef struct {
    long long int e;
    long long int n;
} PublicKey;

/*
 * Stores the public and private key structs.
 */
typedef struct {
    PrivateKey private;
    PublicKey public;
} KeyPair;

typedef struct {
    char *cipher;
    int length;
} cipher;

long long int modularExponentiation(long long int M, long long int e, long long int n);
Result extendedGcd(long long int a, long long int b);

/*
 * using the formula:
 * base^exponent = 2^x
 * log(base^exponent) = log(2^x)
 * exponent * log(base) = x * log(2)
 * x = (exponent * log(base)) / log(2)
 */
double powerOfBase2(int base, int exponent)
{
    return ((exponent * log(base)) / log(2));
}

/*
 * if the number is not 1 OR -1 it's NOT prime
 * if 1 OR -1 then prob prime is at most 50% chance of prime
 */
int lehmann(int p)
{
    int a = rand() % p;
    return (int)modularExponentiation(a, ((p - 1) / 2), p);
}

/*
 * run lehmann's t times
 *
 * once run t time the probability of false posititives
 * is 1 - 1/(2^(t)) where t is the number of times = 10.
 */
bool loopLehmann(int p, int t)
{
    int ii;
    bool prime = true;

    for(ii = 0; ii < t; ii++)
    {
        int result = lehmann(p);
        if (result != 1 && result != (p - 1))
        {
            prime = false; 
        }
    }
    
    return prime;
}


/**
 * calculated the modular exponentiation
 * M^{e} mod q
 * Uses the following law to split large values:
 * ( (M^{e/2} mod q) * (M^{e/2} mod q) ) mod q
 */
long long int modularExponentiation(long long int M, 
        long long int e, long long int n)
{
    /* long longs are able to store 2^64 */
    long long int c;
    long long int largestPowerOf2 = sizeof(unsigned long long) * 8;

    /* Because we can store at most 2^64 in pure c, we must
     * check if we have enough space in our data types. If no
     * we can't perform this calculation we need
     * to break it up into smaller pieces. */
    if (powerOfBase2(M, e) >= largestPowerOf2)
    {
        long long int modProduct = 1;
        long long int remainingPower = e;

        /* split into two "equal" parts using recusion */
        modProduct *= modularExponentiation(M, (remainingPower/2), n);
        remainingPower -= remainingPower/2;

        modProduct *= modularExponentiation(M, remainingPower, n);

        // once done, calculate the final value c
        c = modProduct % n;
    }
    else
    {
        c = (long long int) pow(M, e) % n;
    }
    
    return c;
}

/*
 * attempts to find a prime within the
 * range provided.
 *
 * returns the prime number
 *         -1 if range is larger than RAND_MAX
 */
int findPrime(int start, int end)
{
    int possiblePrime;
    bool foundPrime = false;

    do
    {
        /**
         * continue to pull random numbers until
         * it calls within our range.
         *
         * This is done instead of simply using modulus
         * to ensure we are given a uniform probability.
         */


        /*
         * TODO If I get time try and implement a fully uniform random number
         * generation
        printf("RAND_MAX = %d\nUnsigned long = %lu\n", RAND_MAX, (unsigned long)pow(2, sizeof(unsigned long) * 8));
        do
        {
            possiblePrime = rand();    
            printf("testing number %lu, int max is %d\n", possiblePrime, (int)pow(2, 16));
        }while (possiblePrime >= (unsigned long)start && possiblePrime <= (unsigned long)end);

        printf("Found a prime between %d and %d which is %lu\n", start, end, possiblePrime);
        */
        printf(".");
        possiblePrime = (rand() % (end - start + 1)) + start;

        /* test if this is a prime number */
        if (loopLehmann(possiblePrime, 10) == true)
        {
            foundPrime = true;
        }

        /* 
         * add a check to ensure we can actually generate
         * numbers large enough for our range.
         * Returns -1 if it's not valid
         */
        if (end > RAND_MAX)
        {
            foundPrime = true;
            possiblePrime = -1;
        }

    }while(foundPrime == false);

    return (int)possiblePrime;
}

/*
 * Performs the same function as the brute force was
 * of finding the modular multiplicative inverse however
 * it uses the extended euclidean algorithm to find it using:
 * extendedGcd(a, m) = {a, x, y}
 * we find the remaining is given by a
 * the inverse is given by.
 * NOTE: this will requre a and m to be coprime,
 *       ie. have a gcd of 1.
 */
long long int gcdModularMultiplicativeInverse(long long int a, long long int m)
{
    Result extended = extendedGcd(a, m);
    long long int retVal;

    if (DEBUG == 1)
    {
        printf("input is a = %lli and b = %lli\n", a, m);
    }

    if (extended.a != 1)
    {
        // the values a and m are not coprime.
        // ie. GCD(a, b) != 1
        retVal = 0; // return an impossible value
    }
    else
    {
        retVal = extended.x;
        if (DEBUG == 1)
        {
            printf("ext.a = %lli\next.x = %lli\next.y = %lli\n", 
                    extended.a, extended.x, extended.y);
        }
    }

    return retVal;
}

/*
 * calculates the greatest common divisor of the
 * provided integers a and b. Also works out the co-
 * -efficients that satisfies the equation:
 *  ax + by = gcd(a, b)
 *
 *  Returns a Result struct with:
 *          a = GCD(a, b)
 *          x = that satisfies the eqn for a
 *          y =     "    "   "   "   "
 */
Result extendedGcd(long long int a, long long int b)
{
    long long int oldx, oldy, x, y;
    oldx = 1;
    oldy = 0;
    x = 0;
    y = 1;

    while(b != 0)
    {
        long long int q = a / b; // integer division.
        long long int tmpx, tmpy, tmpa;

        /* for swapping variables around */
        tmpx = x;
        tmpy = y;
        tmpa = a;

        x = oldx - q * x; // calculate new x
        oldx = tmpx; // and store the old in oldx

        y = oldy - q*y; // calculte new y
        oldy = tmpy; // and store the old in oldy

        a = b;
        b = tmpa % b;
    }

    /* create a struct for returning
     * the three variables */
    Result output;
    output.a = a;
    output.x = oldx;
    output.y = oldy;

    return output;
}

/*
 * using the extended euclidean algorithm check
 * if the pair of passed in integers is coprime
 * through GCD(a, b) == 1.
 */
bool isCoprime(long long int a, long long int b)
{
    bool coprime = false;
    Result gcd = extendedGcd(a, b);

    if (gcd.a == 1)
    {
        coprime = true;
    }

    return coprime;
}

/*
 * generate a keypair and return the
 * private and public key in the KeyPair
 * structure.
 *
 * NOTE: you must seed the random number
 *       generator before generating a keypair
 */
KeyPair generateKeyPair()
{
    /**
     * start the generation of the key
     * 1. pick two prime numbers p and q
     * 2. generate n from n = p.q
     * 3. compute phi(n) from n - (p + q - 1)
     * 4. pick a value for e such that 1 < e < phi(n)
     * 5. find d given d.e = 1(mod phi(n))
     */

    KeyPair newPair;
    int p, q;

    printf("Generating primes.");
    /* select two distinct primes p and q */
    p = findPrime(PRIME_MIN, PRIME_MAX);
    printf("found p");

    /* this will continue to loop until we get
     * a distinct value for q relative to p */
    q = 0;
    do
    {
        /* check if this is the first run, 
         * if not, we rejected q */
        if (q != 0)
        {
            /* I've never actually seen this happen */
            printf(".q rejected");
        }

        q = findPrime(PRIME_MIN, PRIME_MAX);
        printf("found q");
    }while(q == p);
    printf("\n");

    /* compute n */
    newPair.private.n = (long long int)p * (long long int)q;
    newPair.public.n = newPair.private.n;

    if (DEBUG == 1)
    {
        printf("We have the following values:\n");
        printf("p = %'d\nq = %'d\nn = %'lli\n", p, q, newPair.private.n);
    }
    
    /* compute the value of phiN */
    long long int phiN = (long long int)(p - 1) * (long long int)(q - 1);

    /* pick a value for e such that 
     * 1 < e < phiN and GCD(e, phiN) == 1*/
    do
    {
        newPair.public.e = ((long long)rand() % ((long long)phiN - 1 + 1)) + 1;
    }while(isCoprime(newPair.public.e, phiN) != true);
    newPair.public.e = 7;

    if (DEBUG == 1)
    {
        printf("phi(n) = %'lli\n", phiN);
        printf("e = %'lli\n", newPair.public.e);
    }

    /* try and solve for d using the extended euclidean algorithm
     * to find the modular muliplicative inverse. */
    newPair.private.d = gcdModularMultiplicativeInverse(newPair.public.e, phiN);

    /* we have found a negative inverse.
     * add phiN to the value of d to it to
     * "flip" the modular clock so to speak
     */
    newPair.private.d = newPair.private.d % phiN;
    if (newPair.private.d < 0)
    {
        newPair.private.d += phiN;
    }

    if (DEBUG == 1)
    {
        printf("We have found d = %'lli\n", newPair.private.d);
    }

    return newPair;
}

/*
 * take the message passed in and output a cipher text
 * encrypted for the received using the to public key
 *
 * message should be a pointer to a char array, with the
 * final char being a null terminator.
 *
 * Encryption is: 
 * c(m) = m^(e) mod n
 *
 * PrivateKey not needed, but it'd be a good idea to sign
 * the message prior to encryption.
 */
void encrypt(const char *plainPtr, PublicKey to, char **cipherText)
{
    /*
    int ii;
    const int charSplit = 3;
    long long int message[(length / charSplit) + 1];
    */

    printf("%s\n", plainPtr);

    mp_int result, message, exp, n, shift, charVal, cipher;
    int res;
    if ((res = mp_init_multi(&result, &message, &exp, 
                    &charVal, &n, &shift, &cipher, NULL)) != MP_OKAY)
    {
        printf("Error initilising the number. %s\n", 
                mp_error_to_string(res));
    }

    /* initialize all of our variables in the format
     * of mp_int */
    mp_set_int(&shift, 1000);
    mp_set_int(&message, 0);
    mp_set_int(&exp, to.e);

    /* 
     * libtommath is a pure c implementation which
     * supports at most 32 bit integer input.
     *
     * because of this we must convert our long long
     * to a string an then use mp_read_radix()
     *
     * Pretty hacky... but it works.
     */
    char tmpString[64];
    sprintf(tmpString, "%lli", to.n);
    mp_read_radix(&n, tmpString, 10);

    int index = 0;
    
    /* ensure that the size of M is less than n */
    int numChars;
    //int numToStore;
    mp_radix_size(&n, 10, &numChars);
    /*if (numChars < 3)
    {
        // can't even store one char here...
    }
    else
    {
        // how many chars can we store?
        numToStore = (numChars / 3);
    }*/

    /* shift the message by 1000 and add the most recent char */
    while (plainPtr[index] != '\0')
    {
        /* convert the chars in into the libtommath mp_int */
        mp_set_int(&charVal, (int)plainPtr[index]);

        /* shift three values to the left and add in our next
         * char value. */
        mp_mul(&message, &shift, &message);
        mp_add(&message, &charVal, &message);

        index++;
    }

    /* message mp_int contains the entire message to encrypt */
    int sizeOfCipher;

    if (DEBUG == 1)
    {
        /* some debugging stuff */
        char output[16000];
        mp_toradix(&message, (char *)&output, 10);
        printf("message for m = %s\n", output);
        
        char output1[16000];
        mp_toradix(&exp, (char *)&output1, 10);
        printf("exp for m = %s\n", output1);

        char output2[16000];
        mp_toradix(&n, (char *)&output2, 10);
        printf("n for m = %s\n", output2);
    }
    

    /* use libtommath library to compute the Modular Exponentiation
     * given that we were not told explicitily to use our own function */
    mp_exptmod(&message, &exp, &n, &cipher);

    /* get a string representation of the computer cipher text */
    if (mp_radix_size(&cipher, 10, &sizeOfCipher) == MP_OKAY)
    {
        *cipherText = (char *)malloc(sizeof(char) * sizeOfCipher);
        mp_toradix(&cipher, *cipherText, 10);
    }

    printf("Output cipher text = %s\n", *cipherText);
}

void decrypt(char *cipherText, PrivateKey private, char **plainText)
{
    mp_int cipher, plain, d, n, shift, charVal;
    int res;
    if ((res = mp_init_multi(&cipher, &plain, &d, 
                    &charVal, &n, &shift, NULL)) != MP_OKAY)
    {
        printf("Error initilising the number. %s\n", 
                mp_error_to_string(res));
    }

    mp_read_radix(&cipher, cipherText, 10);

    if (DEBUG == 1)
    {
        char outputCipher[16000];
        mp_toradix(&cipher, (char *)outputCipher, 10);
        printf("input cipher = %s\n", outputCipher);

        printf("private.d = %lli\n", private.d);
        printf("private.n = %lli\n", private.n);
    }

    mp_set_int(&shift, 1000);
    mp_set_int(&plain, 0);

    //mp_set_int(&d, private.d);
    char dString[64];
    sprintf(dString, "%lli", private.d);
    mp_read_radix(&d, dString, 10);
    
    //mp_set_int(&n, private.n);
    char tmpString[64];
    sprintf(tmpString, "%lli", private.n);
    mp_read_radix(&n, tmpString, 10);

    mp_exptmod(&cipher, &d, &n, &plain);
    char *tmpIntChar;
    int sizeOfPlain;

    /* convert the mp_int into a char array of int's */
    if (mp_radix_size(&plain, 10, &sizeOfPlain) == MP_OKAY)
    {
        //printf("Creating char array of size %d\n", sizeOfCipher);
        tmpIntChar = (char *)malloc(sizeof(char) * sizeOfPlain);
        mp_toradix(&plain, tmpIntChar, 10);
    }

    char intText[16000];
    mp_toradix(&plain, (char *)intText, 10);
    printf("int text = %s\n", intText);
    

    /* convert the char array of int's back into plain text */

    /* take into account stripped leading zero's */


    unsigned long int plainInts = mp_get_int(&plain);
    printf("plain ints = %09lu\n", plainInts);
    int numChars = ceil((double)(sizeOfPlain - 1) / 3.0);
    printf("num chars = %d\n", numChars);

    *plainText = (char *)malloc(sizeof(char) * (numChars + 1));



    int jj;
    for (jj = numChars - 1; jj >= 0; jj--)
    {
        int tmpInt = 0;
        tmpInt = plainInts % 10;
        plainInts /= 10;

        tmpInt += 10 * (plainInts % 10);
        plainInts /= 10;

        tmpInt += 100 * (plainInts % 10);
        plainInts /= 10;

        (*plainText)[jj] = (char)tmpInt;
    }

    if (DEBUG == 1)
    {
        char block[16000];
        mp_toradix(&plain, (char *)&block, 10);
        printf("decrypted text: %s\n", block);

        char dOut[16000];
        mp_toradix(&d, (char *)&dOut, 10);
        printf("d text: %s\n", dOut);

        char nOut[16000];
        mp_toradix(&n, (char *)&nOut, 10);
        printf("n text: %s\n", nOut);
    }
}

void encryptFile(FILE *in, FILE* out, KeyPair receiver)
{
    /* mp_int ready for n */
    mp_int n;
    int res;
    if ((res = mp_init(&n)) != MP_OKAY)
    {
        printf("Error initilising the number. %s\n", 
                mp_error_to_string(res));
    }

    /* load it in */
    char tmpString[64];
    sprintf(tmpString, "%lli", receiver.private.n);
    mp_read_radix(&n, tmpString, 10);

    /* calculate how large we can store in each encryption */
    int numChars;
    int numToStore;
    mp_radix_size(&n, 10, &numChars);
    if (numChars < 3)
    {
        // can't even store one char here...
    }
    else
    {
        // how many chars can we store?
        numToStore = (numChars / 3);

        /* to be safe lets store one less than
         * the max number we can */
        if (numToStore > 1)
        {
            //numToStore -= 1;
        }
    }
    fprintf(out, "%d ", numToStore);
    printf("size to store = %d\n", numToStore);

    char *toEncrypt = (char *)malloc(sizeof(char) * (numToStore + 1));
    int curFilled = 0;

    char c;
    while(( c = fgetc(in) ) != EOF)
    {
        if (curFilled == numToStore)
        {
            toEncrypt[curFilled + 1] = '\0';
            /* process this encryption */
            char *cipher = NULL;
            encrypt((char *)toEncrypt, receiver.public, &cipher);

            fprintf(out, "%s ", cipher);
            curFilled = 0;
            free(toEncrypt);
            toEncrypt = (char *)malloc(sizeof(char) * (numToStore + 1));
        }

        printf("%c\n", c);
        toEncrypt[curFilled] = c;
        curFilled++;
    }
}

/*
 * accepts two file pointers for in and out.
 * The expected file format is a single in stating how
 * many chars exist within the 'blocks' and each set of
 * cipher texts to decrypt
 */
void decryptFile(FILE *in, FILE* out, KeyPair receiver)
{
    /* 
     * read in the number of chars we should expect
     * per RSA encrypted 'block'
     */
    int numChars;
    fscanf(in, "%d", &numChars);
    printf("There at %d chars per 'block' \n", numChars);

    /* store our input value before decryption */
    unsigned long value;

    /* loop through every cipher text until end of file */
    while (fscanf(in, "%lu", &value) != EOF)
    {
        printf("%lu\n", value);

        /* string will be (numChars + 1)*3 */
        char string[(numChars + 1) * 3];
        sprintf(string, "%lu", value);

        /* perform the decryption on each*/
        char *plainText;
        decrypt((char *)&string, receiver.private, &plainText);

        fprintf(out, "%s", plainText);
    }
}

int main(void)
{


    
    /* setup functions for external libraries*/
    srand(time(NULL));
    setlocale(LC_NUMERIC, "");
    setbuf(stdout, NULL);


    /* generate the keypair for the receiver
     * we aren't goint to sign so sender's isn't required. */
    KeyPair receiver;
    receiver = generateKeyPair();

    /* setup file pointers for reading in and writing
     * to the files */
    FILE *in, *out;
    in = fopen("input", "r");
    out = fopen("output", "w+");

    /* 
     * will read the file and encrypt the largest size
     * possible such that M < n.
     * Note: traditionally when performing RSA larger keys
     * will be used (2048 bits etc.) allowing for simply one
     * encryption and decryption.
     */
    encryptFile(in, out, receiver);

    fclose(in);
    fclose(out);

    in = fopen("output", "r");
    out = fopen("decrypted", "w+");

    /*
     * perform the reverse operation of the encryption
     */
    decryptFile(in, out, receiver);

    fclose(in);
    fclose(out);

    return 1;
}
