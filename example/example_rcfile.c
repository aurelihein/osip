

#include <osip/port.h>
#include "example_rcfile.h"


static list_t *elements;

int loadsipconf(char *filename)
{

  FILE *file;
  char *s; 
  configelt_t *configelt;

  elements = (list_t *) smalloc(sizeof(list_t));
  list_init(elements);
#ifndef __VXWORKS_OS__
  file = fopen(filename, "r");
  if (file==NULL) return -1;
  s = (char *)smalloc(201*sizeof(char));
  while (NULL!=fgets(s, 200, file))
    {
    if ((strlen(s)==1)||(0==strncmp(s,"#",1)))
      ; /* skip it */
    else 
      {
      configelt = (configelt_t *)smalloc(sizeof(configelt_t));
      if (0<=setconfigelt((const char *)s,configelt))
	list_add(elements,configelt,-1);
      }
    }
  fclose(file);
#else

  configelt = (configelt_t *)smalloc(sizeof(configelt_t));
  if (0<=setconfigelt("displayname=Aymeric",configelt))
    list_add(elements,configelt,-1);

  configelt = (configelt_t *)smalloc(sizeof(configelt_t));
  if (0<=setconfigelt("username=jack",configelt))
    list_add(elements,configelt,-1);

  configelt = (configelt_t *)smalloc(sizeof(configelt_t));
  if (0<=setconfigelt("localip=136.10.15.182\n",configelt))
    list_add(elements,configelt,-1);

  configelt = (configelt_t *)smalloc(sizeof(configelt_t));
  if (0<=setconfigelt("contact=<sip:jack@136.10.15.182>\n",configelt))
    list_add(elements,configelt,-1);

  configelt = (configelt_t *)smalloc(sizeof(configelt_t));
  if (0<=setconfigelt("from=<sip:jack@136.10.15.182>\n",configelt))
    list_add(elements,configelt,-1);

  configelt = (configelt_t *)smalloc(sizeof(configelt_t));
  if (0<=setconfigelt("localport=5060\n",configelt))
    list_add(elements,configelt,-1);

  configelt = (configelt_t *)smalloc(sizeof(configelt_t));
  if (0<=setconfigelt("logfile=siplog1\n",configelt))
    list_add(elements,configelt,-1);

  /*  configelt = (configelt_t *)smalloc(sizeof(configelt_t));
  if (0<=setconfigelt("sipproxy=<sip:130.10.11.147:5080>\n",configelt))
    list_add(elements,configelt,-1);
    */

  configelt = (configelt_t *)smalloc(sizeof(configelt_t));
  if (0<=setconfigelt("sipregistrar=<sip:130.10.11.147:5090>\n",configelt))
    list_add(elements,configelt,-1);

  configelt = (configelt_t *)smalloc(sizeof(configelt_t));
  if (0<=setconfigelt("networktype=IN\n",configelt))
    list_add(elements,configelt,-1);

  configelt = (configelt_t *)smalloc(sizeof(configelt_t));
  if (0<=setconfigelt("addr_type=IP4\n",configelt))
    list_add(elements,configelt,-1);

#endif

  return 0; /* ok */
}

int
setconfigelt(const char *s,configelt_t *configelt)
{
  char *tmp;
  tmp = strchr(s,61);  /* search "=" 61 */
  configelt->name = (char *) smalloc(tmp-s+1);
  configelt->value= (char *) smalloc(s+strlen(s)-tmp);
  sstrncpy(configelt->name,s,tmp-s);
  sstrncpy(configelt->value,tmp+1,s+strlen(s)-tmp-2);
  return 1;
}

char *
getsipconf(char *name)
{
  configelt_t *tmp;
  int pos = 0;
  while (0==list_eol(elements,pos))
    {
    tmp = (configelt_t *)list_get(elements,pos);
    if (0==strcmp(tmp->name,name))
      return tmp->value;
    pos++;
    }
  return NULL;

}

