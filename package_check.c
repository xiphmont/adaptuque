#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define CRC_POLYNOMIAL 0x04c11db7L
#define CRC_INITVAL    0xffffffff

static int32_t ckop (int32_t csum, uint8_t s) {
  int32_t data = s << 24;
  int i;
  for (i=0; i<8; ++i) {
    if ((data^csum) & 0x80000000) {
      csum = (csum<<1) ^ CRC_POLYNOMIAL;
    } else {
      csum <<= 1;
    }
    data <<= 1;
  }
  return csum;
}

int main(void) {
  unsigned char tag[9]={0};
  unsigned char version[4]={0};
  int ch = 0;
  int len = 0;
  int repeatflag = 0;

  while ((ch=getchar()) != EOF) {
    tag[len] = ch;
    len++;
    putchar(ch);
    if (len == 8) {
      if ( memcmp("PMC\0", tag, 4) &&
           memcmp("PMC_", tag, 4) ){
        memmove(tag, tag+1, 7);
        len--;
        if ((repeatflag & 1) == 0)
          fprintf(stderr,"   Warning: unexpected byte(s) between binary sections.\n");
        repeatflag |= 1;
      } else {
        unsigned char word[4];
        uint32_t section_len = 0;
        repeatflag = 0;

        for (len = 0; len < 4 && (ch = getchar()) != EOF; len++) { putchar(ch); word[len] = ch;}
        if(len < 4){
          fprintf(stderr,"     ERROR: unexpected EOF checking type field in section header.\n");
          return 1;
        }

        if ( !memcmp("PMC\0\0\0\0\0", tag, 8) ){
          fprintf(stderr,">> Found firmware section header: PMC\n");
          if ( !memcmp("\0x01\0x0b\0x05\0x00", word, 4) ) {
            fprintf(stderr,"   Warning: section type mismatch "
                    "{%02x %02x %02x %02x} != {01 0b 05 00}\n.",
                    (unsigned int)word[0],
                    (unsigned int)word[1],
                    (unsigned int)word[2],
                    (unsigned int)word[3]);
          }
        } else if ( !memcmp("PMC_BOOT", tag, 8) ){
          fprintf(stderr,">> Found bootstrap section header: PMC_BOOT\n");
          if ( !memcmp("\0x01\0x0b\0xff\0x00", word, 4) ) {
            fprintf(stderr,"   Warning: section type mismatch "
                    "{%02x %02x %02x %02x} != {01 0b ff 00}\n.",
                    (unsigned int)word[0],
                    (unsigned int)word[1],
                    (unsigned int)word[2],
                    (unsigned int)word[3]);
            }
        } else if ( !memcmp("PMC_ISTR", tag, 8) ){
          fprintf(stderr,">> Found initialization section header: PMC_ISTR\n");
          if ( !memcmp("\0x01\0x0b\0x02\0x00", word, 4) ) {
            fprintf(stderr,"   Warning: section type mismatch "
                    "{%02x %02x %02x %02x} != {01 0b 02 00}\n.",
                    (unsigned int)word[0],
                    (unsigned int)word[1],
                    (unsigned int)word[2],
                    (unsigned int)word[3]);
          }
        } else {
          fprintf(stderr,">> Found unknown file section header: %s\n", tag);
          fprintf(stderr,"   Warning:  Packing it anyway.\n");
        }

        for (len = 0; len < 4 && (ch = getchar()) != EOF; len++) { word[len] = ch;}
        if(len < 4){
          fprintf(stderr,"     ERROR: unexpected EOF checking version field in section header.\n");
          return 1;
        }
        if ( !memcmp("\0\0\0\0", version, 4) ){
          memcpy(version, word, 4);
        } else {
          if ( memcmp(version, word, 4) ){
            fprintf(stderr,"   Warning: version mismatch in %s header.\n"
                    "            {%02x %02x %02x %02x} != {%02x %02x %02x %02x}.\n"
                    "            Replacing with previously seen versioning.\n",
                    tag,
                    (unsigned int)word[0],
                    (unsigned int)word[1],
                    (unsigned int)word[2],
                    (unsigned int)word[3],
                    (unsigned int)version[0],
                    (unsigned int)version[1],
                    (unsigned int)version[2],
                    (unsigned int)version[3]);
          }
        }

        for (len = 0; len < 4 && putchar(word[len]) != EOF; len++);

        for (len = 0; len < 4 && (ch = getchar()) != EOF; len++) { putchar(ch); word[len] = ch;}
        if(len < 4){
          fprintf(stderr,"     ERROR: unexpected EOF checking length field in %s header.\n", tag);
          return 1;
        }
        section_len = (word[0]<<24) | (word[1]<<16) | (word[2]<<8) | word[3];
        if (section_len > 512*1024*1024) {
          fprintf(stderr,"   ERROR: declared section length (%x) > 512MiB.  That seems unlikely.\n",
                  section_len);
          fprintf(stderr,"          Aborting.\n");
          return 1;
        } else {
          unsigned char *section = NULL;
          int32_t csum = CRC_INITVAL;
          int32_t prev_csum = 0;
          int32_t prev_start = 0;

          for (len = 0; len < 4 && (ch = getchar()) != EOF; len++) { word[len] = ch;}
          if(len < 4){
            fprintf(stderr,"     ERROR: unexpected EOF checking hash field in %s header.\n", tag);
            return 1;
          }
          prev_csum = word[0]<<24 | word[1]<<16 | word[2]<<8 | word[3];

          for (len = 0; len < 4 && (ch = getchar()) != EOF; len++) { word[len] = ch;}
          if(len < 4){
            fprintf(stderr,"     ERROR: unexpected EOF checking start pattern in %s header.\n", tag);
            return 1;
          }
          prev_start = word[0]<<24 | word[1]<<16 | word[2]<<8 | word[3];
          if (prev_start != 0xbf000000) {
            fprintf(stderr,"   Warning: unexpected start pattern in %s header.\n"
                           "            {%02x %02x %02x %02x} != {bf 00 00 00}.\n",
                    tag,
                    (unsigned int)word[0],
                    (unsigned int)word[1],
                    (unsigned int)word[2],
                    (unsigned int)word[3]);
          }

          section = calloc(section_len, 1);
          if (!section) {
            fprintf(stderr,"   ERROR: failed to allocate %d bytes of section memory.\n", section_len);
            fprintf(stderr,"          Aborting.\n");
            return 1;
          }

          for (len = 0; len < section_len && (ch = getchar()) != EOF; len++) {
            csum = ckop(csum, (uint8_t)ch);
            section[len] = ch;
          }

          if (len < section_len) {
            fprintf(stderr,"     ERROR: unexpected EOF reading section payload.\n");
            return 1;
          }

          csum = ~csum;

          fprintf(stderr,"   section bytes = %d (0x%08x)\n",
                  len,len);
          fprintf(stderr,"   original hash   {%02x %02x %02x %02x}\n",
                  (prev_csum>>24) & 0xff,
                  (prev_csum>>16) & 0xff,
                  (prev_csum>>8) & 0xff,
                  prev_csum & 0xff);
          fprintf(stderr,"   calculated hash {%02x %02x %02x %02x}\n",
                  (csum>>24) & 0xff,
                  (csum>>16) & 0xff,
                  (csum>>8) & 0xff,
                  csum & 0xff);

          putchar( (csum>>24) & 0xff );
          putchar( (csum>>16) & 0xff );
          putchar( (csum>>8) & 0xff );
          putchar( csum & 0xff );
          putchar( (prev_start>>24) & 0xff );
          putchar( (prev_start>>16) & 0xff );
          putchar( (prev_start>>8) & 0xff );
          putchar( prev_start & 0xff );

          for (len = 0; len < section_len && putchar(section[len]) != EOF; len++);

          if (len < section_len) {
            fprintf(stderr, "   ERROR: unable to write output.\n");
            return 1;
          }
          free (section);
        }
        len = 0;
      }
    }
  }

  if(len){
    fprintf(stderr,"   Warning: extra bytes at end of input.\n");
    return 1;
  }

  fprintf(stderr,"Done.\n");
  return 0;
}
