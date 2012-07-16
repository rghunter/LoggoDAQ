#ifndef stringex_h
#define stringex_h

//supplementary string functions
int isBlank(char);
char upper(char);
void ltrim(char*);
void rtrim(char*);
void trim(char*);
int dtoi(char*);
int htoi(char*);
int stoi(char*);
void toDec(char*, int);
void to0Dec(char*, int, int);

extern char hexDigits[];

#endif
