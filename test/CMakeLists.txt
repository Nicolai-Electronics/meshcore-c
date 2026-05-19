cmake_minimum_required(VERSION 3.5)
project(meshcore_c)

list(APPEND sources
        "../meshcore/packet.c"
        "../meshcore/payload/request.c"
        "../meshcore/payload/ack.c"
        "../meshcore/payload/advert.c"
        "../meshcore/payload/grp_txt.c"
        "crypto/sha256.c"
        "crypto/hmac_sha256.c"
        "crypto/aes.c"
        "ed25519/add_scalar.c"
        "ed25519/fe.c"
        "ed25519/ge.c"
        "ed25519/keypair.c"
        "ed25519/key_exchange.c"
        "ed25519/sc.c"
        "ed25519/seed.c"
        "ed25519/sha512.c"
        "ed25519/sign.c"
        "ed25519/verify.c"
        "../main.c")

add_executable(meshcore_c ${sources})

target_include_directories(
    meshcore_c PUBLIC
    ..
    ../meshcore
    ../meshcore/payload
    crypto
    ed25519
)
