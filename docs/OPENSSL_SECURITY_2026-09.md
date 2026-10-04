# OpenSSL security update — RC3

Date: 2026-10-04. Baseline: v0.3.2-rc.2, Windows OpenSSL 3.6.4-1.
Official advisory: https://openssl-library.org/news/secadv/20260929.txt
MSYS2 package: https://packages.msys2.org/packages/mingw-w64-x86_64-openssl
Debian backport: https://security-tracker.debian.org/tracker/CVE-2026-84782

## Scope and remediation
Windows is rebuilt with OpenSSL 3.6.5-1 or a newer 3.6 patch. CI rejects an older package or an unreviewed branch and compares both packaged OpenSSL DLLs with the package files. Installer and portable get fresh SPDX SBOM, SHA-256 and provenance attestations. Other dependency versions are recorded by the build; rolling MSYS2 packages can also change.

Linux RC1 .deb and .tar.xz are retained byte-for-byte: they use dynamic system libraries. Updating Debian's libssl3t64 updates the runtime without recompiling CgPhone. CI installs the old package in clean Debian 13 with security updates, requires libssl3t64 >= 3.5.7-1~deb13u3 (Debian's backport), checks startup and records the runtime packages. The original Linux SBOM remains a historical build inventory; Linux-RC3-runtime-packages.tsv describes this separate validation environment, not every user's installation. Users must update their own system. This does not certify Qt backports.

## Advisory assessment
The reviewed SIP engine creates UDP transport; SRTP is compiled out. No exposed DTLS, QUIC, CMP, SM2 signing or TLS server context-switch path was identified in CgPhone. Presence of OpenSSL alone does not establish exploitability. This is preventive dependency maintenance.

| CVE | Upstream severity | Required entry / impact |
| --- | --- | --- |
| CVE-2026-84782 | High | Suspended DTLS handshake plus retransmission; memory disclosure/crash |
| CVE-2026-84783 | Moderate | OpenSSL 4.0 concurrent X509 cache; not affected on 3.6/3.5 |
| CVE-2026-35189 | Low | Malicious TLS certificate; excessive memory / DoS |
| CVE-2026-35191 | Low | QUIC server without address validation; amplification |
| CVE-2026-42772 | Low | QUIC fragment reassembly; CPU DoS |
| CVE-2026-54872 | Low | Generic non-NIST curve signing; timing leakage |
| CVE-2026-54873 | Low | QUIC fragment retention; memory DoS |
| CVE-2026-54875 | Low | SM2 ARM64/RISC-V timing; not x64 artifacts |
| CVE-2026-72897 | Low | TLS server SSL_set_SSL_CTX with differing providers; memory access |
| CVE-2026-75804 | Low | QUIC stream flow control; memory DoS |
| CVE-2026-75805 | Low | CMP revocation with CSR; crash |
| CVE-2026-75806 | Low | Established DTLS AEAD session; unauthenticated termination |
| CVE-2026-77696 | Low | SM2 signatures; timing leakage |
| CVE-2026-84784 | Low | QUIC connection ID backlog; memory DoS |

Upstream fixes: 3.6.5 / 3.5.9; Debian can backport fixes under an older upstream number. Official advisory supplies qualitative severity, no CVSS score. No claim of remote compromise of CgPhone.

## Validation and release
RC3 is a prerelease. Publication requires successful Windows build, ctest, installer parity, Defender gate, SBOM/hash generation and Linux runtime validation. Fresh Asterisk/Neotel, physical audio, tray/taskbar and multi-session tests remain pending; earlier results do not validate newly built binaries.
Trellix behavior depends on organizational policies and unsigned binaries; no corporate logs, paths, IPs or exclusions are published. No new Trellix result is claimed.
Authenticode/SignPath signing remains pending; attestations are not Authenticode and SmartScreen can still warn.
Release SHA256SUMS.txt is generated from final attached bytes, not copied from RC2. Linux historical hashes remain unchanged. After any later signing, regenerate hashes and attestations.

Linux remains experimental: SIP password storage is not equivalent to Windows DPAPI protection. Use test accounts with restricted privileges and protect local configuration access.
