/*
 * Provides functions to read config file.
 *
 * Author: Matthieu 'Rubisetcie' Carteron
 */

#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include <stdio.h>

/* Type of purge to proceed */
typedef enum PurgeType
{
	OFF,
	ON,
	FILTER
} PurgeType;

/* Configuration structure */
typedef struct Locale
{
	char *locale;
	struct Locale *next;
} Locale;

/* Configuration structure */
typedef struct Config
{
	PurgeType purgeLocale;
	PurgeType purgeManual;
	PurgeType purgeCups;
	PurgeType purgeHelp;
	PurgeType purgeDoc;
	struct Locale *locale;
} Config;

int configRead(FILE *file, Config *config, int verbose);
int configCheck(const Config *config, const char *locale);

void localeFree(Locale *locale, int verbose);

#endif
