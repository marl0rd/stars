# stars

Type "stars" in your terminal and get your history of commands connected like this:

![Constellations](https://www.profounddecisions.co.uk/mediawiki-public/images/a/a8/RelativePositionslores.JPG)

Create a constellation of you historical commands and learn from your mistakes in a graphical way. Also, it looks kinda cool. Maybe you want to use it as a screensaver, or maybe you just want to stare at it and pretend you’re an astronomer. I don’t care.

## This project is just starting... Development instructions:
1. Install cmake, gdb, gcc, conan
2. Clone the repo and cd to it
3. conan install . --output-folder=build --build=missing
4. cmake --build build/build/Release
5. cd build/build/Release
6. ./stars --help

## Current summary
- Each base command (e.g., ls, git) becomes a central star
- Its variants radiate diagonally like branches of a constellation.
- Multiple constellations are arranged side by side and stacked vertically to create a galaxy of commands.
- This is not just a tree printer—it’s a mini ASCII graphic engine for terminal layouts.

### Example Goal for Galaxy 1.0:
``` bash
                             * <ls -la>                          *--------<ping 255.2...>
                          /                                     /
            * <ls -l>   /                 <ping 255.2...>-----*
          /            *                                       \
<ls>----* <ls -a>       \                                       \
          \               * <ls --col>        <ping 255.3...>----*---<ping 255.3...>
           \             /
            * <ls -al>           * <cd /hom>
                                /
                      <cd>----* <cd ..>                              <nano>
                                \                                       |
                                 * <cd -P>                              |
                                                                        *<nano ~.config>

<rm>----* <rm *>------* <rm -r>------* <rm -r .>         * <git stat>
          \                                             /
           * <rm file>                      <git>----* <git log>
                                                      \
                                                       * <git add>


<echo>---* <echo hi>------* <echo bye>                <pwd>----* <pwd>
           \                                             \
            * <echo $VAR>                                 * <pwd -L>
```

## Current Rules
- Grouping: base → exact variants (no normalization).
- Duplicates: skip identical commands; count frequency.
- Pipelines: treat as single variant.
- Sizing current formula:
    - score = tokens + flags + 2×frequency + 0.25×char_length
- clamp [2, 20]
- Config file: ~/.config/stars/config.json or default in resources/.

## Recommended style
- Header guards: use pragma once
- Namespaces: stars
- Indentation: 2 spaces
- Naming style for Files, Variables, constants, classes, etc:  Java style
- Pointers: smart pointers
- Comments: concise doxygen style with ///
- error handling: exceptions
