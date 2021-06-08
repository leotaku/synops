#ifndef SYNOPS_H
#define SYNOPS_H

typedef struct sqlite3 sqlite3;

typedef struct synops {
  sqlite3 *db;
} synops;

int synops_open(const char *filename, synops **ppOps);
int synops_close(synops *pOps);

int synops_insert_files(synops *pOps, const char *filename);

#endif
