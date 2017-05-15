# AGL Net

AGL Net is a library used to deal with networking in games developed with [AGL](http://github.com/yds12/agl).

## Compiling and Installing AGL Graphics

First install the basic [AGL](http://github.com/yds12/agl) and its dependencies. You will also need to install SDL net (development package). In Ubuntu 16.04 SDL net can be installed via the following:

```
sudo apt-get install libsdl-net1.2-dev
```

Now, open a terminal, `cd` to the directory `src` and type:


```
make install
```

That's it! If you want to use AGL Net for development, you can type `make install_debug` instead, to install it with debug symbols.
