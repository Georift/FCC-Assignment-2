#include<stdio.h>
#include<math.h>
#include<locale.h>
#include<time.h>
#include<stdlib.h>
#include<stdbool.h>

#define PRIME_MIN 10000
#define PRIME_MAX 100000

int modularExponentiation(int M, int e, int n);

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
int modularExponentiation(int M, int e, int n)
{
    unsigned long c;
    int largestPowerOf2 = sizeof(unsigned long) * 8;

    /** 
     * unsigned longs have a max size of 2^64 
     * if it's longer we will need to split up into parts
     */

    /* we can't perform this calculation we need
     * to break it up into smaller pieces. */
    if (powerOfBase2(M, e) >= largestPowerOf2)
    {
        int modProduct = 1;
        int remainingPower = e;

        /* check if it's even and split it */
        modProduct *= modularExponentiation(M, (remainingPower/2), n);
        remainingPower -= remainingPower/2;

        modProduct *= modularExponentiation(M, remainingPower, n);

        c = modProduct % n;
    }
    else
    {
        c = (unsigned long)pow((double)M, (double)e) % n;
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

int main(void)
{
    /* setup functions for external libraries*/
    srand(time(NULL));
    setlocale(LC_NUMERIC, "");
    setbuf(stdout, NULL);

    /*
    value = modularExponentiation(198, 219469, 3921);
    if (value != -1)
    {
        printf("Found value = %d\n", value);
    }

    int primeToTest = 991;
    bool isPrime = loopLehmann(primeToTest, 10);
    if (isPrime)
    {
        printf("%d is a prime number.\n", primeToTest);
    }
    else
    {
        printf("%d is a prime number.\n", primeToTest);
    }
    
    findPrime(900, 1000);
    */

    /**
     * start the generation of the key
     * 1. pick two prime numbers p and q
     * 2. generate n from n = p.q
     * 3. compute phi(n) from n - (p + q - 1)
     * 4. pick a value for e such that 1 < e < phi(n)
     * 5. find d given d.e = 1(mod phi(n))
     */

    int p, q;
    unsigned long n;

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
    n = p * q;

    printf("We have the following values:\n");
    printf("p = %d\nq = %d\nn = %lu\n", p, q, n);

    unsigned long phiN = (unsigned long)(p - 1) * (unsigned long)(q - 1);

    printf("phi(n) = %lu\n", phiN);



    return 1;
}
