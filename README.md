# md2

Clone of the cracked, obsolete but easy to implement Message-Digest-2 hashing algorithm.

## Motivation

- Additional Task in my Information Security module
- Science and fun (no explanation needed)

## Installation
1. `git clone https://bitbucket.org/evilc00kie/md2`
2. cd md2
3. make

Tested on Windows 10 and Debian 8.

## Usage

Hash a single string

`echo -n "I feel so hashy 2day." | md2`
	
Hash whole file

`md2 debian.iso`

Test hashing function

`md2 -t`

Show help

`md2 -h`
## Reference

- RFC of the MD2-Algorithm with C Codeexample: http://www.faqs.org/rfcs/rfc1319.html
- Corrections of the RFC: https://www.rfc-editor.org/errata_search.php?rfc=1319
- Java implementation of the tool: https://www.gnu.org/software/gnu-crypto/ 


# License
LGPL (https://www.gnu.org/copyleft/lesser.html)
