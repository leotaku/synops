#include "synops_sql.h"
#include <malloc.h>
#include <sqlite3.h>
#include <synops.h>

int synops_open(const char *filename, synops **ppOps) {
  sqlite3 *db;
  int status;

  status = sqlite3_open(filename, &db);
  if (status != SQLITE_OK) {
    return status;
  }

  *ppOps = calloc(1, sizeof(synops));
  if (*ppOps == NULL) {
    return SQLITE_NOMEM;
  }
  (*ppOps)->db = db;

  status = sqlite3_exec(db, SQL_CREATE_TABLES, NULL, NULL, NULL);
  if (status != SQLITE_OK) {
    return status;
  }

  return 0;
}

int synops_close(synops *pOps) {
  int status;

  status = sqlite3_close(pOps->db);
  if (status != SQLITE_OK) {
    return status;
  }
  free(pOps);

  return 0;
}
