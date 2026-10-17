#include <stdio.h>

int sum_to_n(int n) {
  int sum = 0;
  for (int i = 1; i <= n; i++) {
    sum += i;
  }
  return sum;
}

int main() {
  int n;
  printf("Entrez n : ");
  scanf("%d", &n);
  if (n < 1) {
    printf("Erreur : n doit etre superieur ou egal a 1\n");
  } else {
    printf("La somme est : %d\n", sum_to_n(n));
  }
  return 0;
}