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

/*static const char* SHARE_SYSTEM_DIR = "/home/rubis/Documents/";*/
static const char* SHARE_SYSTEM_DIR = "/usr/share/";
static const char* SHARE_LOCAL_DIR = "/usr/local/share/";

static const char* LOCALE_DIR = "locale";
static const char* MANUAL_DIR = "man";
static const char* HELP_DIR = "help";
static const char* CUPS_DIR = "cups/";
static const char* CUPS_TEMPLATES_DIR = "templates";
static const char* CUPS_LOCALE_DIR = "locale";
static const char* CUPS_DOCROOT_DIR = "doc-root";
static const char* DOC_DIR = "doc";

/* Check if the directory is excluded */
static int checkExclude(const char *dir, const char **exclude, size_t length)
{
	size_t i;
	for (i = 0; i < length; i++)
	{
		if (strcmp(dir, exclude[i]) == 0)
			return 1;
	}
	return 0;
}

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
				break;
			case FTS_F:
			case FTS_DP:
			case FTS_SL:
			case FTS_SLNONE:
			case FTS_DEFAULT:
				if (remove(cur->fts_accpath) < 0)
					retval = 0;
				break;
		}
	}

END:
	if (ftsp)
		fts_close(ftsp);

	return retval;
}

/* Filter the directories */
static void filterDirectory(const Config *config, const char *path, const char **exclude, size_t excludeLen, int verbose)
{
	DIR *d;
	struct dirent *dir;

	/* Open the directory */
	if ((d = opendir(path)) == NULL)
	{
		fprintf(stderr, "Error while opening the directory %s: %s\n", path, strerror(errno));
		return;
	}

	/* Change the working directory */
	if (chdir(path) != 0)
	{
		fputs("Error while setting the working directory %s\n", stderr);
		goto END;
	}

	/* Delete the directories */
	while ((dir = readdir(d)) != NULL)
	{
		/* Delete only directories */
		if (dir->d_type != DT_DIR)
			continue;

		/* Avoid deleting current and parent and "C" locale dir */
		if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0 || strcmp(dir->d_name, "C") == 0)
			continue;

		/* Avoid deleting excluded directories */
		if (exclude != NULL)
		{
			if (checkExclude(dir->d_name, exclude, excludeLen))
				continue;
		}

		/* Avoid deleting the configured locales */
		if (configCheck(config, dir->d_name))
			continue;

		if (verbose)
			printf("Deleting: %s/%s\n", path, dir->d_name);

		if (!deleteDir(dir->d_name))
			fputs("Error removing the directory\n", stderr);
	}

END:
	closedir(d);
}

/* Remove a directory and its content */
static void removeDirectory(const char *path, int verbose)
{
	if (verbose)
		printf("Deleting: %s\n", path);

	/* Delete the directory */
	if (!deleteDir(path))
		fputs("Error removing the directory\n", stderr);
}

/* Purge the locale directory ("/usr/share/locale") */
void purgeLocale(const Config *config, int local, int verbose)
{
	char path[128];

	/* Construct the directory path */
	strcpy(path, local == 1 ? SHARE_LOCAL_DIR : SHARE_SYSTEM_DIR);
	strcat(path, LOCALE_DIR);

	filterDirectory(config, path, NULL, 0, verbose);
}

void purgeCups(const Config *config, int local, int verbose)
{
	char path[128];

	/* Construct the template directory path */
	strcpy(path, local == 1 ? SHARE_LOCAL_DIR : SHARE_SYSTEM_DIR);
	strcat(path, CUPS_DIR);
	strcat(path, CUPS_TEMPLATES_DIR);

	filterDirectory(config, path, NULL, 0, verbose);

	/* Construct the locale directory path */
	strcpy(path, local == 1 ? SHARE_LOCAL_DIR : SHARE_SYSTEM_DIR);
	strcat(path, CUPS_DIR);
	strcat(path, CUPS_LOCALE_DIR);

	filterDirectory(config, path, NULL, 0, verbose);

	/* Construct the doc-root directory path */
	strcpy(path, local == 1 ? SHARE_LOCAL_DIR : SHARE_SYSTEM_DIR);
	strcat(path, CUPS_DIR);
	strcat(path, CUPS_DOCROOT_DIR);

	filterDirectory(config, path, NULL, 0, verbose);
}

void purgeManual(const Config *config, int local, int verbose)
{
	char path[128];
	const char *exclude[9] =
	{
		"man1",
		"man2",
		"man3",
		"man4",
		"man5",
		"man6",
		"man7",
		"man8",
		"man9"
	};

	/* Construct the directory path */
	strcpy(path, local == 1 ? SHARE_LOCAL_DIR : SHARE_SYSTEM_DIR);
	strcat(path, MANUAL_DIR);

	filterDirectory(config, path, (const char**)exclude, 9, verbose);
}

void purgeHelp(const Config *config, int local, int verbose)
{
	char path[128];
	const char *exclude[1] =
	{
		"C"
	};

	/* Construct the directory path */
	strcpy(path, local == 1 ? SHARE_LOCAL_DIR : SHARE_SYSTEM_DIR);
	strcat(path, HELP_DIR);

	filterDirectory(config, path, exclude, 1, verbose);
}

/* Delete the locale directory ("/usr/share/locale") */
void deleteLocale(int local, int verbose)
{
	char path[128];

	/* Construct the directory path */
	strcpy(path, local == 1 ? SHARE_LOCAL_DIR : SHARE_SYSTEM_DIR);
	strcat(path, LOCALE_DIR);

	removeDirectory(path, verbose);
}

void deleteCups(int local, int verbose)
{
	char path[128];

	/* Construct the directory path */
	strcpy(path, local == 1 ? SHARE_LOCAL_DIR : SHARE_SYSTEM_DIR);
	strcat(path, CUPS_DIR);
	strcat(path, CUPS_LOCALE_DIR);

	removeDirectory(path, verbose);
}

void deleteManual(int local, int verbose)
{
	char path[128];

	/* Construct the directory path */
	strcpy(path, local == 1 ? SHARE_LOCAL_DIR : SHARE_SYSTEM_DIR);
	strcat(path, MANUAL_DIR);

	removeDirectory(path, verbose);
}

void deleteHelp(int local, int verbose)
{
	char path[128];

	/* Construct the directory path */
	strcpy(path, local == 1 ? SHARE_LOCAL_DIR : SHARE_SYSTEM_DIR);
	strcat(path, HELP_DIR);

	removeDirectory(path, verbose);
}

void deleteDoc(int local, int verbose)
{
	char path[128];

	/* Construct the directory path */
	strcpy(path, local == 1 ? SHARE_LOCAL_DIR : SHARE_SYSTEM_DIR);
	strcat(path, DOC_DIR);

	removeDirectory(path, verbose);
}
