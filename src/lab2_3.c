#include <stdio.h>

int is_prime(int n) {
  if (n < 2) return 0;
  for (int i = 2; i * i <= n; i++) {
    if (n % i == 0) return 0;
  }
  return 1;
}

int main() {
  int n;
  printf("Entrez n : ");
  scanf("%d", &n);

  if (n < 2) {
    printf("Erreur : n doit etre superieur ou egal a 2\n");
  } else {
    printf("Nombres premiers jusqu'a %d : ", n);
    for (int i = 2; i <= n; i++) {
      if (is_prime(i) == 1) {
        printf("%d ", i);
      }
    }
    printf("\n");
  }
  return 0;
}
