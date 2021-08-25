# data-structures

This repository is a place for me to dink around with different data
structures.  They are designed to be easily included into other projects.

This repository is licensed under GPL3.  If you wish to
purchase a different license, email czipperz AT gmail DOT com.

## Building

1. Clone the repository and the submodules.

```
git clone https://github.com/czipperz/data-structures
cd data-structures
git submodule init
git submodule update
```

2. Build data-structures by running (on all platforms):

```
./build-release
```

3.  After building, you can copy the library and shared object
    into your project from `./build/release/data-structures`.
