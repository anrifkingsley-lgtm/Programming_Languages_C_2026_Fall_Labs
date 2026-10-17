#include <stdio.h>

long long factorial(int n) {
  long long fact = 1;
  for (int i = 1; i <= n; i++) {
    fact *= i;
  }
  return fact;
}

int main() {
  int n;
  printf("Entrez n : ");
  scanf("%d", &n);

  if (n < 0) {
    printf("Erreur : n doit etre superieur ou egal a 0\n");
  } else {
    printf("La factorielle est : %lld\n", factorial(n));
  }
  return 0;
}