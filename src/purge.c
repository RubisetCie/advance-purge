/*
 * Provides functions to purge the directories.
 *
 * Author: Matthieu 'Rubisetcie' Carteron
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <fts.h>

#include "purge.h"

/*static const char* SHARE_SYSTEM_DIR = "/usr/share/";*/
static const char* SHARE_SYSTEM_DIR = "/home/rubis/Documents/";
static const char* SHARE_LOCAL_DIR = "/usr/local/share/";
static const char* LOCALE_DIR = "locale";

/* Delete a directory with its files recursively */
static int deleteDir(const char *dir)
{
	int retval = 1;
	FTS *ftsp = NULL;
	FTSENT *cur;

	/* Cast needed although "fts_open" does not modify the argument */
	char *files[] = { (char*) dir, NULL };

    if (!(ftsp = fts_open(files, FTS_NOCHDIR | FTS_PHYSICAL | FTS_XDEV, NULL)))
	{
		fprintf(stderr, "Error while opening file hierarchy traverser: %s\n", strerror(errno));
		retval = 0;
		goto END;
	}

	while ((cur = fts_read(ftsp)))
	{
		switch (cur->fts_info)
		{
			case FTS_NS:
			case FTS_DNR:
			case FTS_ERR:
				fprintf(stderr, "Error while opening file: %s\n", strerror(cur->fts_errno));
				break;
			case FTS_F:
			case FTS_DP:
			case FTS_SL:
			case FTS_SLNONE:
			case FTS_DEFAULT:
				if (remove(cur->fts_accpath) < 0)
				{
					fprintf(stderr, "Error while remove file: %s\n", strerror(cur->fts_errno));
					retval = 0;
				}
				break;
		}
	}

END:
	if (ftsp)
		fts_close(ftsp);

	return retval;
}

/* Purge the locale directory ("/usr/share/locale") */
void purgeLocale(Config *config, int local, int verbose)
{
	DIR *d;
	char path[128];
	struct dirent *dir;

	/* Construct the directory path */
	strcpy(path, local == 1 ? SHARE_LOCAL_DIR : SHARE_SYSTEM_DIR);
	strcat(path, LOCALE_DIR);

	/* Open the directory */
	if ((d = opendir(path)) == NULL)
	{
		fprintf(stderr, "Error while opening the locale directory: %s\n", strerror(errno));
		return;
	}

	/* Change the working directory */
	if (chdir(path) != 0)
	{
		fputs("Error while setting the working directory\n", stderr);
		goto END;
	}

	/* Delete the directories */
	while ((dir = readdir(d)) != NULL)
	{
		/* Avoid deleting current and parent */
		if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
			continue;

		/* Avoid deleting the configured locales */
		if (configCheck(config, dir->d_name))
			continue;

		if (verbose)
			printf("Deleting: %s\n", dir->d_name);

		if (!deleteDir(dir->d_name))
			fputs("Error removing the directory\n", stderr);
	}

END:
	closedir(d);
}

void purgeCupsTemplates(int local, int verbose)
{
}

void purgeGnomeHelp(int local, int verbose)
{
}

void purgeKdeHelp(int local, int verbose)
{
}

void purgeHelp(int local, int verbose)
{
}

/* Delete the locale directory ("/usr/share/locale") */
void deleteLocale(int local, int verbose)
{
	char path[128];

	/* Construct the directory path */
	strcpy(path, local == 1 ? SHARE_LOCAL_DIR : SHARE_SYSTEM_DIR);
	strcat(path, LOCALE_DIR);

	if (verbose)
		printf("Deleting: %s\n", path);

	/* Delete the directory */
	if (!deleteDir(path))
		fputs("Error removing the directory\n", stderr);
}

void deleteCupsTemplates(int local, int verbose)
{
}

void deleteGnomeHelp(int local, int verbose)
{
}

void deleteKdeHelp(int local, int verbose)
{
}

void deleteHelp(int local, int verbose)
{
}
