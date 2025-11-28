# stars

Type "stars" in your terminal and get your history of commands connected like this:

![Constellations](https://www.profounddecisions.co.uk/mediawiki-public/images/a/a8/RelativePositionslores.JPG)

Create a constellation of you historical commands and learn from your mistakes in a graphical way. Also, it looks kinda cool. Maybe you want to use it as a screensaver, or maybe you just want to stare at it and pretend you’re an astronomer. I don’t care.

## Current summary
A Linux terminal application that transforms your shell history into ASCII constellations.
- Each base command (e.g., ls, git) becomes a central star
- Its variants radiate diagonally like branches of a constellation.
- Multiple constellations are arranged side by side and stacked vertically to create a galaxy of commands.
- This is not just a tree printer—it’s a mini ASCII graphic engine for terminal layouts.

## Goals
- Linux-only so far, ASCII-only output.
- Shell-agnostic: read history files, not shell builtins.
- Configurable via JSON (grouping, sizing, layout).
- Scales to 100k history lines efficiently.

## Layout Concept (Mini Graphic Engine)
Constellations per base command:
Root star = base command (for example <ls>).
Variants radiate diagonally (/ and \) with branch length proportional to score.

### Galaxy arrangement:
- Multiple constellations placed horizontally and vertically to populate terminal space effitienly.
- Avoid large blank spaces; stagger groups for balance.

### Compact labels:
- Truncate to ≤10 chars by default: <ls --col> instead of full --color=never.

### Collision avoidance:
Use a 2D buffer to track occupied cells.
Nudge labels down if collision detected.

### Spacing rules:
- Horizontal gap between groups: 8–12 columns.
- Vertical gap between rows: 4–6 lines.

### Example Galaxy 1.0:
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

### Canvas:
- Represented as std::vector<std::string> rows.
- Width = terminal width (via ioctl), height = computed dynamically.

### Branch placement:
- Root placed at left margin of its allocated band.
- Branch slopes alternate: / and \.
- Length = score (clamped).

### Collision handling:
Before placing a label, check buffer cell; if occupied, nudge down.

### Galaxy arrangement:
Compute horizontal and vertical gaps.
Place constellations in rows until width exhausted, then wrap.

## Core Rules
- Grouping: base → exact variants (no normalization).
- Duplicates: skip identical commands; count frequency.
- Pipelines: treat as single variant.
- Sizing current formula:
    - score = tokens + flags + 2×frequency + 0.25×char_length
- clamp [2, 20]
- Config file: ~/.config/stars/config.json or default in resources/.

 ## Tech Stack
- C++20, GCC, CMake (≥3.22) fetching dependencies.
- Boost: filesystem, regex, program_options.
- GoogleTest for unit tests.

## Recommended style
- Header guards: use pragma once
- Namespaces: Stars
- Indentation: 2 spaces
- Naming style for Files, Variables, constants, classes, etc:  Java style
- Pointers: smart pointers
- Comments: concise doxygen style with ///
- error handling: exceptions

## Milestones
1. Skeleton (main.cpp, --help, CMake).
2. History reader (auto-detect, parse lines).
3. Tokenizer & index (base + variants).
4. Config loader & sizing.
5. Layout engine folder:
    - ascii_canvas (buffer ops)
    - constellation_planner (branch logic)
    - renderer_ascii (final print)

## Development dependencies
- cmake
- gdb
- gcc
