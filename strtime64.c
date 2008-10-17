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
     * The use of modulo in this file is not necessarily safe on pre-C99
       compilers
     * Gah. Nor is the use of larger ints. I think it's OK but I'd better
       go back 'n check

*/

#include "time64.h"


#undef HAVE_C99
#if defined(__STDC_VERSION__)&&(__STDC_VERSION__>=199901L)
#  define HAVE_C99
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
#  ifndef HAVE_C99
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
#ifdef HAVE_C99
#  include <stdbool.h>
#else
#  define bool int
#  define true 1
#  define false 0
#endif
/* Deal with dinosaurs. ANSI says there is no snprintf; OTOH everything has
   had one since about 1992. We detect on C99 - if we have it, we have snprintf
   You can define HAVE_SNPRINTF outside to override if you have one really
   even though you're not C99

   Actually might get away without this. I'll leave the preprocessing in here
   in case we do, remove it later if it turns out we can get around it.

   But we need stdint if we can. Same principle.
*/
#ifdef HAVE_C99
#  undef HAVE_SNPRINTF
#  define HAVE_SNPRINTF
#  undef HAVE_STDINT
#  define HAVE_STDINT
#endif

/* Sized ints. Get the standard set or fake it. Note you may need to override
   the I64_FORMAT if you're using a 17th century compiler */
#ifndef I64_FORMAT
#  define I64_FORMAT "lld"
#endif

#define NO_PADDING 'N'
#define DEFAULT_PADDING 'D'

typedef enum {
  copying_input,
  reading_modifiers,
  reading_width
} strftime_state;

/* the traditional C sucks. Standard strncpy returns dest, which is almost
   always useless. We return the number of characters written.
  
   NOTE!! We don't add the terminating '\0'. In this context it's handled
   elsewhere, but don't cut&paste this without being sure you know the
   consequences. */
static size_t scpy(char * dest, char const * src, size_t const len) {
  size_t l=len;
  while(*src&&l) {
    *dest++=*src++;
    --l;
  }
  return len-l;
}

#ifndef I64_FORMAT
/* This is for when we don't have a format for printing 64 bit integers. It's
   pretty theoretical since everything does if it has them at all, this is
   basically a safety in case the format string isn't available */
void format_64_int(char * s, int64_t i) {
  long a, b;
  if(i<0) {
    *s++='-';
    i=-i;
  }
  a=i%1000000000L;
  i/=1000000000L;
  b=i%1000000000L;
  i/=1000000000L;
  if(i!=0)
    sprintf(s, "%d%09ld%09ld", (int)i, b, a);
  else if(b)
    sprintf(s, "%d%09d", b, a);
  else
    sprintf(s, "%d", a);
}
#endif;


