#ifndef SYNOPS_SQL_H
#define SYNOPS_SQL_H

#include <sqlite3.h>
#include <stdio.h>

// clang-format off

static const char *SQL_CREATE_TABLES =
    "CREATE TABLE IF NOT EXISTS files"
    "(filename TEXT PRIMARY KEY, hash TEXT, etag TEXT)";

static const char *SQL_FILE_EXISTS =
  "SELECT EXISTS("
  "  SELECT * FROM files"
  "  WHERE filename=?1"
  "  AND etag=?3 || ?4 || ?5 || ?6 || ?7 || ?8"
  ");";

static const char *SQL_INSERT_FILE =
    "INSERT OR REPLACE INTO files (filename, hash, etag)"
    "VALUES(?1, ?2, ?3 || ?4 || ?5 || ?6 || ?7 || ?8);";

static const char *SQL_DELETE_FILE =
  "DELETE FROM files "
  "WHERE filename=:1";

static const char *SQL_GET_FILES =
  "SELECT (filename)"
  "FROM files;";

// clang-format on

#define TRACE_ERROR(status) \
  printf("%s:%d: %s\n", __FILE__, __LINE__, sqlite3_errstr(status))

#define ENSURE(expr)           \
  {                            \
    int status = (expr);       \
    if (status != SQLITE_OK) { \
      TRACE_ERROR(status);     \
      return status;           \
    }                          \
  }

#define ENSURE_DONE(expr)        \
  {                              \
    int status = (expr);         \
    if (status != SQLITE_DONE) { \
      TRACE_ERROR(status);       \
      return status;             \
    }                            \
  }

#endif
