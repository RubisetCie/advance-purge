# Advance Purge

**Advance Purge** is a small software that automatically removes files and folder that are dispensable (notably unused locales).

It can be triggered automatically as a "post-install" script by APT, or manually by running `advancepurge`.

## Building

Building *advance-purge* can be done using GNU Make:

```
make
```

## Install

To install *advance-purge*, run the following target:

```
make install PREFIX=(prefix)
```

The variable `PREFIX` defaults to `/usr/local`.

## Uninstall

To uninstall *advance-purge*, run the following target using the same prefix as specified in the install process:

```
make uninstall PREFIX=(prefix)
```

To reinstall all the packages which *advance-purge* has been taking care of before, you can use the following command:

```
apt-get -u --reinstall --fix-missing install $(dpkg -S LC_MESSAGES | cut -d: -f1 | tr ', ' '\n' | sort -u)
```
