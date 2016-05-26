#include<stdio.h>
#include<math.h>
#include<locale.h>
#include<time.h>
#include<stdlib.h>
#include<stdbool.h>
#include<limits.h>

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

int modularExponentiation(unsigned int M, unsigned int e, unsigned int n);
Result extendedGcd(long long int a, long long int b);

/**
 * find the binary sum of a number
 */
void printBinarySum(unsigned long num)
{
    /* work out where to start from */
    int remaining = num;
    int jj;
    for (jj = floor(sqrt(num)); jj >= 0; jj--)
    {
        // check how many 2^{jj} we can remove
        int count = remaining / (int)pow(2.0, jj);
        remaining = remaining - (count * (int)pow(2.0, jj));
    } 
}

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
    return modularExponentiation(a, ((p - 1) / 2), p);
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
int modularExponentiation(unsigned int M, unsigned int e, unsigned int n)
{
    unsigned long long c;
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
        c = (long long)pow((long long)M, (long long)e) % (long long)n;
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

/**
 * Find a value x such that:
 * a.x = 1 mod m
 * x is the modular multiplicative.
 *
 * @returns 0 on error otherwise value of x
 * NOTE: Code taken from previously submitted
 *       assignment #1 written by my.
 */
int modularMultiplicativeInverse(int a, int m)
{
    bool foundInverse = false;
    int inverse = 0;
    int index = 0;

    while (foundInverse == false)
    {
        int tempCalc;

        tempCalc = ((a * index) % m);
        if (tempCalc == 1)
        {
            inverse = index;
            foundInverse = true;
        }

        index++;
        if (index == m)
        {
            /* this shouldn't ever happen... */
            inverse = 0; /* signifies an error */
        }
    }

    return inverse;
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

int main(void)
{
    /* setup functions for external libraries*/
    srand(time(NULL));
    setlocale(LC_NUMERIC, "");
    setbuf(stdout, NULL);

    /* begin the key generation */

    /**
     * start the generation of the key
     * 1. pick two prime numbers p and q
     * 2. generate n from n = p.q
     * 3. compute phi(n) from n - (p + q - 1)
     * 4. pick a value for e such that 1 < e < phi(n)
     * 5. find d given d.e = 1(mod phi(n))
     */

    int p, q;
    long long n;

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
    n = (long long)p * (long long)q;

    printf("We have the following values:\n");
    printf("p = %'d\nq = %'d\nn = %'lli\n", p, q, n);
    
    /* compute the value of phiN */
    long long int phiN = (long long)(p - 1) * (long long)(q - 1);

    printf("phi(n) = %'lli\n", phiN);

    /* pick a value for e such that 
     * 1 < e < phiN and GCD(e, phiN) == 1*/
    long long int e;

    do
    {
        e = ((long long)rand() % ((long long)phiN - 1 + 1)) + 1;
    }while(gcd(e, phiN) != 1);
    printf("Selected a value for e = %'lli\n", e);

    /*
    Result ext = extendedGcd(35, 15);
    printf("ext.a = %lli\next.x = %lli\next.y = %lli\n", ext.a, ext.x, ext.y);
    */

    /* try and solve for d using the extended euclidean algorithm
     * to find the modular muliplicative inverse. */
    long long int d = gcdModularMultiplicativeInverse(e, phiN);

    /*
     sanity check 
    if (ext.a != 1)
    {
          e and phiN are not coprime 
          * this shouldn't happen as it 
          * can't have broken out of 
          * the while loop
    }*/

    printf("We have found d = %'lli\n", d);


    return 1;
}
