//////////////////////////////////////////////////////////
// This file is a part of Nanos Copyright (C) 2008, 2012//
// ashok.s.das@gmail.com           GNU GPL-V2           //
//////////////////////////////////////////////////////////
// string and related functions header                  //
//                                                      //
//////////////////////////////////////////////////////////
#ifndef _STRING_H_
#define _STRING_H_
namespace String{
/*
   SOME STANDARD TYPEDEFS
                          */

#ifndef size_t
#define size_t unsigned int
#endif

#ifndef NULL
#define NULL 0
#endif

/*
   PROTOTYPES
              */

/*void
movedata (unsigned srcseg, unsigned srcoff,
          unsigned destseg, unsigned destoff, size_t n);
*/
char *memcpy (void *dest, const void *src, size_t n);

char *memset (void *s, int c, size_t n);

char *memchr (const void *s, int c, size_t n);

int memcmp (const void *s1, const void *s2, size_t n);

char *strcat (char *s, const char *append);

char *strchr (const char *s, int c);

int strcmp (const char *s1, const char *s2);

char *strcpy (char *to, const char *from);

size_t strcspn (const char *s1, const char *s2);

size_t strlen (const char *str);
size_t strnlen(const char *str,size_t MAX_LEN);
char *strncat (char *dst, const char *src, size_t n);

int strncmp (const char *s1, const char *s2, size_t n);

char *strncpy (char *dst, const char *src, size_t n);

char *strpbrk (const char *s1, const char *s2);

char *strrchr (const char *s, int c);

size_t strspn (const char *s1, const char *s2);

char *strstr (const char *s, const char *find);

char *strtok (char *s, const char *delim);

size_t strxfrm (char *dst, char *src, size_t n);

char *strlwr (char *s);

char *strupr (char *s);
int atoi(const char *s);
void itoa(char *buf, int base, long long d);
}; //namespace string
#endif
