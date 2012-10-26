#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

sqlite3 *openSqlite(char *dbname)
{
  sqlite3 *db;
  if(sqlite3_open(dbname, &db) != SQLITE_OK)
    {
      sqlite3_errmsg(db);
      return NULL;
    }
  return db;
}

void callback()
{
}

int runSql(sqlite3 *db, char *sql)
{
  char *errmesg;
  if(sqlite3_exec(db, "BEGIN", NULL, NULL, &errmesg) != SQLITE_OK)
    {
      sqlite3_errmsg(db);
      fprintf(stderr, "%s\n", errmesg);
      return -1;
    }

  if(sqlite3_exec(db, sql, NULL, NULL, &errmesg) != SQLITE_OK)
    {
      sqlite3_errmsg(db);
      fprintf(stderr, "%s\n", errmesg);
      return -1;
    }

  if(sqlite3_exec(db, "COMMIT", NULL, NULL, &errmesg) != SQLITE_OK)
    {
      sqlite3_errmsg(db);
      fprintf(stderr, "%s\n", errmesg);
      return -1;
    }
  return 0;
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

  sql = sqlite3_mprintf("select friendlyname from Chats where name not like '%%$hoge%%' friendlyname like '%%hoge%%'");
  printf("%s\n", sql);
  /* (void)sqlite3_exec(db, sql, callback, NULL, &errmsg); */
  sqlite3_close(db);
  
  return 0;
}
