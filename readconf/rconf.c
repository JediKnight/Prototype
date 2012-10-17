#include <getopt.h>
#include <stdio.h> 
#include <stdlib.h>
#include <sqlite3.h>  
#include <string.h>
#include <time.h>

#include <glib-2.0/glib.h>
#define GROUP_SETTING "setting"
#define KEY_INTERVAL "interval"
#define KEY_KEYWORDFILE "keywordfile"
#define GROUP_SKYPE "skype"
#define KEY_USER "user"

typedef struct {
  struct {
    gint interval;
    gchar *keywordfile;
  } setting;

  struct {
    gchar *user;
  } skype;
} config_t;

config_t *pconf;

int readconf(char *conffilename)
{  
  GKeyFile *keyfile;
  GError *error = NULL;
  gsize len;
  gchar *pdata;
  FILE *pconffile;

  pconf = g_slice_new(config_t);
  keyfile = g_key_file_new();

  if(!g_key_file_load_from_file(keyfile, conffilename, G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS, &error))
    {
      perror("g_key_file_load_from_file");

      pconf->setting.interval = 300;
      pconf->setting.keywordfile = "/etc/sls.key";
      pconf->skype.user = "username";

      g_key_file_set_integer(keyfile, GROUP_SETTING, KEY_INTERVAL, pconf->setting.interval);
      g_key_file_set_string(keyfile, GROUP_SETTING, KEY_KEYWORDFILE, pconf->setting.keywordfile);
      g_key_file_set_string(keyfile, GROUP_SKYPE, KEY_USER, pconf->skype.user);

      if(!(pdata = g_key_file_to_data(keyfile, &len, &error)))
	perror("g_key_file_to_data");

      if(!(pconffile = fopen(conffilename, "w")))
	perror("fopen");

      if(fwrite(pdata, len, 1, pconffile) < 1)
	perror("fwrite");

      fclose(pconffile);
      return -1;
    }

  pconf->setting.interval = g_key_file_get_integer(keyfile, GROUP_SETTING, KEY_INTERVAL, NULL);
  pconf->setting.keywordfile = g_key_file_get_string(keyfile, GROUP_SETTING, KEY_KEYWORDFILE, NULL);
  pconf->skype.user = g_key_file_get_string(keyfile, GROUP_SKYPE, KEY_USER, NULL);

  g_key_file_free(keyfile);

  return 0;
}

int main(int argc, char **argv)
{
  if(readconf(argv[1]) < 0)
    {
      printf("add conffile");
      return -1;
    }

  printf("%s\n", pconf->setting.interval);
  
  return 0;
}
