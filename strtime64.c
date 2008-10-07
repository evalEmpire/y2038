/* strtime64.c
   
   We provide the strftime and strptime functions in 64 bit form, along
   with their wchar equivalents.

   Doesn't work properly yet. Not integrated with the Makefile.

   To compile:
   gcc -g -Wall -std=c99 -pedantic -DUSE_TM64 -DSTRFTIME64_BUILD_MAIN \
     -o strftime64 -g strtime64.c time64.o -lm

   You might also want -DHANDLE_MBCS, -DENCODING_IS_[UN]SAFE, see below

   Note you'll need to have USE_TM64 set for time64.o

   Not currently "plain" ANSI C. This is because there is no MBCS until C99,
   which in turn means no non-ASCII charsets unless we can crobar them as
   ENCODING_IS_SAFE - OK for UTF8. Also no snprintf, I'll have to crowbar
   that too. Forthcoming.

   Several formats are not currently handled

   TO DO
     * Fix the ANSI problems.
     * Do the currently missing formats
     * And the modifiers
     * And the widths...
     * Check uncalculated fields Do The Right Thing
     * BCE/CE/BC/AD problems
     * Figure out what to do about ISO8601
     * wchars
     * could do with better commenting

*/

#undef WE_HAVE_C99
#if defined(__STDC_VERSION__)&&(__STDC_VERSION__>=199901L)
#  define WE_HAVE_C99
#endif

/* ENCODING_IS_SAFE and ENCODING_IS_UNSAFE are designed to declare whether
   the character encoding is safe against "accidentally" having a % character
   which is part of an MBC. UNSAFE is the, er, safe one because it assumes
   nothing about the encoding, however, this isn't available in bog-standard
   ANSI since mblen &c are not available until C99. We try to handle this
   below. ENCODING_IS_SAFE can be set for ASCII/ISO646, and indeed those and
   upward compatible encodings are the only things we can handle for ANSI.

   If neither is defined, we look for UTF8. If we have UTF8, we act like
   SAFE, otherwise like UNSAFE. So basically set them if you know for
   certain one thing or the other. glibc is UTF8 so you can set SAFE for thet.

   These are irrelevant without HANDLE_MBCS, in which case we'll set SAFE
   unless we have something silly in the compilation settings. Don't do
   silly things in the compilation settings.
*/
#ifdef HANDLE_MBCS
#  ifndef WE_HAVE_C99
/* We can't handle MBCS unless we find a proprietory way, which we'll handle
   as it comes up, since I've no way of knowing this in advance */
#    error "C environment is pre-C99. Cannot handle MBCS"
/* Otherwise do nothing, the settings of ENCODING_IS_[UN]SAFE carry through */
#  endif
#else
/* No MBCS. We error if UNSAFE is indicated, and define safe if it isn't
   already, since this is ASCII or (just possibly) EBCDIC, or something
   related to them */
#  ifdef ENCODING_IS_UNSAFE
#    error "Encoding unsafe in a non-MBCS environment. This makes no sense"
#  endif
#  undef ENCODING_IS_SAFE
#  define ENCODING_IS_SAFE
#endif
#if defined(ENCODING_IS_SAFE)&&defined(ENCODING_IS_UNSAFE)
#  error "Enconding can't be safe and unsafe at the same time. Try unsafe - it's safe with all encodings"
#endif

/* These are standard */
#include <string.h>  /* we have lots of string processing, it goes without
			saying */
#include <wchar.h>   /* MBCS functions are in here if we have them, but we
			need wchars for the wchar version anyway */
#include <stdlib.h>  /* mblen and related, if we have them. The inclusion
			does no harm if we don't */
#include <assert.h>  /* self explanatory */
#include <stdio.h>   /* We don't do any IO, but snprintf lives here if we
                        have it. */
/* This I'm pretty sure is X/OPEN, but any *nix worth its salt will have it.
   We'll have to figure out what to do with other platforms in the fullness
   of time.
  
   You'll probably tank out here if you're not X/OPEN compliant, but since
   I don't know the equivalent on whatever platform you're using, I don't
   know in advance what to do about it. Raise a complaint if you hit it ;) */
