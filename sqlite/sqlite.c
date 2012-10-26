#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

sqlite3 *openSqlite(char *dbname)
{
  sqlite3 *db;
  if(sqlite3_open(dbname, &db) != SQLITE_OK)
    {
      sqlite3_errmsg(&db);
      return NULL;
    }
  return db;
}

int runSql(sqlite3 *db, char *sql)
{
  char *err;
  if(sqlite3_exec(db, "BEGIN", NULL, NULL, &err) != SQLITE_OK)
    {
      sqlite3_errmsg(&db);
      return -1;
    }

  
}

int main(int argc, char **argv)
{
  sqlite3 *db;
  char dbfile[16];
  char *sql;

  if((db = openSqlite("test.db")) == NULL)
    {
      fprintf(stdout, "openSqlite");
      exit(EXIT_FAILURE);
    }

  /* (void)sqlite3_exec(db, "BEGIN", NULL, NULL, &errmsg);  */
  
  /* sql = sqlite3_mprintf("select c.displayname, m.from_dispname, m.body_xml from Messages as m inner join Conversations as c on m.convo_id = c.id where author not like '%s' and body_xml like '%%%s%%' and timestamp > %d order by timestamp", pconf->skype.user, key, (int)past); */
  /* (void)sqlite3_exec(db, sql, callback, NULL, &errmsg); */
  
  /* sql = sqlite3_mprintf("select friendlyname from Chats where name not like '%%$%s%%' friendlyname like '%%%s%%'", pconf->skype.user, key); */
  /* (void)sqlite3_exec(db, sql, callback, NULL, &errmsg); */
  
  /* (void)sqlite3_exec(db, "COMMIT", NULL, NULL, &errmsg);  */

  sqlite3_close(db);
  
  return 0;
}
