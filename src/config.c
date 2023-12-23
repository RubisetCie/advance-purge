/*
 * Provides functions to read config file.
 *
 * Author: Matthieu 'Rubisetcie' Carteron
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"

#define CAT_OPERATIONS "[Operations]"
#define CAT_LOCALES    "[Locales]"

#define OP_LOCALES     "locale"
#define OP_MANUAL      "manual"
#define OP_CUPS        "cups"
#define OP_HELP        "help"
#define OP_DOC         "doc"

/* Configuration category during reading process */
typedef enum ConfigCategory
{
	UNDEFINED,
	OPERATIONS,
	LOCALES
} ConfigCategory;

/* Checks if the line is a comment starting by '#' (account for blank leading) */
static int isComment(const char *line, ssize_t len)
{
	ssize_t i;
	char c;
	for (i = 0; i < len - 1; i++)
	{
		c = line[i];

		if (isblank(c))
			continue;

		if (c == '#')
			return 1;

		return 0;
	}

	return 0;
}

/* Trim the blank leading */
static void trimLeft(char **line, ssize_t *len)
{
	ssize_t i;
	char c;

	/* Skip the blank leading */
	for (i = 0; i < *len - 1; i++)
	{
		c = (*line)[i];

		if (isblank(c))
			continue;

		break;
	}
	*line = &(*line)[i];
	*len -= i;
}

/* Read the configuration category */
static int readCategory(const char *line, ssize_t len, ConfigCategory *category)
{
#define COMPARE_CATEGORY(line, cat, len, category, val) \
	if (strncmp(line, cat, len - 1) == 0)               \
	{                                                   \
		category = val;                                 \
		return 1;                                       \
	}

	/* Compare the rest of the line to the available categories (include the newline character) */
	COMPARE_CATEGORY(line, CAT_OPERATIONS, len, *category, OPERATIONS);
	COMPARE_CATEGORY(line, CAT_LOCALES, len, *category, LOCALES);

#undef COMPARE_CATEGORY

	return 0;
}

/* Read the operation state */
static void readOperation(const char *line, ssize_t len, Config *config, int verbose)
{
	ssize_t i;

	/* Determine the position of the equal character */
	for (i = 0; i < len; i++)
	{
		if (line[i] == '=')
			break;
	}

#define COMPARE_OPERATION(line, op, len, eq, opt, v)  \
	if (strncmp(line, op, strlen(op)) == 0)           \
	{                                                 \
		size_t j;                                     \
		char c;                                       \
		for (j = eq + 1; j < len - 1; j++)            \
		{                                             \
			c = line[j];                              \
			if (isblank(c))                           \
				continue;                             \
			break;                                    \
		}                                             \
		if (strncmp(&line[j], "on", 2) == 0 ||        \
			strncmp(&line[j], "yes", 3) == 0)         \
			opt = ON;                                 \
		else if (strncmp(&line[j], "filter", 6) == 0) \
			opt = FILTER;                             \
		else                                          \
			opt = OFF;                                \
		if (v)                                        \
			printf("Operation %s: %s\n", op,          \
				opt == ON ? "on" :                    \
				opt == FILTER ? "filter" : "off");    \
		return;                                       \
	}

	/* Test the operation name */
	COMPARE_OPERATION(line, OP_LOCALES, len, i, config->purgeLocale, verbose);
	COMPARE_OPERATION(line, OP_MANUAL, len, i, config->purgeManual, verbose);
	COMPARE_OPERATION(line, OP_CUPS, len, i, config->purgeCups, verbose);
	COMPARE_OPERATION(line, OP_HELP, len, i, config->purgeHelp, verbose);
	COMPARE_OPERATION(line, OP_DOC, len, i, config->purgeDoc, verbose);

#undef COMPARE_OPERATION
}

/* Add the locale name to the list to keep */
static int addLocale(const char *line, ssize_t len, Locale *locale, int verbose)
{
	/* Advance to the last config node */
	while (locale->locale != NULL)
	{
		if (locale->next == NULL)
		{
			locale->next = malloc(sizeof(Locale));
			if (locale->next == NULL)
			{
				fputs("Error while allocating the configuration structure.\n", stderr);
				return 0;
			}
			locale->next->locale = NULL;
			locale->next->next = NULL;
		}
		locale = locale->next;
	}

	/* Append the locale */
	locale->locale = malloc(sizeof(char) * len);
	if (locale->locale == NULL)
	{
		fputs("Error while allocating the configuration locale.\n", stderr);
		return 0;
	}

	memcpy(locale->locale, line, len - 1);
	locale->locale[len - 1] = '\0';

	if (verbose)
		printf("Configuration add: %s\n", locale->locale);

	return 1;
}

/* Read the configuration file and allocate a data structure */
int configRead(FILE *file, Config *config, int verbose)
{
	int retval = 1;
	char *line = NULL;
    size_t len = 0;
	ssize_t read;
	ConfigCategory category = UNDEFINED;

	while ((read = getline(&line, &len, file)) != -1)
	{
		/* If blank line */
		if (read <= 1)
			continue;

		/* If comment */
		if (isComment(line, read))
			continue;

		/* Trim the leading blank characters */
		trimLeft(&line, &read);

		/* Read the category */
		if (readCategory(line, read, &category))
			continue;

		switch (category)
		{
			case OPERATIONS:
				/* Read the operation state */
				readOperation(line, read, config, verbose);
				break;
			case LOCALES:
				/* Add the locale */
				if (!addLocale(line, read, config->locale, verbose))
				{
					retval = 0;
					goto END;
				}
				break;
		}
	}

END:
	free(line);

	return retval;
}

/* Check if a locale is present in the configuration data structure */
int configCheck(const Config *config, const char *locale)
{
	Locale *loc = config->locale;
	do
	{
		if (strcmp(loc->locale, locale) == 0)
			return 1;
	} while ((loc = loc->next) != NULL);
	return 0;
}

/* Free a configuration data structure */
void configFree(Locale *locale, int verbose)
{
	Locale *next = locale->next;

	if (verbose && locale->locale)
		printf("Configuration delete: %s\n", locale->locale);

	free(locale->locale);
	free(locale);

	if (next == NULL)
		return;

	configFree(next, verbose);
}