size_t strftime64(char * s, size_t max, const char * format,
                  const struct TM * tm) {
  strftime_state state = copying_input;
  size_t output_pos = 0;

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

  char const * format_start_pos=NULL;
  char const * format_width_beginning=NULL;
  int padding=DEFAULT_PADDING;
  int output_int;
  int default_width;
  bool force_lc=false;
  bool swap_case=false;
  bool alternate_representation=false;
  bool alternate_numbers=false;
  bool stf_err=false;
  while(!stf_err&&*format&&(output_pos<max)) {
    if(state==copying_input) {
      if(*format=='%') {
        format_start_pos=format++;
        state=reading_modifiers;
      } else
        if(enc_is_safe)
          s[output_pos++]=*format++;
        else {
          /* THIS IS INCORRECT albeit only slightly (need to make it
             reentrant and handle the redundant shift situation) */
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
          if(state==reading_modifiers) {
            format_width_beginning=format;
            state=reading_width;
          }
          format++;
          break;
  
        /* Finally, the actual conversion specifier characters */
        case 'A':
          output_string=nl_langinfo(DAY_1+tm->tm_wday);
          default_width=0;
          goto output_final_string;
  
        case 'a':
          output_string=nl_langinfo(ABDAY_1+tm->tm_wday);
          default_width=0;
          goto output_final_string;
  
        case 'B':
          output_string=nl_langinfo(MON_1+tm->tm_mon);
          default_width=0;
          goto output_final_string;
  
        case 'b':
        case 'h':
          output_string=nl_langinfo(ABMON_1+tm->tm_mon);
          default_width=0;
          goto output_final_string;
  
        case 'C':
          /* SU requires this to be 2 digits, but we ignore that. By default
             it's hence at lease 2 digits. This is debatable, so, give feedback
             if you want it changing... */
#ifdef I64_FORMAT
            sprintf(dump_buffer, "%" I64_FORMAT, (Year)tm->tm_year/100+19);
#else
            format_64_int(dump_buffer, (Year)tm->tm_year/100+19);
#endif
          output_string=dump_buffer;
          default_width=2;
          padding='0';
          goto output_final_string;
  
        case 'c':
          /* we'll "borrow" output_string for the format ;) */
          output_string=nl_langinfo(D_T_FMT);
        recurse_with_output_string_as_format:
          {
            size_t n_written;

            n_written=strftime64(s+output_pos, max-output_pos,
              output_string, tm);
            /* We'll try to keep the string safe even if there's an error;
               not everyone checks the return value... */
            if(n_written==0)
              stf_err=true;
            else
              output_pos+=n_written;
            ++format;
          }
          break;

        case 'D':
          sprintf(dump_buffer, "%02d/%02d/%02d",
            tm->tm_mon+1, tm->tm_mday, (int)tm->tm_year%100);
          output_string=dump_buffer;
          default_width=0;
          goto output_final_string;
  
        case 'e':
          padding=' ';
        case 'd':
          output_int=tm->tm_mday;
          default_width=2;
          goto output_final_int;
          
        case 'F':
          /* We need to handle dates outside the 0000-9999 range here. Not
             doing this for the moment; we'll come back to it...

             We're using an iterative methodology. Right? */
          if((tm->tm_year>=0)||(tm->tm_year<10000)) {
            sprintf(dump_buffer, "%04" I64_FORMAT "-%02d-%02d",
              tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday);
            output_string=dump_buffer;
          } else {
            output_string="0000-00-00";
            stf_err=true;
          }
          default_width=0;
          padding=' ';
          goto output_final_string;
  
        case 'H':
          output_int=tm->tm_hour;
          default_width=2;
          goto output_final_int;
  
        case 'l':
          padding=' ';
        case 'I':
          output_int=tm->tm_hour%12;
          default_width=2;
          goto output_final_int;
  
        case 'j':
          output_int=tm->tm_yday;
          default_width=3;
          goto output_final_int;
  
        case 'k':
          output_int=tm->tm_hour;
          default_width=2;
          goto output_final_int;
  
        case 'M':
          output_int=tm->tm_min;
          default_width=2;
          goto output_final_int;
  
        case 'm':
          output_int=tm->tm_mon+1;
          default_width=2;
          goto output_final_int;
  
        case 'n':
          s[output_pos++]='\n';
          ++format;
          state=copying_input;
          break;
  
        case 'p':
          if(tm->tm_hour<12)
            output_string=nl_langinfo(AM_STR);
          else
            output_string=nl_langinfo(PM_STR);
          default_width=0;
          goto output_final_string;
  
        case 'R':
          sprintf(dump_buffer, "%02d:%02d", tm->tm_hour, tm->tm_min);
          output_string=dump_buffer;
          default_width=0;
          goto output_final_string;
  
        case 'r':
          /* borrowing output_string again... */
          output_string=nl_langinfo(T_FMT_AMPM);
          goto recurse_with_output_string_as_format;

        case 'S':
          output_int=tm->tm_sec;
          default_width=2;
          goto output_final_int;
  
        case 's':
          /* This may well be unix specific. */
          output_string="(%s: Not yet implemented)";
          goto output_final_string;
  
        case 'T':
          sprintf(dump_buffer, "%02d:%02d:%02d",
            tm->tm_hour, tm->tm_min, tm->tm_sec);
          output_string=dump_buffer;
          default_width=0;
          goto output_final_string;
  
        case 't':
          /* !!MBCS!! */
          s[output_pos++]='\t';
          ++format;
          state=copying_input;
          break;
  
        case 'U':
          output_int=(tm->tm_yday-tm->tm_wday+6)/7;
          default_width=2;
          goto output_final_int;

        case 'u':
          output_int=tm->tm_wday?tm->tm_wday:7;
          default_width=1;
          goto output_final_int;
  
        case 'W':
          output_int=(tm->tm_yday-(tm->tm_wday+1)%7+6)/7;
          default_width=2;
          goto output_final_int;

        case 'w':
          output_int=tm->tm_wday;
          default_width=1;
          goto output_final_int;
  
        case 'X':
          output_string=nl_langinfo(T_FMT);
          goto recurse_with_output_string_as_format;

        case 'x':
          output_string=nl_langinfo(D_FMT);
          goto recurse_with_output_string_as_format;

        case 'Y':
#ifdef I64_FORMAT
          sprintf(dump_buffer, "%" I64_FORMAT, (Year)tm->tm_year+1900);
#else
          format_64_int(dump_buffer, (Year)tm->tm_year/1900);
#endif
          output_string=dump_buffer;
          default_width=4;
          padding='0';
          goto output_final_string;
  
        case 'y':
          output_int=tm->tm_year%100;
          default_width=2;
        output_final_int:
          if(padding==DEFAULT_PADDING)
            padding='0';
          sprintf(dump_buffer, "%d", output_int);
          output_string=dump_buffer;
          goto output_final_string;
  
        
  
        case 'G':
        case 'g':
        case 'P':
        case 'V':

        /* Z and z may prove a challenge; may need to forward it on to the
           underlying strftime. I've tried to avoid that for reasons of
           efficiency; we'll C */
        case 'Z':
        case 'z':
          sprintf(dump_buffer, "(%c: Not yet implemented)", (int)*format);
          output_string=dump_buffer;
        output_final_string:
          if(padding==DEFAULT_PADDING)
            padding=' ';
          {
            /* I don't think this section is technically MBCS safe. TBC */
            size_t n_to_cp=strlen(output_string);
            int pad_width;
            if(format_width_beginning) {
              pad_width=atoi(format_width_beginning);
              if(pad_width>=10000)
                /* silly long. Almost certainly an error. GNU rejects this so
                   we're compatible */
                goto dump_duff_format;
            } else
              pad_width=default_width;
            if(padding==NO_PADDING)
                /* override the previously set width */
              pad_width=0;
            if(pad_width>n_to_cp)
              pad_width-=n_to_cp;
            else
              pad_width=0;
            if(pad_width>max-output_pos)
              pad_width=max-output_pos;
            memset(s+output_pos, padding, pad_width);
            output_pos+=pad_width;
            output_pos+=scpy(s+output_pos, output_string, max-output_pos);
          }
          state=copying_input;
          format_start_pos=NULL;
          format_width_beginning=NULL;
          force_lc=false;
          swap_case=false;
          alternate_representation=false;
          alternate_numbers=false;
          padding=DEFAULT_PADDING;
          ++format;
          break;
  
        /* And the one very special character */
  
        case '%':
          output_string="%";
          default_width=0;
          if(padding==DEFAULT_PADDING)
            padding=' ';
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
