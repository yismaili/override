#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct var {
  char  msg[140];
  char  username[40];
  int   len;
};

void  secret_backdoor() {
  char  str[128];
  fgets(str, 128, stdin);
  system(str);
}

void  set_msg(struct var *test) {
  char  str[1024] = {0};

  puts(">: Msg @Unix-Dude");
  printf(">>: ");
  fgets(str, 1024, stdin);
  strncpy(test->msg, str, test->len);
}

void  set_username(struct var *test) {
  int i = 0;
  char str[128] = {0};

  puts(">: Enter your username");
  printf(">>: ");
  fgets(str, 128, stdin);

  while (i <= 40 && str[i]) {
    test->username[i] = str[i];
    i++;
  }

  printf(">: Welcome, %s", test->username);
}

void  handle_msg() {
  struct var test = {0};

  test.username;
  test.len = 140;
  set_username (&test);
  set_msg(&test);
}

int main(void) {
  puts("--------------------------------------------\n|   ~Welcome to l33t-m$n ~    v1337        |\n--------------------------------------------\n");
  handle_msg();
  return 0;
}