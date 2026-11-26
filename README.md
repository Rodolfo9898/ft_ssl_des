*This project has been created as part of the 42 curriculum by rperez-t.*

# ft_ssl [base64] [des]

## Description

Symmetric encoding and encryption from scratch. `ft_ssl` recodes part of OpenSSL: the
**base64** encoding and the **DES** cipher in ECB and CBC mode, with keys given in
hexadecimal or derived from a password the way `openssl enc` does. It is the second ft_ssl
project and builds upon the first one, [ft_ssl_md5](https://github.com/Rodolfo9898/ft_ssl_md5):
`md5` and `sha256` are still there, and come from it (see *How the libraries are handled*).

Everything it writes can be read by OpenSSL, and everything OpenSSL writes can be read by
it: the tests compare their output byte for byte.

| Part | Program | What it does |
|---|---|---|
| Mandatory | `ft_ssl` | `md5`, `sha256`, `base64`, `des` (= `des-cbc`), `des-ecb`, `des-cbc` |
| Bonus | `ft_ssl_bonus` | The same, plus `des-ofb`, Triple DES (`des3` = `des3-cbc`, `des3-ecb`, `des3-cbc`, `des3-ofb`), and ft_ssl_md5's bonus: `whirlpool` and commands read from the standard input |

### base64

| Flag | Effect |
|---|---|
| `-e` | Encode (the default) |
| `-d` | Decode: spaces, tabs and newlines anywhere are skipped |
| `-i file` | Read `file` instead of the standard input |
| `-o file` | Write `file` instead of the standard output |

The output is in lines of 64 characters, like `openssl base64`.

### des, des-ecb, des-cbc

| Flag | Effect |
|---|---|
| `-e` / `-d` | Encrypt (the default) / decrypt |
| `-a` | Base64: encode the output when encrypting, decode the input when decrypting |
| `-i file`, `-o file` | Input and output files |
| `-k key` | The key in hex, like `openssl -K`: a short key is padded with zeros (`FF1` is `FF10000000000000`), a long one is cut |
| `-v iv` | The IV in hex, like `openssl -iv`. Needed with `-k` in every mode but ECB (`iv undefined` otherwise, as OpenSSL) |
| `-p password` | The password, like `openssl -pass pass:...` |
| `-s salt` | The salt in hex, like `openssl -S` |

Without `-k` nor `-p`, the password is asked on the terminal without being shown
(`getpass`), twice when encrypting, like OpenSSL. From a password and a salt, the key and
the IV are made with OpenSSL's PBKDF: `EVP_BytesToKey` with SHA-256 and one iteration
(what `openssl enc` does without `-pbkdf2`). The salt is 8 new random bytes from
`/dev/urandom` every time, written before the ciphertext as `Salted__` and the salt, and
read back from there when decrypting. With `-s`, the salt is the one given and, as in
OpenSSL 3, no header is written.

The padding is OpenSSL's (PKCS#7). A wrong key, a wrong password or a damaged
ciphertext gives `bad decrypt`, and every error says what went wrong on the standard error,
with a status of 1.

## Instructions

### Requirements

A Linux machine with `cc`, `make` and `git`. The first `make` fetches libft and
ft_ssl_md5 (see *How the libraries are handled*), which needs network access, but no GitHub
account or ssh key. The tests also use `valgrind` and `openssl` with its legacy provider
(OpenSSL 3 only has DES there); the Docker image has them all.

### Building

```sh
make        # ft_ssl
make bonus  # ft_ssl_bonus
```

| Rule | Effect |
|---|---|
| `all`, `ft_ssl` | Build `ft_ssl`, relinking only what changed |
| `bonus` | Build `ft_ssl_bonus`: the same files, with `-D BONUS_MODE=1` |
| `debug` | The mandatory build with `-g -fsanitize=address` (run `make fclean` first) |
| `clean` | Remove the objects, including those of libft and ft_ssl_md5 |
| `fclean` | `clean`, and remove the programs, `libft.a` and what ft_ssl_md5 built |
| `re` | `fclean`, then `all` |
| `docker-mandatory` | Run the mandatory checks in Docker |
| `docker-bonus` | Run the bonus checks in Docker |
| `docker-shell` | Open a bash shell in the Docker image, with both programs built |
| `docker-build` | Fetch the submodules and build the image (the three rules above do it for you) |
| `docker-clean` | Remove the Docker image |

### Running

```sh
echo toto | ./ft_ssl base64 -e                  # dG90bwo=
echo "d G9 0bwo =" | ./ft_ssl base64 -d         # toto
echo "one deep secret" | ./ft_ssl des-cbc -a -k 6162636461626364 -v 0011223344556677
                                                # zqYWONX68rWNxl7msIdGC67Uh2HfVEBo
echo "zqYWONX68rWNxl7msIdGC67Uh2HfVEBo" | openssl des-cbc -provider legacy -provider default \
    -d -a -K 6162636461626364 -iv 0011223344556677   # one deep secret
./ft_ssl des-ecb -p "MySuperSecurePassword" -i file -o file.enc
openssl des-ecb -provider legacy -provider default -d -in file.enc -pass pass:MySuperSecurePassword
./ft_ssl foobar                                 # the error, then the list of commands
```

With OpenSSL 3, DES needs `-provider legacy -provider default`, and `openssl ... -K` means
ft_ssl's `-k`.

### ft_ssl_bonus

```sh
echo "mode ofb" | ./ft_ssl_bonus des-ofb -a -k C0FFEE69DEADBEEF -v 0102030405060708
echo "triple" | ./ft_ssl_bonus des3 -a -p pass123 | \
    openssl des-ede3-cbc -provider legacy -provider default -d -a -pass pass:pass123
printf 'md5 -s "42 is nice"\nbase64 -i file\nexit\n' | ./ft_ssl_bonus
```

### Testing

```sh
bash tests/mandatory.sh   # builds ./ft_ssl, then checks it
bash tests/bonus.sh       # builds ./ft_ssl_bonus, then checks it
```

Each prints `PASS` or `FAIL` for every check, and exits with the number of failures. They
check the examples of the subject, that base64 gives the same bytes as `openssl base64`
and every DES command the same bytes as `openssl` with `-K -iv` (for every padding case,
both ways, raw and with `-a`), the passwords both ways and the random salt, every error
(wrong keys, bad padding, invalid base64, bad flags, missing, unreadable or unwritable
files, a full disk), and the leaks with valgrind. Their files go to `test_out/`.

### Testing on Linux with Docker

```sh
make docker-mandatory   # or docker-bonus, or docker-shell
```

The image (Debian, with `build-essential`, `valgrind`, `openssl` and its legacy provider)
builds both programs from the sources as a user that is not root, so that the unreadable
file check means something.

A plain `make` that fails also prints, in yellow, `dependencies missing: run make
docker-shell to have them all inside the docker container` (`mk/hint.mk`). The build runs
as always, in a second `make`, and its exit status is kept: the line only comes after its
errors. Only the plain `make` does it: `make bonus`, `make clean`, `make re`, any other
rule, a Ctrl-C and a `make` inside the container print nothing extra. A mistake in your own
code shows it too, so read the errors above it first. `make docker-shell` gives a shell in
the Linux image, with the toolchain and both programs built; `make docker-clean` removes
the image when you are done.

## How the libraries are handled

Both libraries are git submodules in `libs/`, fetched by the first `make` (`git submodule
update --init --remote --recursive`) and built by their own Makefiles:

* **libs/libft**: the string, memory and output helpers (`libft.a`).
* **libs/ft_ssl_md5**: the previous project. Its Makefile compiles it (`make`, or `make
  bonus` for its bonus objects), and its objects go in an archive,
  `objects/libft_ssl_md5.a`, without its `main.o` and its `dispatch.o`: this project has
  its own `main()` and its own command table, with the cipher commands. From it come
  `md5` and `sha256`, the buffers (`buffer_append`, `read_file`...), the output
  (`write_all`, `print_system_error`), the SHA-256 block function that the password's key
  derivation uses, and in the bonus `whirlpool` and the command reader. It has libft as a
  submodule of its own, for its headers (`--recursive` fetches it); the link uses the one
  of `libs/libft`.

## Project structure

```
Makefile, mk/        the build: colors and helpers, settings, libraries, sources,
                     and hint.mk, the yellow hint of a plain make that fails
include/
  ft_ssl_des.h       the prototypes, by file
  core/des_defines.h every constant, with its meaning (the DES tables entry by entry)
  core/des_enums.h   the modes, the direction, the three keys of Triple DES
  core/des_structs.h the options, a cipher command, the round keys, the tables
  core/des_libs.h    the system headers, libft and ft_ssl_md5
sources/
  main.c             the usage, or the command
  utils/             the command table, the errors, the flags and hex, the files
  base64/            encode, decode, and the base64 command
  des/               the DES block, its round keys and tables, ECB and CBC, the flags,
                     the key from -k or from a password, the salt, the PBKDF
  bonus/             des-ofb and Triple DES (empty without -D BONUS_MODE=1)
libs/                libft and ft_ssl_md5 (git submodules)
tests/               setup.sh, mandatory.sh, bonus.sh
Dockerfile           the Linux test image
```

## Technical choices

**A command is a row of a table.** The command table (`utils/dispatch.c`) gives each
command its function. Every cipher command is then a `t_cipher`: its name, its mode, its
key size, its block function (DES or Triple DES) and its mode function (ECB/CBC or OFB),
all run by the same `run_cipher`. Triple DES and OFB are only new rows: the mandatory
build never names a bonus function.

**The constants have names.** Every size, flag, message and table entry is defined in
`des_defines.h`, with where it comes from (FIPS 46-3 for DES, RFC 4648 for base64). The
tables are a `static const` structure filled with those names, at file scope
(`des/tables.c`): no code includes data.

**OpenSSL is the reference.** The padding, the key padding and truncation, the
`Salted__` header, the PBKDF, the prompts and the error messages are OpenSSL's, and the
tests compare against it rather than against fixed outputs.

**Errors are never silent.** Every `open`, `read`, `write`, `close` and `malloc` is
checked, and every failure says why: `ft_ssl: des-cbc: file: No such file or directory`,
`bad decrypt`, `iv undefined`, `invalid hex value 'ZZ'`... The password and the key are
wiped before being freed.

**The bonus never touches the mandatory build.** `make bonus` compiles every file again
into `objects_bonus/` with `-D BONUS_MODE=1`, and links ft_ssl_md5's bonus objects.

## Resources

**The subject's topics**
* FIPS 46-3, *Data Encryption Standard*: the algorithm and its tables.
* NIST SP 800-38A: the ECB, CBC and OFB modes.
* RFC 4648: base64.
* RFC 2898 and RFC 8018 (PKCS #5); `man enc` and OpenSSL's `EVP_BytesToKey(3)`: the key
  derivation `openssl enc` uses without `-pbkdf2`, and the `Salted__` format.
* `man 4 random`, `man 3 getpass`.

**Tools**
* `openssl` 3 with its legacy provider, as the reference; `valgrind`; `norminette`.

**Use of AI.** An AI assistant (Claude) was used for review,it checked the finished library against the subject and the Norm, pointed out failures and explained why they did not match the subject.
