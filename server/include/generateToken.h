#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define SHA256_ROTL(a,b) (((a>>(32-b))&(0x7fffffff>>(31-b)))|(a<<b))
#define SHA256_SR(a,b) ((a>>b)&(0x7fffffff>>(b-1)))
#define SHA256_Ch(x,y,z) ((x&y)^((~x)&z))
#define SHA256_Maj(x,y,z) ((x&y)^(x&z)^(y&z))
#define SHA256_E0(x) (SHA256_ROTL(x,30)^SHA256_ROTL(x,19)^SHA256_ROTL(x,10))
#define SHA256_E1(x) (SHA256_ROTL(x,26)^SHA256_ROTL(x,21)^SHA256_ROTL(x,7))
#define SHA256_O0(x) (SHA256_ROTL(x,25)^SHA256_ROTL(x,14)^SHA256_SR(x,3))
#define SHA256_O1(x) (SHA256_ROTL(x,15)^SHA256_ROTL(x,13)^SHA256_SR(x,10))
int GenerateToken(char *user,char *tokenValue);
char* StrSHA256(const char* str, int length, char* sha256);
int token_insert(char *username,char* tokenValue);
int token_query(char *username,char *tokenValue);
int token_update(char *username,char* tokenValue);
int token_query1(char *username);
int token_query2(char *username,char *tokenValue);
