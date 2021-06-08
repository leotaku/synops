#define _XOPEN_SOURCE 500

#include "synops_sql.h"
#include <ftw.h>
#include <malloc.h>
#include <sqlite3.h>
#include <string.h>
#include <synops.h>
#include <unistd.h>
#include <xxhash.h>

static __thread sqlite3_stmt *g_insert_file_stmt;
static __thread sqlite3_stmt *g_file_exists_stmt;
static __thread sqlite3_stmt *g_get_files_stmt;
static __thread sqlite3_stmt *g_delete_file_stmt;

int synops_bind_file_query(sqlite3_stmt *stmt,
                           const char *fpath,
                           const char *checksum,
                           const struct stat *stat) {
  ENSURE(sqlite3_reset(stmt));
  ENSURE(sqlite3_bind_text(stmt, 1, fpath, -1, SQLITE_TRANSIENT));
  ENSURE(sqlite3_bind_text(stmt, 2, checksum, -1, SQLITE_TRANSIENT));
  ENSURE(sqlite3_bind_int(stmt, 3, stat->st_mtime));
  ENSURE(sqlite3_bind_int(stmt, 4, stat->st_size));
  ENSURE(sqlite3_bind_int(stmt, 5, stat->st_ino));
  ENSURE(sqlite3_bind_int(stmt, 6, stat->st_mode));
  ENSURE(sqlite3_bind_int(stmt, 7, stat->st_uid));
  ENSURE(sqlite3_bind_int(stmt, 8, stat->st_gid));

  return 0;
}

int synops_file_checksum(const char *fpath, char *checksum) {
  FILE *f = fopen(fpath, "r");
  if (f == NULL) {
    return SQLITE_CANTOPEN;
  }

  XXH3_state_t *const state = XXH3_createState();
  char *buffer = calloc(BUFSIZ, sizeof(char));
  XXH3_64bits_reset_withSeed(state, 0);
  int bytes_read;

  while ((bytes_read = fread(buffer, sizeof(char), BUFSIZ, f))) {
    XXH3_64bits_update(state, buffer, bytes_read);
  }
  sprintf(checksum, "%016lx", XXH3_64bits_digest(state));
  XXH3_freeState(state);
  free(buffer);
  fclose(f);

  return 0;
}

static int g_synops_insert_file(const char *fpath, const struct stat *stat,
                                __attribute__((unused)) int tflag,
                                __attribute__((unused)) struct FTW *ftwbuf) {
  if (S_ISDIR(stat->st_mode)) {
    return 0;
  }

  ENSURE(synops_bind_file_query(g_file_exists_stmt, fpath, "", stat));
  while (sqlite3_step(g_file_exists_stmt) == SQLITE_ROW) {
    if (sqlite3_column_int(g_file_exists_stmt, 0)) {
      return 0;
    }
  }

  char checksum[16];
  ENSURE(synops_file_checksum(fpath, checksum));
  ENSURE(synops_bind_file_query(g_insert_file_stmt, fpath, checksum, stat));
  ENSURE_DONE(sqlite3_step(g_insert_file_stmt));
  printf("%s %s\n", checksum, fpath);

  return 0;
}

int synops_insert_files(synops *pOps, const char *filename) {
  ENSURE(sqlite3_prepare_v2(pOps->db, SQL_INSERT_FILE, -1, &g_insert_file_stmt, NULL));
  ENSURE(sqlite3_prepare_v2(pOps->db, SQL_FILE_EXISTS, -1, &g_file_exists_stmt, NULL));
  ENSURE(sqlite3_prepare_v2(pOps->db, SQL_GET_FILES, -1, &g_get_files_stmt, NULL));
  ENSURE(sqlite3_prepare_v2(pOps->db, SQL_DELETE_FILE, -1, &g_delete_file_stmt, NULL));
  ENSURE(nftw(filename, g_synops_insert_file, 0, 0));

  while (sqlite3_step(g_get_files_stmt) == SQLITE_ROW) {
    const char *fpath = (const char *)(sqlite3_column_text(g_get_files_stmt, 0));
    if (access(fpath, F_OK)) {
      ENSURE(sqlite3_reset(g_delete_file_stmt));
      ENSURE(sqlite3_bind_text(g_delete_file_stmt, 1, fpath, -1, NULL));
      ENSURE_DONE(sqlite3_step(g_delete_file_stmt));
    };
  }

  ENSURE(sqlite3_finalize(g_insert_file_stmt));
  ENSURE(sqlite3_finalize(g_file_exists_stmt));
  ENSURE(sqlite3_finalize(g_get_files_stmt));
  ENSURE(sqlite3_finalize(g_delete_file_stmt));

  return 0;
}
