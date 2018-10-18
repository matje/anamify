# anamify

A tool that looks for ANAME records in your zone and expand the zone with the target address records,

The specification is a work in progress: https://github.com/each/draft-aname

## Mode of operation

1. Provide a zone name that you want to perform ANAME resolution for.
2. Look for ANAME RRset, perform validation checks.
3. For each ANAME RRset, resolve the A and AAAA RRset for the target.
4. If resolution is successful, rename owner name of target RRsets with ANAME owner name.
4. Add target RRsets to the zone.

This could be improved to actually replace the sibling A and AAA RRset on the
owner name of ANAME.

## Build

First install ldns. Then you can do:

```
autoconf
./configure
make
LD_LIBRARY_PATH=/path/to/lib
export LD_LIBRARY_PATH
```

Now you can run `./anamify <zonefile>`.

## ANAME has no number yet

Since ANAME is a draft in the IETF, it has no assigned number yet. I use the
private RR type TYPE65533 for now.
