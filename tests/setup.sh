#!/bin/bash
# Shared helpers, sourced by tests/mandatory.sh and tests/bonus.sh. Logs and the
# files the checks use go to test_out/. Run from the root of the repository:
# each script builds the program it tests itself (make for ./ft_ssl, make bonus
# for ./ft_ssl_bonus, which bonus.sh sets in SSL).

# Bytes, not characters.
export LC_ALL=C

ROOT=$(pwd)
OUT=$ROOT/test_out
SSL=$ROOT/ft_ssl
FAILS=0
rm -rf "$OUT"
mkdir -p "$OUT"

# OpenSSL 3 only has DES in its legacy provider (ft_ssl_des.pdf p.7).
LEGACY=(-provider legacy -provider default)

check() { # $1 = condition result (0 = ok), $2 = description
	if [ "$1" -eq 0 ]; then
		echo "  PASS  $2"
	else
		echo "  FAIL  $2"
		FAILS=$((FAILS + 1))
	fi
}

note() { echo "  NOTE  $1"; }

build() { # $1 = the make rule: all or bonus, which builds $SSL; stops the script when it fails
	make --no-print-directory "$1" >"$OUT/make_$1.log" 2>&1
	check $? "make $1 builds ./$(basename "$SSL")"
	if [ ! -x "$SSL" ]; then
		echo "### no ./$(basename "$SSL"): the other checks cannot run (see $OUT/make_$1.log)"
		exit "$FAILS"
	fi
}

# What ft_ssl prints against what is expected; the difference is shown when
# they differ.
same() { # $1 = description, $2 = expected, $3 = got
	if [ "$2" == "$3" ]; then
		check 0 "$1"
	else
		check 1 "$1"
		diff <(echo "$2") <(echo "$3") | sed 's/^/        /' | head -20
	fi
}

# Two files must be byte for byte the same.
same_file() { # $1 = description, $2 = expected file, $3 = got file
	cmp -s "$2" "$3"
	check $? "$1"
}

# Runs a command whose input is given, and sets CODE, STDOUT and STDERR.
run_with() { # $1 = file for stdin, the rest = the command
	local input=$1
	shift
	timeout 10 "$@" <"$input" >"$OUT/run.out" 2>"$OUT/run.err"
	CODE=$?
	STDOUT=$(cat "$OUT/run.out")
	STDERR=$(cat "$OUT/run.err")
}

# An error: a status that is not 0, nothing on stdout, and the message on stderr.
fails_with() { # $1 = description, $2 = expected text in stderr, $3 = file for stdin, the rest = the command
	local desc=$1 text=$2
	shift 2
	run_with "$@"
	[ "$CODE" -ne 0 ] && [ "$CODE" -ne 124 ] && [ -z "$STDOUT" ] \
		&& grep -qF -- "$text" "$OUT/run.err"
	check $? "$desc -> ${STDERR%%$'\n'*} (status $CODE)"
}

# valgrind: no error, no leak of any kind, every block freed at exit.
leak_check() { # $1 = description, $2 = file for stdin, the rest = the command
	local desc=$1 input=$2
	shift 2
	if ! command -v valgrind >/dev/null; then
		note "valgrind is missing: $desc skipped"
		return
	fi
	valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all \
		--error-exitcode=42 --log-file="$OUT/leaks.log" "$@" <"$input" >/dev/null 2>&1
	[ $? -ne 42 ] && grep -q "ERROR SUMMARY: 0 errors" "$OUT/leaks.log" \
		&& grep -qE "in use at exit: 0 bytes|no leaks are possible" "$OUT/leaks.log"
	check $? "$desc"
}

# The sizes every cipher check goes through: every padding case, then larger.
SIZES="$(seq 0 33) 63 64 65 1000 4096 65537"

if ! openssl des-ecb "${LEGACY[@]}" -K 00 -in /dev/null >/dev/null 2>&1; then
	echo "### openssl cannot run DES here (its legacy provider is missing): use the Docker image"
	exit 1
fi
