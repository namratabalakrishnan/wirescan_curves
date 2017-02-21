#include <stddef.h>
#include <stdio.h>

#define ELEMENT_COUNT(X) (sizeof(X) / sizeof((X)[0]))

void convolve(const double Signal[/* SignalLen */], size_t SignalLen,
              double SmoothResult[/*SignalLen*/])
{
  double Kernel[] = { 0.249, 0.249, 0.249, 0.249};
  size_t KernelLen = ELEMENT_COUNT(Kernel);
  
  double Result[SignalLen + KernelLen - 1];
  
    
  size_t n;

  for (n = 0; n < SignalLen + KernelLen - 1; n++)
  {
    size_t kmin, kmax, k;
    
    Result[n] = 0;

    kmin = (n >= KernelLen - 1) ? n - (KernelLen - 1) : 0;
    kmax = (n < SignalLen - 1) ? n : SignalLen - 1;

    for (k = kmin; k <= kmax; k++)
    {
      Result[n] += Signal[k] * Kernel[n - k];
    }
  }
  
  size_t j;
  
  j = SignalLen - KernelLen - 1;
  for (n = 0; n < SignalLen; n++)
  {
    SmoothResult[n] = Signal [n];
    if ((n == j) && (j != (SignalLen-1)))
    {
       SmoothResult[n] = Result[j+1];
       j++;
    }
   }
}

void printSignal(const char* Name,
                 double Signal[/* SignalLen */], size_t SignalLen)
{
  size_t i;

  for (i = 0; i < SignalLen; i++)
  {
    printf("%s[%zu] = %f\n", Name, i, Signal[i]);
  }
  printf("\n");
}


int main(void)
{
  double signal[] = { 17.88, 17.92, 17.96, 18, 19, 20, 21 };
  double smoothResult[ELEMENT_COUNT(signal)];

  convolve(signal, ELEMENT_COUNT(signal),
           smoothResult);

  printSignal("signal", signal, ELEMENT_COUNT(signal));
  printSignal("smoothResult", smoothResult, ELEMENT_COUNT(smoothResult));
  //printSignal("kernel", kernel, ELEMENT_COUNT(kernel));
  //printSignal("result", result, ELEMENT_COUNT(result));

  return 0;
}
