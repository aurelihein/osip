/*
  The oSIP library implements the Session Initiation Protocol (SIP -rfc2543-)
  Copyright (C) 2001  Aymeric MOIZARD jack@atosc.org
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#ifdef ENABLE_MPATROL
#include <mpatrol.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include <osip/smsg.h>
#include <osip/port.h>

int test_message (char *msg, int verbose, int clone);

void
usage ()
{
  fprintf (stderr, "Usage: ./torture_test torture_file number [-v] [-c]\n");
  exit (1);
}

int
main (int argc, char **argv)
{
  int i;
  int verbose = 0;		/* 1: verbose, 0 (or nothing: not verbose) */
  int clone = 0;		/* 1: verbose, 0 (or nothing: not verbose) */
  char *marker;
  FILE *torture_file;
  char *tmp;
  char *msg;
  char *tmpmsg;
  static int num_test = 0;

  if (argc > 3)
    {
      if (0 == strncmp (argv[3], "-v", 2))
	verbose = 1;
      else if (0 == strncmp (argv[3], "-c", 2))
	clone = 1;
      else
	usage ();
    }

  if (argc > 4)
    {
      if (0 == strncmp (argv[4], "-v", 2))
	verbose = 1;
      else if (0 == strncmp (argv[4], "-c", 2))
	clone = 1;
      else
	usage ();
    }

  if (argc < 3)
    {
      usage ();
    }

  torture_file = fopen (argv[1], "r");
  if (torture_file == NULL)
    {
      usage ();
    }

  /* initialize parser */
  parser_init ();

  i = 0;
  tmp = (char *) smalloc (500);
  marker = fgets (tmp, 500, torture_file);	/* lines are under 500 */
  while (marker != NULL && i < atoi (argv[2]))
    {
      if (0 == strncmp (tmp, "|", 1))
	i++;
      marker = fgets (tmp, 500, torture_file);
    }
  num_test++;

  msg = (char *) smalloc (50000);	/* msg are under 10000 */
  if (msg == NULL)
    {
      fprintf (stderr, "Error! smalloc failed\n");
      return -1;
    }
  tmpmsg = msg;

  if (marker == NULL)
    {
      fprintf (stderr,
	       "Error! The message's number you specified does not exist\n");
      exit (1);			/* end of file detected! */
    }
  /* this part reads an entire message, separator is "|" */
  /* (it is unlinkely that it will appear in messages!) */
  while (marker != NULL && strncmp (tmp, "|", 1))
    {
      sstrncpy (tmpmsg, tmp, strlen (tmp));
      tmpmsg = tmpmsg + strlen (tmp);
      marker = fgets (tmp, 500, torture_file);
    }

  if (verbose)
    {
      fprintf (stdout, "test %s : ============================ \n", argv[2]);
      fprintf (stdout, "%s", msg);

      if (0 == test_message (msg, verbose, clone))
	fprintf (stdout, "test %s : ============================ OK\n",
		 argv[2]);
      else
	fprintf (stdout, "test %s : ============================ FAILED\n",
		 argv[2]);
    }
  else
    {
      if (0 == test_message (msg, verbose, clone))
	fprintf (stdout, "test %s : OK\n", argv[2]);
      else
	fprintf (stdout, "test %s : FAILED\n", argv[2]);
    }

  sfree (msg);
  sfree (tmp);
  fclose (torture_file);

  return 0;
}

int
test_message (char *msg, int verbose, int clone)
{
  sip_t *sip;

  {
    char *result;

    /* int j=10000; */
    int j = 1;

    fprintf (stdout,
	     "Trying %i sequentials calls to msg_init(), msg_parse() and msg_free()\n",
	     j);
    while (j != 0)
      {
	j--;
	msg_init (&sip);
	if (msg_parse (sip, msg) != 0)
	  {
	    fprintf (stdout, "ERROR: failed while parsing!\n");
	    msg_free (sip);	/* try to free msg, even if it failed! */
	    sfree (sip);
	    return -1;
	  }
	msg_free (sip);		/* try to free msg, even if it failed! */
	sfree (sip);
      }

    msg_init (&sip);
    if (msg_parse (sip, msg) != 0)
      {
	fprintf (stdout, "ERROR: failed while parsing!\n");
	msg_free (sip);		/* try to free msg, even if it failed! */
	/* this seems dangerous..... */
	return -1;
      }
    else
      {
	int i;

	i = msg_2char (sip, &result);
	if (i == -1)
	  {
	    fprintf (stdout, "ERROR: failed while printing message!\n");
	    msg_free (sip);
	    sfree (sip);
	    return -1;
	  }
	else
	  {
	    if (verbose)
	      fprintf (stdout, "%s", result);
	    if (clone)
	      {
		/* create a clone of message */
		/* int j = 10000; */
		int j = 1;

		fprintf (stdout,
			 "Trying %i sequentials calls to msg_clone() and msg_free()\n",
			 j);
		while (j != 0)
		  {
		    sip_t *copy;

		    j--;
		    i = msg_clone (sip, &copy);
		    if (i != 0)
		      {
			fprintf (stdout,
				 "ERROR: failed while creating copy of message!\n");
		      }
		    else
		      {
			char *tmp;

			msg_force_update (copy);
			i = msg_2char (copy, &tmp);
			if (i != 0)
			  {
			    fprintf (stdout,
				     "ERROR: failed while printing message!\n");
			  }
			else
			  {
			    if (0 == strcmp (result, tmp))
			      {
				if (verbose)
				  printf
				    ("The msg_clone method works perfectly\n");
			      }
			    else
			      printf
				("ERROR: The msg_clone method DOES NOT works\n");
			    if (verbose)
			      printf ("Here is the copy: \n%s\n", tmp);

			    sfree (tmp);
			  }
			msg_free (copy);
			sfree (copy);
		      }
		  }
		fprintf (stdout, "sequentials calls: done\n");
	      }
	    sfree (result);
	  }
	msg_free (sip);
	sfree (sip);
      }
  }
  return 0;
}
