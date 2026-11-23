#!/bin/bash
# Mandatory checks (make, then ./ft_ssl base64, des, des-ecb, des-cbc, against
# the subject and openssl). Exit status: the number of failed checks.

source "$(dirname "$0")/setup.sh"

echo "### make"
build all
cd "$OUT" || exit 1
echo "this file is in the way" >empty_dir_marker

echo "### usage and invalid commands (ft_ssl_des.pdf p.7)"
INVALID="ft_ssl: Error: 'foobar' is an invalid command.
Standard commands:
Message Digest commands:
md5
sha256
Cipher commands:
base64
des
des-ecb
des-cbc"
same "foobar: the error and the commands of the subject" "$INVALID" "$($SSL foobar 2>&1)"
$SSL foobar >/dev/null 2>&1
check $([ $? -ne 0 ]; echo $?) "foobar: status not 0"
$SSL des-ofb -k 00 -v 00 </dev/null 2>&1 | grep -q "'des-ofb' is an invalid command"
check $? "des-ofb is a bonus command: make does not have it"
same "no command: the usage" "usage: ft_ssl command [flags] [file/string]" "$($SSL 2>&1 </dev/null)"

echo "### ft_ssl_md5 still works (p.7: build upon it)"
same 'echo "42 is nice" | md5' "(stdin)= 35f1d6de0302e2086a4e472266efb3a9" "$(echo "42 is nice" | $SSL md5)"
same 'sha256 -s "42 is nice"' \
	'SHA256 ("42 is nice") = b7e44c7a40c5f80139f0a50f3650fb2bd8d00b0d24667c4c2ca32c88e13b758f' \
	"$($SSL sha256 -s "42 is nice")"

echo "### base64: the examples of the subject (p.9)"
same "echo toto | base64 -e" "dG90bwo=" "$(echo toto | $SSL base64 -e)"
same "echo dG90bwo= | base64 -d" "toto" "$(echo dG90bwo= | $SSL base64 -d)"
same 'echo "d G9 0bwo\n=" | base64 -d (whitespace)' "toto" "$(printf 'd G9 0bwo\n=\n' | $SSL base64 -d)"
same "echo foobar | base64 -e" "Zm9vYmFyCg==" "$(echo foobar | $SSL base64 -e)"
same "echo foobar | base64 (encode is the default)" "Zm9vYmFyCg==" "$(echo foobar | $SSL base64)"
same "ft_ssl's output read by the system's base64 -d" "foobar" "$(echo foobar | $SSL base64 | base64 -d)"
echo "best game in the franchise" | $SSL base64 -e -o halo1
same "-o, then -i" "best game in the franchise" "$($SSL base64 -d -i halo1)"

echo "### base64 against openssl base64, both ways"
bad_enc=0
bad_dec=0
for n in $SIZES 1048576; do
	head -c "$n" /dev/urandom >random.bin
	openssl base64 -in random.bin -out expected.b64
	$SSL base64 -i random.bin -o got.b64
	cmp -s expected.b64 got.b64 || { bad_enc=$((bad_enc + 1)); echo "        encode: $n bytes differ"; }
	openssl base64 -d -in got.b64 | cmp -s - random.bin || bad_dec=$((bad_dec + 1))
	$SSL base64 -d -i expected.b64 | cmp -s - random.bin || bad_dec=$((bad_dec + 1))
done
check $bad_enc "the same output as openssl base64 (lines of 64), 0 bytes to 1 MB"
check $bad_dec "openssl decodes ft_ssl's, ft_ssl decodes openssl's"

echo "### base64 errors"
echo "bad!" >bad.b64
fails_with "base64 -d of invalid input" "invalid base64 input" bad.b64 $SSL base64 -d
printf 'Zm9vY' >cut.b64
fails_with "base64 -d of a cut group" "invalid base64 input" cut.b64 $SSL base64 -d
printf 'Zg==Zg==' >after.b64
fails_with "base64 -d with data after the '='" "invalid base64 input" after.b64 $SSL base64 -d
fails_with "base64 -x" "invalid option '-x'" /dev/null $SSL base64 -x
fails_with "base64 -i without its file" "option requires an argument '-i'" /dev/null $SSL base64 -i
fails_with "base64 -i no_such_file" "ft_ssl: base64: no_such_file: No such file or directory" /dev/null $SSL base64 -i no_such_file
fails_with "base64 -o into a missing folder" "ft_ssl: base64: no_dir/out:" /dev/null $SSL base64 -o no_dir/out

echo "### des-cbc: the example of the subject (p.12)"
same 'echo "one deep secret" | des-cbc -a -k 6162636461626364 -v 0011223344556677' \
	"zqYWONX68rWNxl7msIdGC67Uh2HfVEBo" \
	"$(echo "one deep secret" | $SSL des-cbc -a -k 6162636461626364 -v 0011223344556677)"
