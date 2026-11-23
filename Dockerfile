# Linux test image for ft_ssl_des: builds ft_ssl and ft_ssl_bonus, then runs the
# checks: tests/mandatory.sh, then tests/bonus.sh, both using tests/setup.sh.
#   - every example of the subject must give exactly its output;
#   - base64 must give the same bytes as openssl base64, and des, des-ecb,
#     des-cbc (des-ofb and Triple DES in the bonus) the same bytes as openssl
#     with -K and -iv, both ways, on every padding case;
#   - passwords: openssl must decrypt what ft_ssl encrypts and the other way
#     round, with a random salt that changes every time;
#   - errors (wrong keys, bad padding, invalid base64, bad flags, missing files,
#     a full disk) must be reported, and nothing may leak (valgrind).
# The exit status is the total number of failed checks. The submodules in libs/
# (libft and ft_ssl_md5, which has its own libft) must be checked out first:
# they are copied into the image as they are. The docker-* rules of the
# Makefile do this for you.
#
#   make docker-mandatory                                  # mandatory part only
#   make docker-bonus                                      # bonus part only
#   make docker-shell                                      # bash inside the image
#   make docker-build && docker run --rm ft_ssl_des-test   # mandatory, then bonus
#   make docker-clean                                      # removes the image
FROM debian:trixie

# openssl-provider-legacy: openssl 3 only has DES (and whirlpool) with it.
RUN apt-get update \
	&& DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
		build-essential valgrind openssl openssl-provider-legacy \
	&& rm -rf /var/lib/apt/lists/*

# Not root: a file without read permission must stay unreadable to the checks.
RUN useradd --create-home tester && mkdir /work && chown tester:tester /work
WORKDIR /work
COPY --chown=tester:tester . .
USER tester
RUN make && make bonus

CMD ["bash", "-c", "bash tests/mandatory.sh; m=$?; bash tests/bonus.sh; exit $((m + $?))"]
