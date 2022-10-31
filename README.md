Volume Icon
===========
A lightweight volume control applet (Qt port)

Compilation
-----------

```raw
  $ ./autogen.sh
  $ ./configure --prefix=/usr
  $ make
  $ sudo make install
```

Compilation Flags
-----------------
```
--with-default-mixerapp: Set the default mixer application, defaults to
                         alsamixer.
```

Build Dependencies
------------------
The following packages must be installed for compilation (Debian names given):
* libasound2-dev
* libglib2.0-dev
* libnotify-dev
* qt5base-dev
* perl (uses `pod2man` to generate man pages)

Contributing
------------
In order to keep coding style consistent (barring third-party code) we use the
`clang-format` command-line tool with the settings specified in
`.clang-format`. To that end, please make sure to run the `format-source.sh`
script before submitting pull requests.