#include <langinfo.h>
/* Standard bools or fake bools... */
#ifdef WE_HAVE_C99
#  include <stdbool.h>
#else
#  define bool int
#  define true 1
#  define false 0
#endif
/* Deal with dinosaurs. ANSI says there is no snprintf; OTOH everything has
   had one since about 1992. We detect on C99 - if we have it, we have snprintf
   You can define HAS_SNPRINTF outside to override if you have one really
   even though you're not C99

   Actually might get away without this. I'll leave the preprocessing in here
   in case we do, remove it later if it turns out we can get around it.
*/
#ifdef WE_HAVE_C99
#  undef HAS_SNPRINTF
#  define HAS_SNPRINTF
#endif

#include "time64.h"

typedef enum {
  copying_input,
  reading_modifiers,
  reading_width
} strftime_state;

size_t strftime64(char * s, size_t max, const char * format,
                  const struct TM * tm) {
  strftime_state state;
  size_t output_pos;

#ifdef ENCODING_IS_SAFE
  const int enc_is_safe=1;
#else
#  ifdef ENCODING_IS_UNSAFE
  const int enc_is_safe=0;
#  else
  char const * enc=nl_langinfo(CODESET);
  /* I toyed with checking for a various charsets here, but I don't think it's
     efficient: is the number of comparisons we'll do in processing the loop
     worth running strcmp a bunch of times here?

     UTF8 will catch nearly every practical system these days, and
     the "unsafe" path is, er, safe if it _is_ safe... Note that glibc is
     UTF8 all through, so you can set ENCODING_IS_SAFE.
    
     The subject is open for discussion ;) */
  const int enc_is_safe=strcmp(enc, "UTF-8")==0;
#  endif
#endif

  output_pos=0;
  state=copying_input;
  while(*format&&(output_pos<max)) {
    char const * format_start_pos;
    int padding=' ';
    bool force_lc=false;
    bool swap_case=false;
    bool alternate_representation=false;
    bool alternate_numbers=false;
    if(state==copying_input) {
      if(*format=='%') {
        format_start_pos=format++;
        state=reading_modifiers;
      } else
        if(enc_is_safe)
          s[output_pos++]=*format++;
        else {
          int mbl=mblen(format, MB_CUR_MAX);
          assert(mbl); /* since '\0' should be caught by the loop cond */
          if(mbl<0)
            mbl=1;
          if(max-output_pos<=mbl) {
            strncpy(s+output_pos, format, mbl);
            format+=mbl;
            output_pos+=mbl;
          } else {
            /* The character is valid but we don't have room to store it.
               End of the game. The spec indicates that the contents of s
               are undefined if you run out of room, but we'll do our best
               to leave something that won't blow up if you use it... So we
               won't copy a partial, and we'll add a terminating \0, and
               advance output_pos off-end so we'll quit out */
            s[output_pos]='\0';
            output_pos=max;
          }
        } /* end else of (charset_is_safe) */
    } else
      switch(*format) {
        char dump_buffer[100]; /* a dumping ground when we want to print
                                  something with sprintf; I don't think this
                                  will ever occur in production, this is just
                                  while testing. */
        char * output_string;
        int n_wr;

        /* ANSI requires that some characters are available as a single char
           in the compilation environment. We process these directly. It should
           be noted, however, that the format string may be built up by
           something like sprintf, which may not be using the same charset as
           the compilation env. Well, we process these independently in any
           case; I've never seen a char encoding that doesn't have these as
           single characters and we can cross that bridge if we ever come to
           it... It should be noted that the glibc strftime does not appear
           to handle these obscure character sets either.

           Included are all the characters we're interested in for the format
           itself.

           Believed good charsets/encodings (assuming the underlying library
           functions behave themselves):
             ASCII
             ISO646 all (0x5C is not an issue as we don't use it)
             ISO8859 all
             ISO2022
             ECMA6 = ISO646
             ECMA35 = ISO2022
             EUC all
             UCS4 (if we have 32bit chars...) hence UCS2
             UTF8
             UTF16 (if we have 16bit chars)
  	     * basically any unicode except Punycode, which is not intended
  	       for this use.
             Big5, hence GCCS
             UHC
             GBK
             GB18030
             Shift-JIS (0x5C again not an issue)
             VISCII
             EBCDIC
  	         All IBM/Microsoft "codepages"
        */
  
        /* Next up are the modifiers, i. e. stuff that changes how the output
           is displayed. */
  
        case '#':
	  /* The semantics of this are obscure. The Gnu docs state it swaps
	     the case of the resulting string, but also state that it's not
	     really useful with anything but %Z. However, surely it might be
	     useful with %p or %P, or when ^ is used. Not quite sure what to
	     do atm, but I'll investigate */
	  swap_case=true;
	  ++format;
	  break;

        case '-':
	  padding=NO_PADDING;
	  ++format;
	  break;

        case '^':
	  force_lc=true;
	  ++format;
	  break;
	  
        case '_':
	  padding=' ';
	  ++format;
	  break;

        case '0':
	  padding='0';
	  ++format;
	  break;

        case 'E':
	  alternate_representation=true;
	  ++format;
	  break;

        case 'O':
	  alternate_numbers=true;
	  ++format;
	  break;

        /* The numbers, apart from 0 which has a dual role. These mean we're
           getting a format width. */
  
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          /* I'll figure these out later */
          switch(state) {
            case reading_modifiers:
              state=reading_width;
            case reading_width:
              sprintf(dump_buffer, "(Number: %c)", (int)*format);
              format++;
              output_string=dump_buffer;
              goto output_some_string;

            default:
              assert(false);
          }
          break;
  
        /* Finally, the actual conversion specifier characters */
        case 'A':
          output_string=nl_langinfo(DAY_1+tm->tm_wday);
          goto output_final_string;
  
        case 'a':
          output_string=nl_langinfo(ABDAY_1+tm->tm_wday);
          goto output_final_string;
  
        case 'B':
          output_string=nl_langinfo(MON_1+tm->tm_mon);
          goto output_final_string;
  
        case 'b':
        case 'h':
          output_string=nl_langinfo(ABMON_1+tm->tm_mon);
          goto output_final_string;
  
        case 'C':
          /* SU requires this to be 2 digits, but we ignore that. By default
             it's hence at lease 2 digits. This is debatable, so, give feedback
             if you want it changing... */
          n_wr=snprintf(s+output_pos, max-output_pos,
            "%2lld", (long long)(tm->tm_year+1900)/100);
        move_on:
          ++format;
          force_lc=false;
          swap_case=false;
          alternate_representation=false;
          alternate_numbers=false;
          if(n_wr>0)
            output_pos+=n_wr;
          /* not a great deal we can do about it if there's an error signaled */
          state=copying_input;
          break;
  
        case 'D':
          n_wr=snprintf(s+output_pos, max-output_pos, "%02d/%02d/%02d",
            tm->tm_mday, tm->tm_mon+1, (int)tm->tm_year%100);
          goto move_on;
  
        case 'd':
          n_wr=snprintf(s+output_pos, max-output_pos,
            "%02d", tm->tm_mday);
          goto move_on;
  
        case 'e':
          n_wr=snprintf(s+output_pos, max-output_pos,
            "%2d", tm->tm_mday);
          goto move_on;
  
        case 'H':
          n_wr=snprintf(s+output_pos, max-output_pos, "%02d", tm->tm_hour);
          goto move_on;
  
        case 'I':
          n_wr=snprintf(s+output_pos, max-output_pos, "%02d", tm->tm_hour%12);
          goto move_on;
  
        case 'j':
          n_wr=snprintf(s+output_pos, max-output_pos, "%03d", tm->tm_yday);
          goto move_on;
  
        case 'k':
          n_wr=snprintf(s+output_pos, max-output_pos, "%2d", tm->tm_hour);
          goto move_on;
  
        case 'l':
          n_wr=snprintf(s+output_pos, max-output_pos, "%2d", tm->tm_hour%12);
          goto move_on;
  
        case 'M':
          n_wr=snprintf(s+output_pos, max-output_pos, "%02d", tm->tm_min);
          goto move_on;
  
        case 'm':
          n_wr=snprintf(s+output_pos, max-output_pos, "%02d", tm->tm_mon);
          goto move_on;
  
        case 'n':
          s[output_pos++]='\n';
          ++format;
          state=copying_input;
          break;
  
        case 'P':
          output_string="(%P: Not yet implemented)";
          goto output_final_string;
  
        case 'p':
          if(tm->tm_hour<12)
            output_string=nl_langinfo(AM_STR);
          else
            output_string=nl_langinfo(PM_STR);
          goto output_final_string;
  
        case 'R':
          n_wr=snprintf(s+output_pos, max-output_pos, "%02d:%02d",
            tm->tm_hour, tm->tm_min);
          goto move_on;
  
        case 'S':
          n_wr=snprintf(s+output_pos, max-output_pos, "%02d", tm->tm_sec);
          goto move_on;
  
        case 's':
          /* This may well be unix specific. */
          output_string="(%s: Not yet implemented)";
          goto output_final_string;
  
        case 'T':
          n_wr=snprintf(s+output_pos, max-output_pos, "%02d:%02d:%02d",
            tm->tm_hour, tm->tm_min, tm->tm_sec);
          goto move_on;
  
        case 't':
          s[output_pos++]='\t';
          ++format;
          state=copying_input;
          break;
  
        case 'u':
          n_wr=snprintf(s+output_pos, max-output_pos, "%d",
            tm->tm_wday?tm->tm_wday:7);
          goto move_on;
  
        case 'w':
          n_wr=snprintf(s+output_pos, max-output_pos, "%d", tm->tm_wday);
          goto move_on; 
  
        case 'Y':
          n_wr=snprintf(s+output_pos, max-output_pos, "%4lld",
            (long long)tm->tm_year+1900);
          goto move_on;
  
        case 'y':
          n_wr=snprintf(s+output_pos, max-output_pos, "%2d",
            (int)tm->tm_year%100);
          goto move_on;
  
        
  
        case 'c':
        case 'F':
        case 'G':
        case 'g':
        case 'r':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'x':
        case 'Z':
        case 'z':
          sprintf(dump_buffer, "(%c: Not yet implemented)", (int)*format);
          output_string=dump_buffer;
        output_final_string:
          state=copying_input;
          ++format;
          force_lc=false;
          swap_case=false;
          alternate_representation=false;
          alternate_numbers=false;
        output_some_string:
          {
            size_t n_to_cp=strlen(output_string);
            if(n_to_cp>max-output_pos)
              n_to_cp=max-output_pos;
            memcpy(s+output_pos, output_string, n_to_cp);
            output_pos+=n_to_cp;
          }
          break;
  
        /* And the one very special character */
  
        case '%':
          output_string="%";
          goto output_final_string;
  
        /* In the default case, we look for an mbchar, and copy whatever we
         have. It's duff if we get anything not listed above, mbchar or not,
         other than when we're copying the input. */

        default:
          ++format;
        dump_duff_format:
          /* we'll do our best to copy the format over to provide the
             programmer with clues ;) Also that's what the gnu strftime
             seems to do */
          assert(format_start_pos);
          n_wr=format-format_start_pos<max-output_pos?
            format-format_start_pos:
            max-output_pos;
          strncpy(s+output_pos, format_start_pos, n_wr);
          output_pos+=n_wr;
          state=copying_input;
          break;

      } /* end switch(*format) */
    /* end else if(state==copying_input) */
  } /* end while(*format&(output_pos<max)) */
  if(output_pos>=max) {
    /* we ran out of room. We return zero, but this doesn't always indicate
       an error since some conversion specifiers can legitimately expand
       to nothing. Also there's no way to indicate some other error.

       This sucks but that's what the strftime spec says...

       We'll add a '\0' just as a safety in case someone tries to print the
       damn thing.
    */
    s[max-1]='\0';
    output_pos=0;
  } else
    s[output_pos]='\0';
  /* output_pos is the number of characters written into s unless we zeroed it
     above because we ran out of room, and hence forms the return value */
  return output_pos;
}

#ifdef STRFTIME64_BUILD_MAIN

/* Simple testing main(), this'll be removed on integration to the main
   suite
*/

#include <locale.h>
#include <time.h>

int main(int argc, char**argv) {
  char ** i;
  struct TM tm;
  Time64_T t;

  setlocale(LC_ALL, "");

  t=time(0);
  localtime64_r(&t, &tm);
  for(i=argv+1; *i; ++i) {
    char b[10000];
    int n;

    n=strftime64(b, 10000, *i, &tm);
     if(!n)
       printf("<%s> returned %d\n", *i, n);
     else
       printf("<%s> <%s>\n", *i, b);
   }
   return 0;
}


#endif

