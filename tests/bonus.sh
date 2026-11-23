#!/bin/bash
# Bonus checks (make bonus, then ./ft_ssl_bonus): des-ofb and Triple DES against
# openssl, ft_ssl_md5's bonus (whirlpool, commands from the standard input),
# and the mandatory part still there. Exit status: the number of failed checks.

source "$(dirname "$0")/setup.sh"
SSL=$ROOT/ft_ssl_bonus

echo "### make bonus"
build bonus
cd "$OUT" || exit 1

echo "### the commands of the bonus (ft_ssl_des.pdf p.13)"
INVALID="ft_ssl: Error: 'foobar' is an invalid command.
Standard commands:
Message Digest commands:
md5
sha256
whirlpool
Cipher commands:
base64
des
des-ecb
des-cbc
des-ofb
des3
des3-ecb
des3-cbc
des3-ofb"
same "foobar: the commands, with the bonus ones" "$INVALID" "$($SSL foobar 2>&1 </dev/null)"

echo "### des-ofb and Triple DES against openssl: the same bytes, both ways"
K=C0FFEE69DEADBEEF
K3=0123456789abcdeffedcba98765432100011223344556677
IV=0001020304050607
for pair in des-ofb:des-ofb:$K des3:des3:$K3 des3-ecb:des-ede3-ecb:$K3 \
	des3-cbc:des-ede3-cbc:$K3 des3-ofb:des-ede3-ofb:$K3; do
	IFS=: read -r cmd ossl key <<<"$pair"
	bad=0
	for n in $SIZES; do
		head -c "$n" /dev/urandom >random.bin
		openssl "$ossl" "${LEGACY[@]}" -K "$key" -iv $IV -in random.bin -out expected.bin 2>/dev/null
		$SSL "$cmd" -k "$key" -v $IV -i random.bin -o got.bin
		cmp -s expected.bin got.bin || { bad=$((bad + 1)); echo "        $cmd: $n bytes differ"; }
		$SSL "$cmd" -d -k "$key" -v $IV -i expected.bin | cmp -s - random.bin || bad=$((bad + 1))
	done
	check $bad "$cmd = openssl $ossl: 0 to 65537 bytes"
done
same "des-ofb: as long as its input, no padding" "13" "$(head -c 13 random.bin | $SSL des-ofb -k $K -v $IV | wc -c | tr -d ' ')"
echo "triple pwd" >original
$SSL des3-cbc -a -p pass123 -i original | openssl des-ede3-cbc "${LEGACY[@]}" -d -a -pass pass:pass123 2>/dev/null | cmp -s - original
check $? "des3-cbc -p: openssl decrypts it (a 24-byte key from the password)"
openssl des-ede3-cbc "${LEGACY[@]}" -pass pass:pass123 -in original 2>/dev/null | $SSL des3 -d -p pass123 | cmp -s - original
check $? "des3 -d -p of openssl des-ede3-cbc -pass output"
fails_with "des3-ofb -k without -v" "iv undefined" original $SSL des3-ofb -k $K3

echo "### ft_ssl_md5's bonus: commands from stdin, whirlpool"
same "commands read from stdin, ciphers included" 'MD5 ("42 is nice") = 0029a98ee90fdb85d70924d44d3c9e75
dG90bwo=' "$(printf 'md5 -s "42 is nice"\nbase64 -i toto_file\nexit\n' | (echo toto >toto_file; timeout 5 $SSL 2>&1))"
same 'whirlpool -q -s "abc" (ISO test vector)' \
	'4e2448a4c6f486bb16b6562c73b4020bf3043e3a731bce721ae1b303d97e6d4c7181eebdb6c57e277d0e34957114cbd6c797fc9d95d8b582d225292076d4eef5' \
	"$($SSL whirlpool -q -s abc 2>&1)"

echo "### the mandatory part still works with make bonus"
same "the CBC example of the subject" "zqYWONX68rWNxl7msIdGC67Uh2HfVEBo" \
	"$(echo "one deep secret" | $SSL des-cbc -a -k 6162636461626364 -v 0011223344556677)"
same "echo toto | base64" "dG90bwo=" "$(echo toto | $SSL base64)"

echo "### leaks (valgrind)"
head -c 100 /dev/urandom >random.bin
leak_check "des3-cbc -p" random.bin $SSL des3-cbc -p pw
leak_check "des-ofb -k -v" random.bin $SSL des-ofb -k $K -v $IV
leak_check "des3-ofb -d -a of invalid base64" random.bin $SSL des3-ofb -d -a -k $K3 -v $IV
printf 'des-ecb -k 00 -i original\nfoobar\nexit\n' >session.txt
leak_check "a session of commands from stdin" session.txt $SSL

echo "### bonus: $FAILS failed check(s)"
exit "$FAILS"
