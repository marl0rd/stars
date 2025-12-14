# stars

Type "stars" in your terminal and get your history of commands connected like this:

![Constellations](https://www.profounddecisions.co.uk/mediawiki-public/images/a/a8/RelativePositionslores.JPG)

Create a constellation of you historical commands and learn from your mistakes in a graphical way. Also, it looks kinda cool. Maybe you want to use it as a screensaver, or maybe you just want to stare at it and pretend you’re an astronomer. I don’t care.

## This project is just starting... Development instructions:
1. `Install build tools`: cmake, gdb, gcc, conan
2. `Clone the repo and cd to it`: git clone ....
3. `Download repo dependencies`: conan install . --output-folder=build --build=missing --settings=build_type=Debug
4. `build it`: cmake --build build/build/Release
5. `Run it`: build/build/Debug/stars

## Current summary
- Each base command (e.g., ls, git) becomes a central star. Let's say "ls"
- Its variants radiate diagonally like branches of a constellation. "ls -l", "ls -a". that is if the flag is different.
- If the flag specializes more, it follows the chronological order and keep expanding. let's say "ls -l", "ls -lr"
- Multiple constellations are arranged side by side and stacked vertically to create a galaxy of commands.
- It’s a mini ASCII graphic engine for terminal layouts.
- Duplicates: skip identical commands; count frequency.
- Currently only available for bash.

### Example Goal for Galaxy 1.0:
``` bash
                          * <ls -ta>                 *--------<ping 255.25..>
                        /                           /
            * <ls -t> *                 <ping>-----*
          /            \                            \
<ls>----* <ls -r>       \                            \
          \               * <ls -tal>                 *---<ping 255.3...>
           \             /
            * <ls -al> *         * <cd /hom>
                                /
                      <cd>----* <cd ..>            <nano>*--------*<nano .>
                                \                        |
                                 * <cd -P>               |
                                                         *<nano ~.config>

<rm>----* <rm file>------* <rm -r file>             * <git stat>
          \                                        /
           * <rm *>                      <git>----* <git log>------*<git log --pretty>
                                                   \
                                                    * <git add>


<echo>---* <echo hello>------* <echo hello world>   <pwd>----*<pwd>
          \                                                   \
           * <echo $VAR>                                       * <pwd -L>
```

## Recommended style
- Header guards: use pragma once
- Namespaces: stars. The cpp will use "using namespace stars"
- Indentation style: Google
- Naming style for Files, Variables, constants, classes, etc: Java style with Clean Code standards.
- Memory Mangement: std:: smart pointers.
- Structure: 
  - include and src files for separating header from definition. No subfolders.
  - When developing a class, make 1 class for a global idea and use it as depicted in Clean Code and Effective C++ books. Avoid doing like having a class called CommandIndex, another one called CommandTokenizer, etc. Just have 1 class and specialize it. Classes can be large (we skip the clean code standard here), but the method considerable.
- Comments: concise doxygen style with /// but just brief for class and public members.
- error handling: exceptions. No returning error code.
- C++ std::20 and Boost 1.89.

