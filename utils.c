/* 
 * Copyright (c) 2013 Dylan Reinhold
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * dsdtune is a wrapper around the dsdplus to find the best values to run under
 */
#include <ctype.h>
#include "dsdtune.h"
#include "utils.h"
#include <time.h>

unsigned int decode_rate(char *line) {
  /* Assume line is formated as :
   * decoding score = 42,072
   */
  char tmp_rate[15];
  int i,j,len;
  memset(tmp_rate,'0',14);
  tmp_rate[14] = '\0';
  len = strlen(line);
  line += len - 1 ;
  for(i=0,j=13; i < len; i++) {
    if(*(line-i) == ',' || *(line-i) == '\n' || *(line-i) == '\r')
      continue;
    if(isdigit(*(line-i))) {
      tmp_rate[j--] = *(line-i);
    } else {
      i=len+1;
    }
  }
  return atol(tmp_rate);
}

void get_time_stamp(char *timestamp) {
  time_t sec;
  sec =  time(NULL);
  sprintf(timestamp, "%s", ctime(&sec));
}

void write_batch(dsd_params *params, options *opts) {
  /* Write a batch file with the sugested settings */
  FILE *b_fh;
  char timestamp[30];
  int i = 0;
  
  b_fh = fopen(opts->batch_name, "wt");
  if(b_fh == NULL) {
    fprintf(stderr, "ERROR : Could not create batch file %s (%d)\n", opts->batch_name, errno);
    return;
  }
  get_time_stamp(timestamp);
  fprintf(b_fh, "@echo off\n");
  fprintf(b_fh, "REM : batch file created with dsdtune utility on %s\n", timestamp);
  fprintf(b_fh, "\n");
  if(opts->decode_option_set) 
    if(opts->batch_options != NULL)
      fprintf(b_fh, "%s %s -f%s", opts->exe_name, opts->batch_options, opts->decode_option);
    else
      fprintf(b_fh, "%s -f%s", opts->exe_name, opts->decode_option);
  else
    fprintf(b_fh, "%s", opts->exe_name);

  while(params[i].name[0] != ' ') {
    fprintf(b_fh, " -%s%u",params[i].name, params[i].best_setting);
    i++;
  }
  fprintf(b_fh, "\n");
  fclose(b_fh);
  printf("\nCreated batch file %s to run dsd\n", opts->batch_name);
}

