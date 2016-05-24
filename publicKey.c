#include<stdio.h>
#include<math.h>
#include<locale.h>

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

        printf("2^%d = %d\n", jj, count);
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

/**
 * calculated the modular exponentiation
 * M^{e} mod q
 */
int modularExponentiation(int M, int e, int n)
{
    unsigned long c;
    int largestPowerOf2 = sizeof(unsigned long) * 8;

    /*
    printf("Unsigned long can hold %zu bytes\n", sizeof(unsigned long));
    printf("This is the same as 2^%zu = %'.0f\n", (sizeof(unsigned long) * 8),
            pow(2.0, (double)(sizeof(unsigned long) * 8)));
    */

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

int main(void)
{
    int value = -1;
    setlocale(LC_NUMERIC, "");

    //printBinarySum(117);
    //printf("5^117 = 2^x where x = %lf", powerOfBase2(5, 117));

    value = modularExponentiation(198, 219469, 3921);
    if (value != -1)
    {
        printf("Found value = %d\n", value);
    }
    //printf("solution %d\n", modularExponentiation(3, 97, 353));

    return 1;
}