same "openssl decrypts it" "one deep secret" \
	"$(echo zqYWONX68rWNxl7msIdGC67Uh2HfVEBo | openssl des-cbc "${LEGACY[@]}" -d -a -K 6162636461626364 -iv 0011223344556677)"

echo "### des, des-ecb, des-cbc against openssl -K -iv: the same bytes, both ways"
K=C0FFEE69DEADBEEF
IV=0102030405060708
for cmd in des-ecb des-cbc des; do
	bad=0
	for n in $SIZES; do
		head -c "$n" /dev/urandom >random.bin
		openssl "$cmd" "${LEGACY[@]}" -K $K -iv $IV -in random.bin -out expected.bin 2>/dev/null
		$SSL "$cmd" -k $K -v $IV -i random.bin -o got.bin
		cmp -s expected.bin got.bin || { bad=$((bad + 1)); echo "        $cmd: $n bytes differ"; }
		$SSL "$cmd" -d -k $K -v $IV -i expected.bin | cmp -s - random.bin || bad=$((bad + 1))
		openssl "$cmd" "${LEGACY[@]}" -d -a -A -K $K -iv $IV -in <($SSL "$cmd" -a -k $K -v $IV -i random.bin) \
			2>/dev/null | cmp -s - random.bin || bad=$((bad + 1))
	done
	check $bad "$cmd: 0 to 65537 bytes, raw and -a"
done
head -c 5000 /dev/urandom >random.bin
$SSL des-cbc -a -k $K -v $IV -i random.bin | openssl des-cbc "${LEGACY[@]}" -d -a -K $K -iv $IV | cmp -s - random.bin
check $? "-a output of 5000 bytes, in lines of 64: openssl -a reads it without -A"
openssl des-cbc "${LEGACY[@]}" -a -K $K -iv $IV -in random.bin | $SSL des-cbc -d -a -k $K -v $IV | cmp -s - random.bin
check $? "ft_ssl -d -a reads openssl's -a output (lines of 64)"

echo "### keys like openssl -K (p.11): short ones padded with zeros, long ones cut"
for k in ABCD FF1 FF12CD 00000000 FFFFFFFFFFFFFFFF 0123456789ABCDEF0011 abcdef; do
	same "des-ecb -k $k" "key test" "$(echo "key test" | openssl des-ecb "${LEGACY[@]}" -K $k 2>/dev/null | $SSL des-ecb -d -k $k)"
done
same "FF1 is FF10000000000000" "$(echo x | $SSL des-ecb -k FF10000000000000 | od -An -tx1)" \
	"$(echo x | $SSL des-ecb -k FF1 | od -An -tx1)"

echo "### CBC: the IV changes after each block (p.12)"
printf 'AAAAAAAAAAAAAAAA' >two_blocks
$SSL des-ecb -k $K -i two_blocks | head -c 16 | od -An -tx1 >ecb.hex
$SSL des-cbc -k $K -v $IV -i two_blocks | head -c 16 | od -An -tx1 >cbc.hex
[ "$(cut -c1-24 ecb.hex)" == "$(cut -c25-48 ecb.hex)" ] && [ "$(cut -c1-24 cbc.hex)" != "$(cut -c25-48 cbc.hex)" ]
check $? "two equal blocks: the same in ECB, different in CBC"

echo "### passwords (p.11): openssl's PBKDF, a random salt, the Salted__ header"
echo "sample password content" >original
openssl des-ecb "${LEGACY[@]}" -in original -out from_openssl -pass pass:password 2>/dev/null
same "des-ecb -d -p of openssl -pass output" "sample password content" "$($SSL des-ecb -d -i from_openssl -p password)"
openssl des-cbc "${LEGACY[@]}" -a -in original -pass pass:password 2>/dev/null | $SSL des -d -a -p password | cmp -s - original
check $? "des -d -a -p of openssl des-cbc -a -pass output"
$SSL des-cbc -p password -i original | openssl des-cbc "${LEGACY[@]}" -d -pass pass:password 2>/dev/null | cmp -s - original
check $? "openssl -d -pass of des-cbc -p output"
$SSL des-ecb -a -p password -i original | openssl des-ecb "${LEGACY[@]}" -d -a -pass pass:password 2>/dev/null | cmp -s - original
check $? "openssl -d -a -pass of des-ecb -a -p output"
salt1=$($SSL des -p x -i original | head -c 16 | od -An -tx1 | tr -d ' \n')
salt2=$($SSL des -p x -i original | head -c 16 | od -An -tx1 | tr -d ' \n')
[ "${salt1:0:16}" == "53616c7465645f5f" ] && [ "${salt1:16}" != "0000000000000000" ] && [ "$salt1" != "$salt2" ]
check $? "Salted__ then a random salt, different each time (${salt1:16} / ${salt2:16})"
for cmd in des-ecb des-cbc; do
	openssl "$cmd" "${LEGACY[@]}" -S 0011223344556677 -pass pass:password -in original -out expected.bin 2>/dev/null
	$SSL "$cmd" -p password -s 0011223344556677 -i original -o got.bin
	same_file "$cmd -p -s: the same bytes as openssl -pass -S" expected.bin got.bin
	same "$cmd -d -p -s of openssl -S output" "sample password content" "$($SSL "$cmd" -d -p password -s 0011223344556677 -i expected.bin)"
