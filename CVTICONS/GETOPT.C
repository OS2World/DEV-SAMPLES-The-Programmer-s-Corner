/*********************************** Function GetOpt ************************/
/* takes an argument count and an array of pointers to argument strings     */
/* and a string variable that is a list of allowable option flags.          */
/* Single letters, case sensitive, and single digits are acceptable.        */
/* if a valid option is followed by a colon, GetOpt will return an argument */
/* that follows that option.  gnOptErr if set to 0 will turn of the error   */
/* message display, gnOptInd is the index to the current option being       */
/* processed.  gszOptArg is where the parameter following an option will be */
/* returned in.								    */
/****************************************************************************/
#define NULL 0
#define EOF (-1)

#include <string.h>
#include <stdio.h>

#define ERR(sz, nC)   if(gnOptErr){\
		char szErrBuf[2];\
		szErrBuf[0] = nC; szErrBuf[1] ='\0';\
		(void) printf("%s : ",szArgv[0]);\
		(void) printf(sz);\
		(void) printf(" : %s\n",szErrBuf);}

int  gnOptErr = 1;
int  gnOptInd = 1;
int  gnOptOpt;
char *gszOptArg;

int GetOpt(int nArgc, char **szArgv, char *szOpts)
{
  static   int  nSp = 1;
  register int  nC;
  register char *szP;

  if (nSp == 1)
    if(gnOptInd >= nArgc || szArgv[gnOptInd][0] != '-' ||
			    szArgv[gnOptInd][1] == '\0')
      return EOF;
  else
    if (strcmp(szArgv[gnOptInd], "--") == NULL)
    {
      gnOptInd++;
      return EOF;
    }
  gnOptOpt = nC = szArgv[gnOptInd][nSp];
  if(nC == ':' || (szP = strchr(szOpts, nC)) == NULL)
  {
    ERR("Illegal option --", (char) nC);
    if(szArgv[gnOptInd][++nSp] == '\0')
    {
      gnOptInd++;
      nSp = 1;
    }
    return '?';
  }
  if(*++szP == ':')
  {
    if(szArgv[gnOptInd][nSp+1] != '\0')
      gszOptArg = &szArgv[gnOptInd++][nSp+1];
    else
      if(++gnOptInd >= nArgc)
      {
	ERR("Option requires an argument -- ", (char)nC);
	nSp = 1;
	return '?';
      }
      else
	gszOptArg = szArgv[gnOptInd++];
    nSp = 1;
  }
  else
  {
    if(szArgv[gnOptInd][++nSp] == '\0')
    {
      nSp = 1;
      gnOptInd++;
    }
    gszOptArg = NULL;
  }
  return(nC);
}
