/*
 * Provides functions to purge the directories.
 *
 * Author: Matthieu 'Rubisetcie' Carteron
 */

#ifndef PURGE_H_INCLUDED
#define PURGE_H_INCLUDED

#include "config.h"

void purgeLocale(const Config *config, int local, int verbose);
void purgeCups(const Config *config, int local, int verbose);
void purgeManual(const Config *config, int local, int verbose);
void purgeHelp(const Config *config, int local, int verbose);

void deleteLocale(int local, int verbose);
void deleteCups(int local, int verbose);
void deleteManual(int local, int verbose);
void deleteHelp(int local, int verbose);
void deleteDoc(int local, int verbose);

#endif