done
openssl des-cbc "${LEGACY[@]}" -iv $IV -pass pass:password -in original -out expected.bin 2>/dev/null
same "des-cbc -p -v: the IV of -v, the key of the password" "sample password content" \
	"$($SSL des-cbc -d -p password -v $IV -i expected.bin)"

echo "### errors: a reason on stderr, status not 0"
$SSL des-cbc -k $K -v $IV -i original -o right.bin
fails_with "a wrong key" "bad decrypt" right.bin $SSL des-cbc -d -k 1111111111111111 -v $IV
fails_with "a wrong password" "bad decrypt" from_openssl $SSL des-ecb -d -p wrong
head -c 13 right.bin >cut.bin
fails_with "a length that is not a multiple of 8" "bad decrypt" cut.bin $SSL des-cbc -d -k $K -v $IV
fails_with "-a and invalid base64" "invalid base64 input" bad.b64 $SSL des-cbc -d -a -k $K -v $IV
fails_with "des-cbc -k without -v, like openssl" "iv undefined" original $SSL des-cbc -k $K
fails_with "des -k without -v (des is des-cbc)" "iv undefined" original $SSL des -k $K
run_with original $SSL des-ecb -k $K
[ "$CODE" -eq 0 ] && [ -n "$STDOUT" ]
check $? "des-ecb -k needs no IV"
fails_with "-p decrypting data without Salted__ nor -s" "bad magic number" right.bin $SSL des-cbc -d -p password
fails_with "-k ZZ" "invalid hex value 'ZZ'" original $SSL des -k ZZ -v $IV
fails_with "-k with an empty key" "invalid hex value ''" original $SSL des -k "" -v $IV
fails_with "-v 12G4" "invalid hex value '12G4'" original $SSL des -k $K -v 12G4
fails_with "-s xyz" "invalid hex value 'xyz'" original $SSL des -p pw -s xyz
fails_with "-q" "invalid option '-q'" original $SSL des -q
fails_with "-k without its key" "option requires an argument '-k'" original $SSL des -k
fails_with "-i no_such_file" "ft_ssl: des-cbc: no_such_file: No such file or directory" /dev/null $SSL des-cbc -k $K -v $IV -i no_such_file
mkdir -p a_directory
fails_with "-i a_directory" "ft_ssl: des-ecb: a_directory:" /dev/null $SSL des-ecb -k $K -i a_directory
fails_with "-o into a missing folder" "ft_ssl: des-ecb: no_dir/out:" original $SSL des-ecb -k $K -o no_dir/out
if [ -w /dev/full ]; then
	fails_with "output to a full disk" "ft_ssl: des-ecb: stdout: No space left on device" original \
		sh -c "$SSL des-ecb -k $K >/dev/full"
fi
echo secret >unreadable && chmod 000 unreadable
if [ ! -r unreadable ]; then
	fails_with "-i unreadable" "Permission denied" /dev/null $SSL des-ecb -k $K -i unreadable
else
	note "this user can read a file without read permission (root): the unreadable file check is skipped"
fi
chmod 600 unreadable

echo "### leaks (valgrind)"
$SSL des-cbc -p pw -i original -o salted.bin
leak_check "des-cbc -a -k -v" original $SSL des-cbc -a -k $K -v $IV
leak_check "des-cbc -d -p of Salted__ data" salted.bin $SSL des-cbc -d -p pw
leak_check "des-ecb -p (random salt)" original $SSL des-ecb -p pw
leak_check "a wrong password" salted.bin $SSL des-cbc -d -p wrong
leak_check "invalid base64" bad.b64 $SSL des-cbc -d -a -k $K -v $IV
leak_check "iv undefined" original $SSL des-cbc -k $K
leak_check "a bad option" original $SSL des -q
leak_check "-o into a missing folder" original $SSL des-ecb -k $K -o no_dir/out
leak_check "base64 -d" got.b64 $SSL base64 -d
leak_check "base64 -d of invalid input" bad.b64 $SSL base64 -d
leak_check "foobar" original $SSL foobar
leak_check "no command" original $SSL

echo "### mandatory: $FAILS failed check(s)"
exit "$FAILS"
