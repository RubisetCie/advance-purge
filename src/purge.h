/*
 * Provides functions to purge the directories.
 *
 * Author: Matthieu 'Rubisetcie' Carteron
 */

#ifndef PURGE_H_INCLUDED
#define PURGE_H_INCLUDED

#include "config.h"

void purgeLocale(Config *config, int local, int verbose);
void purgeCupsTemplates(int local, int verbose);
void purgeGnomeHelp(int local, int verbose);
void purgeKdeHelp(int local, int verbose);
void purgeHelp(int local, int verbose);

void deleteLocale(int local, int verbose);
void deleteGnomeHelp(int local, int verbose);
void deleteKdeHelp(int local, int verbose);
void deleteHelp(int local, int verbose);

#endif
