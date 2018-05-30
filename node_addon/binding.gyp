
{
    "targets": [
        {
            "target_name": "addon",
            "sources": [ "hello.cpp" ],
            "libraries": [
                "/home/aautushka/projects/bitshares-core/build/libraries/wallet/libgraphene_wallet.a",
                "/home/aautushka/projects/bitshares-core/build/libraries/chain/libgraphene_chain.a",
                "/home/aautushka/projects/bitshares-core/build/libraries/utilities/libgraphene_utilities.a",
                "/home/aautushka/projects/bitshares-core/build/libraries/app/libgraphene_app.a",
                "/home/aautushka/projects/bitshares-core/build/libraries/fc/libfc_debug.a",
                "/home/aautushka/projects/bitshares-core/build/libraries/libevm/libdevcore/libdevcore.a",
                "/home/aautushka/projects/bitshares-core/libraries/fc/vendor/secp256k1-zkp/.libs/libsecp256k1.a",
                "/opt/boost_1_63_0_fpic/lib/libboost_thread.a",
                "/opt/boost_1_63_0_fpic/lib/libboost_system.a",
                "/opt/boost_1_63_0_fpic/lib/libboost_coroutine.a",
                "/opt/boost_1_63_0_fpic/lib/libboost_context.a",
                "/opt/boost_1_63_0_fpic/lib/libboost_filesystem.a",
                "/opt/boost_1_63_0_fpic/lib/libboost_chrono.a",
                "-lgmp"
            ]
        }
    ]
}
