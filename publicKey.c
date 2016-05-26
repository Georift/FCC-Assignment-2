#include<stdio.h>
#include<math.h>
#include<locale.h>
#include<time.h>
#include<stdlib.h>
#include<stdbool.h>

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
 * returns -1 on error/not found
 *         prime number.
 */
int findPrime(int start, int end)
{
    int retVal = -1;
    if (start < end)
    {
        int jj;
        for (jj = start; jj <= end; jj++)
        {
            if (loopLehmann(jj, 10) == true)
            {
                printf("Found a prime %d\n", jj);
            }
        }
    }

    return retVal;
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
    int value;
    setlocale(LC_NUMERIC, "");
    srand(time(NULL));

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

    return 1;
}
