/*
 * Entry point for advance purge.
 *
 * Author: Matthieu 'Rubisetcie' Carteron
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "config.h"

/* Configuration file path */
static const char* CONFIG_FILE_DEFAULT = "/etc/advancepurge.conf";

/* Prints the command line help */
static void help(const char *prg)
{
	printf("Usage: %s (options)\n  -h|--help    : Show help.\n  -l|--local   : Purge the '/usr/local' directories.\n  -v|--verbose : More verbose output.\n", prg);
}

int main(int argc, const char **argv)
{
	Config config;
	FILE *confFile;
	unsigned int i = 1;
	int local = 0;
	int verbose = 0;
	int retval = 0;

	/* Checks the arguments */
	while (i < argc)
	{
		const char *arg = argv[i++];

		if (strcmp(arg, "-?") == 0 ||
			strcmp(arg, "-h") == 0 ||
			strcmp(arg, "--help") == 0)
		{
			help(argv[0]);
			return 0;
		}
		else if (strcmp(arg, "-l") == 0 ||
			strcmp(arg, "--local") == 0)
			local = 1;
		else if (strcmp(arg, "-v") == 0 ||
			strcmp(arg, "--verbose") == 0)
			verbose = 1;
	}

	/* Open the configuration file if exists */
	if ((confFile = fopen(CONFIG_FILE_DEFAULT, "r")) == NULL)
	{
		fprintf(stderr, "Error while opening the config: %s\n", strerror(errno));
		return 2;
	}

	/* Initialize the configuration */
	config.locale = malloc(sizeof(Locale));
	config.locale->locale = NULL;
	config.locale->next = NULL;
	config.purgeLocale = FILTER;
	config.purgeManual = FILTER;
	config.purgeCups = FILTER;
	config.purgeHelp = FILTER;
	config.purgeDoc = ON;

	/* Read the configuration file */
	if (!configRead(confFile, &config, verbose))
		retval = 3;

	/* Close the file */
	fclose(confFile);

	/* Proceed to the purge */
	if (retval == 0)
	{
		if (config.purgeLocale == FILTER)
			purgeLocale(&config, local, verbose);
		else if (config.purgeLocale == ON)
			deleteLocale(local, verbose);

		if (config.purgeCups == FILTER)
			purgeCups(&config, local, verbose);
		else if (config.purgeCups == ON)
			deleteCups(local, verbose);

		if (config.purgeManual == FILTER)
			purgeManual(&config, local, verbose);
		else if (config.purgeManual == ON)
			deleteManual(local, verbose);

		if (config.purgeHelp == FILTER)
			purgeHelp(&config, local, verbose);
		else if (config.purgeHelp == ON)
			deleteHelp(local, verbose);

		if (config.purgeDoc != OFF)
			deleteDoc(local, verbose);
	}

	/* Frees the configuration structure */
	configFree(config.locale, verbose);

	return retval;
}
