# Befunge

This is quite a fast interpreter for [Befunge-93](https://en.wikipedia.org/wiki/Befunge).

## Features

* 100% compliance with the Befunge-93 specifications
* Step-by-step befunge debugger

## Performance

See `benchmarks.txt`

## How to build:

### Production:

Run your befunge programs at max speed

```
make
```

### Step-By-Step

Print the program state at every command

```
make debug
```

![Debugger](http://owenowen.netsoc.ie/res/befunge/debug.png)

## How to run

```
./befunge <path/to/program.bef>
```

I've included a few great programs for you to try in the `programs` folder
