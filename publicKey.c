#include<stdio.h>
#include<math.h>
#include<locale.h>
#include<time.h>
#include<stdlib.h>
#include<stdbool.h>
#include<limits.h>
#include<string.h>
#include<tommath.h>

#define PRIME_MIN 50000
#define PRIME_MAX 100000

/*
 * Used for storing the results of an
 * extended euclidean algorithm
 */
typedef struct {
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
    long long int c;
    long long int largestPowerOf2 = sizeof(unsigned long) * 8;

    /** 
     * unsigned longs have a max size of 2^64 
     * if it's longer we will need to split up into parts
     */

    /* we can't perform this calculation we need
     * to break it up into smaller pieces. */
    if (powerOfBase2(M, e) >= largestPowerOf2)
    {
        long long int modProduct = 1;
        long long int remainingPower = e;

        /* check if it's even and split it */
        modProduct *= modularExponentiation(M, (remainingPower/2), n);
        remainingPower -= remainingPower/2;

        modProduct *= modularExponentiation(M, remainingPower, n);

        c = modProduct % n;
    }
    else
    {
        c = (long long int)pow((long long int)M, (long long int)e) % (long long int)n;
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

    printf("input is a = %lli and b = %lli\n", a, m);

    if (extended.a != 1)
    {
        // the values a and m are not coprime.
        // ie. GCD(a, b) != 1
        retVal = 0; // return an impossible value
    }
    else
    {
        retVal = extended.x;
        printf("ext.a = %lli\next.x = %lli\next.y = %lli\n", 
                extended.a, extended.x, extended.y);
    }

    return retVal;
}

/*
 * finds the greatest common divisor between two
 * numbers a and b.
 */
int gcd(long long int a, long long int b)
{
    int gcd;
    while (a != 0 && b != 0)
    {
        if (a > b)
        {
            a %= b;
        }
        else
        {
            b %= a;
        }
    }

    gcd = (int) b;
    if (a > b)
    {
        gcd = (int) a;
    }

    return gcd;
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

    q = 0;
    do
    {
        /* check if this is the first run, 
         * if not, we rejected q */
        if (q != 0)
        {
            printf(".q rejected");
        }

        q = findPrime(PRIME_MIN, PRIME_MAX);
        printf("found q");
    }while(q == p);
    printf("\n");

    /* compute n */
    newPair.private.n = (long long)p * (long long)q;
    newPair.public.n = newPair.private.n;

    printf("We have the following values:\n");
    printf("p = %'d\nq = %'d\nn = %'lli\n", p, q, newPair.private.n);
    
    /* compute the value of phiN */
    long long int phiN = (long long)(p - 1) * (long long)(q - 1);

    printf("phi(n) = %'lli\n", phiN);

    /* pick a value for e such that 
     * 1 < e < phiN and GCD(e, phiN) == 1*/
    do
    {
        newPair.public.e = ((long long)rand() % ((long long)phiN - 1 + 1)) + 1;
    }while(isCoprime(newPair.public.e, phiN) != true);

    printf("Selected a value for e = %'lli\n", newPair.public.e);

    /*
    Result ext = extendedGcd(35, 15);
    printf("ext.a = %lli\next.x = %lli\next.y = %lli\n", ext.a, ext.x, ext.y);
    */

    /* try and solve for d using the extended euclidean algorithm
     * to find the modular muliplicative inverse. */
    newPair.private.d = gcdModularMultiplicativeInverse(newPair.public.e, phiN);

    /* we have found a negative inverse.
     * add phiN to the value of d to it to
     * "flip" the modular clock so to speak
     */
    if (newPair.private.d < 0)
    {
        newPair.private.d += phiN;
    }

    printf("We have found d = %'lli\n", newPair.private.d);

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
void encrypt(const char *plainPtr, int length, PublicKey to)
{
    /*
    int ii;
    const int charSplit = 3;
    long long int message[(length / charSplit) + 1];
    */

    mp_int result, message, exp, n, shift, charVal, cipher;
    int res;
    if ((res = mp_init_multi(&result, &message, &exp, 
                    &charVal, &n, &shift, &cipher, NULL)) != MP_OKAY)
    {
        printf("Error initilising the number. %s\n", 
                mp_error_to_string(res));
    }

    mp_set_int(&shift, 1000);
    mp_set_int(&message, 0);
    mp_set_int(&exp, to.e);
    mp_set_int(&n, to.n);

    int index = 0;

    /* shift the message by 1000 and add the most recent char */
    while (plainPtr[index] != '\0')
    {
        mp_set_int(&charVal, (int)plainPtr[index]);

        mp_mul(&message, &shift, &message);
        mp_add(&message, &charVal, &message);
        /*printf("added %c to number it has a value %lu\n", 
                plainPtr[index], mp_get_int(&charVal));*/
        index++;
    }

    /* message mp_int contains the entire message to encrypt */
    int sizeOfCipher;
    char *cipherText;

    char output[16000];
    mp_toradix(&message, (char *)&output, 10);
    printf("message for m = %s\n", output);

    char output1[16000];
    mp_toradix(&exp, (char *)&output1, 10);
    printf("exp for m = %s\n", output1);

    char output2[16000];
    mp_toradix(&n, (char *)&output2, 10);
    printf("n for m = %s\n", output2);

    mp_exptmod(&message, &exp, &n, &cipher);

    if (mp_radix_size(&cipher, 10, &sizeOfCipher) == MP_OKAY)
    {
        printf("Creating char array of size %d\n", sizeOfCipher);
        cipherText = (char *)malloc(sizeof(char) * sizeOfCipher);

        mp_toradix(&cipher, cipherText, 10);
    }

    printf("%s\n", cipherText);

    

    /*
    message[ii / charSplit] = message[ii / charSplit] * 1000;
    message[ii / charSplit] += (long long int)plainPtr[ii];

    mp_int result, messageHolder, exp, n;
    int resultCode;

    if ((resultCode = mp_init_multi(&result, &messageHolder, &exp, &n, NULL)) 
            != MP_OKAY)
    {
        printf("Error initilising the number. %s\n", 
                mp_error_to_string(resultCode));
    }


    mp_exptmod(&messageHolder, &exp, &n, &result);

    bool debug = false;
    if (debug == true)
    {
        char output[16000];
        mp_toradix(&result, (char *)&output, 10);
        printf("cipher for m%d = %s\n", ii, output);

        char output1[16000];
        mp_toradix(&exp, (char *)&output1, 10);
        printf("exp for m%d = %s\n", ii, output1);

        char output2[16000];
        mp_toradix(&n, (char *)&output2, 10);
        printf("n for m%d = %s\n", ii, output2);

        char output3[16000];
        mp_toradix(&messageHolder, (char *)&output3, 10);
        printf("message for m%d = %s\n", ii, output3);
    }

    * convert this round to base 10 and output *
    char block[16000];
    mp_toradix(&result, (char *)&block, 10);

    printf("%s ", block);

    printf("\n");
    */
}

int main(void)
{
    /* setup functions for external libraries*/
    srand(time(NULL));
    setlocale(LC_NUMERIC, "");
    setbuf(stdout, NULL);

    //KeyPair sender;
    KeyPair receiver;

    //sender = generateKeyPair();
    receiver = generateKeyPair();

    char plaintext[9] = {"TestingT"};

    /* encrypt the message for the receiver */
    encrypt((char *)&plaintext, strlen(plaintext), receiver.public);

    //decrypt(cipher, receiver.private);


    return 1;
}
