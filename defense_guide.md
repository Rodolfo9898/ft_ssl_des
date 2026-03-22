# ft_ssl_des Defense Guide

## 1) Scope (Project 2)
This guide is for the `ft_ssl_des` defense, built on top of `ft_ssl_md5`.

Current command families:
- Message Digest: `md5`, `sha256`, `whirlpool`
- Cipher (mandatory): `base64`, `des`, `des-ecb`, `des-cbc`
- Cipher (bonus): `des-ofb`, `des3`, `des3-ecb`, `des3-cbc`, `des3-ofb`

## 2) Quick sanity checks
```sh
./ft_ssl invalid_param
./ft_ssl md5 <<< "42"
./ft_ssl base64 <<< "hello"
./ft_ssl des-cbc -k C0FFEE69DEADBEEF -v 0102030405060708 <<< "hello"
```

Expected:
- Invalid command prints a sensible error + command list.
- Existing digest commands still work.
- Cipher commands are available.

## 3) OpenSSL 3 notes (important during defense)
On OpenSSL 3, DES/3DES often require legacy providers:
```sh
-provider legacy -provider default
```
For password-based legacy comparisons, use:
```sh
-md md5
```

## 4) Mandatory checklist

### A) Base64
```sh
echo "repeat after me 'encoding is not encryption'" | base64
echo "repeat after me 'encoding is not encryption'" | ./ft_ssl base64
diff <(echo -n "bac" | openssl base64) <(echo -n "bac" | ./ft_ssl base64)
echo "All your base are belong to us. You have no chance make your time." | ./ft_ssl base64 | ./ft_ssl base64 -d
```

Flags:
```sh
echo "best game in the franchise" | ./ft_ssl base64 -e -o halo1
./ft_ssl base64 -d -i halo1
```

### B) DES ECB minimum
```sh
export DES_KEY="C0FFEE69DEADBEEF"
echo "foobar" | ./ft_ssl des-ecb -k "$DES_KEY" | openssl des-ecb -provider legacy -provider default -d -K "$DES_KEY"
echo "foobar" | openssl des-ecb -provider legacy -provider default -K "$DES_KEY" | ./ft_ssl des-ecb -d -k "$DES_KEY"

echo "smol" | ./ft_ssl des-ecb -k "$DES_KEY" | openssl des-ecb -provider legacy -provider default -d -K "$DES_KEY"
echo "smol" | openssl des-ecb -provider legacy -provider default -K "$DES_KEY" | ./ft_ssl des-ecb -d -k "$DES_KEY"

echo "testing the key now" | openssl des-ecb -provider legacy -provider default -K "ABCD" | ./ft_ssl des-ecb -d -k "ABCD"
echo "More key tests" | openssl des-ecb -provider legacy -provider default -K "FFFFFFFFFFFFFFFF" | ./ft_ssl des-ecb -d -k "FFFFFFFFFFFFFFFF"
echo "what kind of lock takes no key?" | openssl des-ecb -provider legacy -provider default -K "00000000" | ./ft_ssl des-ecb -d -k "00000000"
```

### C) DES simple flags
Use custom cases for `-a -d -e -i -o`:
```sh
echo "flag test" > in.txt
./ft_ssl des-cbc -e -a -k C0FFEE69DEADBEEF -v 0102030405060708 -i in.txt -o enc.txt
./ft_ssl des-cbc -d -a -k C0FFEE69DEADBEEF -v 0102030405060708 -i enc.txt -o dec.txt
cat dec.txt
```

### D) Password handling
```sh
echo "sample password content" > original.html
echo "password" > password_file

openssl des-ecb -provider legacy -provider default -md md5 -p \
  -in original.html -out ciphertext.html -pass "pass:$(cat password_file)"

SALT=$(openssl des-ecb -provider legacy -provider default -md md5 -p \
  -in original.html -out /tmp/throwaway.bin -pass "pass:$(cat password_file)" \
  2>/dev/null | grep '^salt=' | head -n1 | cut -d= -f2)

./ft_ssl des-ecb -d -i ciphertext.html -o decrypted.html \
  -p "$(cat password_file)" -s "$SALT"

cmp -s original.html decrypted.html && echo OK
```

Note:
- If OpenSSL input contains `Salted__` header, decryption also works without `-s`.

### E) CBC mode
```sh
echo "Make sure they really tell you what is going on 'under the hood' when you use CBC mode, and how it is more secure than ECB" \
| openssl des-cbc -provider legacy -provider default -K "BABE101010FACADE" -iv "77696E6B66616365" \
| ./ft_ssl des-cbc -k "BABE101010FACADE" -v "77696E6B66616365" -d
```

## 5) Bonus checklist

### A) Triple DES (E-D-E)
```sh
echo "triple test" \
| ./ft_ssl des3-cbc -a -k 0123456789abcdeffedcba98765432100011223344556677 -v 0001020304050607 \
| openssl des-ede3-cbc -provider legacy -provider default -d -a \
    -K 0123456789abcdeffedcba98765432100011223344556677 -iv 0001020304050607
```

Password comparison:
```sh
echo "triple pwd" | ./ft_ssl des3-cbc -a -p pass123 > triple.enc
cat triple.enc | openssl des-ede3-cbc -provider legacy -provider default -d -a -md md5 -pass pass:pass123
```

### B) Other block modes
```sh
echo "mode ofb" \
| ./ft_ssl des-ofb -a -k C0FFEE69DEADBEEF -v 0102030405060708 \
| openssl des-ofb -provider legacy -provider default -d -a -K C0FFEE69DEADBEEF -iv 0102030405060708

echo "mode 3ofb" \
| ./ft_ssl des3-ofb -a -k 0123456789abcdeffedcba98765432100011223344556677 -v 0001020304050607 \
| openssl des-ede3-ofb -provider legacy -provider default -d -a \
    -K 0123456789abcdeffedcba98765432100011223344556677 -iv 0001020304050607
```

## 6) Architecture notes (to explain during defense)
- Command dispatch is table-driven (`command_registry`) rather than if/else trees.
- Base64 is shared codec logic used by both `base64` and DES `-a` mode.
- DES engine supports:
  - DES + 3DES (EDE)
  - ECB, CBC, OFB
  - Key input (`-k`) and password-derived key material (`-p`, legacy OpenSSL-compatible flow)
  - Salt handling (`Salted__` format)

## 7) Final defense advice
- Keep terminal history of test commands/results.
- When a mismatch appears, isolate:
  1. mode (`ecb/cbc/ofb`)
  2. key source (`-k` vs `-p`)
  3. transport (`raw` vs `-a`)
  4. provider/digest flags on OpenSSL 3.
