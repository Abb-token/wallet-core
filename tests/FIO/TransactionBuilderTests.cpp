// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "FIO/Action.h"
#include "FIO/Transaction.h"
#include "FIO/TransactionBuilder.h"

#include "HexCoding.h"

#include <gtest/gtest.h>

#include <iostream>

using namespace TW;
using namespace TW::FIO;
using namespace std;


const Data chainId = parse_hex("4e46572250454b796d7296eec9e8896327ea82dd40f2cd74cf1b1d8ba90bcd77");
// 5KEDWtAUJcFX6Vz38WXsAQAv2geNqT7UaZC8gYu9kTuryr3qkri FIO6m1fMdTpRkRBnedvYshXCxLFiC5suRU8KDfx8xxtXp2hntxpnf
const PrivateKey privKeyBA = PrivateKey(parse_hex("ba0828d5734b65e3bcc2c51c93dfc26dd71bd666cc0273adee77d73d9a322035"));
const PublicKey pubKey6M = privKeyBA.getPublicKey(TWPublicKeyTypeSECP256k1);
const Address addr6M(pubKey6M);

TEST(FIOTransactionBuilder, RegFioAddress) {
    ChainParams chainParams{chainId, 39881, 4279583376};
    uint64_t maxFee = 5000000000;

    string t = TransactionBuilder::createRegFioAddress(addr6M, privKeyBA, "adam@fiotestnet", addr6M.string(),
        chainParams, maxFee, TransactionBuilder::WalletFioName, 1579784511);

    EXPECT_EQ(R"({
"signatures": ["SIG_K1_K19ugLriG3ApYgjJCRDsy21p9xgsjbDtqBuZrmAEix9XYzndR1kNbJ6fXCngMJMAhxUHfwHAsPnh58otXiJZkazaM1EkS5"],
"compression": "none",
"packed_context_free_data": "",
"packed_trx": "3f99295ec99b904215ff0000000001003056372503a85b0000c6eaa66498ba01102b2f46fca756b200000000a8ed3232650f6164616d4066696f746573746e65743546494f366d31664d645470526b52426e6564765973685843784c4669433573755255384b44667838787874587032686e7478706e6600f2052a01000000102b2f46fca756b20e726577617264734077616c6c657400"
})", t);
}

TEST(FIOTransactionBuilder, AddPubAddress) {
    ChainParams chainParams{chainId, 11565, 4281229859};

    string t = TransactionBuilder::createAddPubAddress(addr6M, privKeyBA, "adam@fiotestnet", {
        {"BTC", "bc1qvy4074rggkdr2pzw5vpnn62eg0smzlxwp70d7v"},
        {"ETH", "0xce5cB6c92Da37bbBa91Bd40D4C9D4D724A3a8F51"},
        {"BNB", "bnb1ts3dg54apwlvr9hupv2n0j6e46q54znnusjk9s"}},
        chainParams, 0, TransactionBuilder::WalletFioName, 1579729429);

    EXPECT_EQ(R"({
"signatures": ["SIG_K1_K85BxXzJwvjPs3mFeKatWSjBHuMXTw634RRtf6ZMytpzLCdpHcJ7CQWPeXJvwm7aoz7XJJKapmoT4jzCLoVBv2cxP149Bx"],
"compression": "none",
"packed_context_free_data": "",
"packed_trx": "15c2285e2d2d23622eff0000000001003056372503a85b0000c6eaa664523201102b2f46fca756b200000000a8ed3232bd010f6164616d4066696f746573746e657403034254432a626331717679343037347267676b647232707a773576706e6e3632656730736d7a6c7877703730643776034554482a30786365356342366339324461333762624261393142643430443443394434443732344133613846353103424e422a626e6231747333646735346170776c76723968757076326e306a366534367135347a6e6e75736a6b39730000000000000000102b2f46fca756b20e726577617264734077616c6c657400"
})", t);
}

TEST(FIOTransaction, ActionRegFioAddressInternal) {
    RegFioAddressData radata("adam@fiotestnet", addr6M.string(),
        5000000000, "rewards@wallet", "qdfejz2a5wpl");
    Data ser1;
    radata.serialize(ser1);
    EXPECT_EQ(
        hex(parse_hex("0F6164616D4066696F746573746E65743546494F366D31664D645470526B52426E6564765973685843784C4669433573755255384B44667838787874587032686E7478706E6600F2052A01000000102B2F46FCA756B20E726577617264734077616C6C6574")),
        hex(ser1));

    Action raAction;
    raAction.account = "fio.address";
    raAction.name = "regaddress";
    raAction.includeExtra01BeforeData = false;
    raAction.actionDataSer = ser1;
    raAction.auth.authArray.push_back(Authorization{"qdfejz2a5wpl", "active"});
    Data ser2;
    raAction.serialize(ser2);
    EXPECT_EQ(
        "003056372503a85b0000c6eaa66498ba0"
        "1102b2f46fca756b200000000a8ed3232"
        "65"
        "0f6164616d4066696f746573746e65743546494f366d31664d645470526b52426e6564765973685843784c4669433573755255384b44667838787874587032686e7478706e6600f2052a01000000102b2f46fca756b20e726577617264734077616c6c657400",
        hex(ser2));

    Transaction tx;
    tx.expiration = 1579784511;
    tx.refBlockNumber = 39881;
    tx.refBlockPrefix = 4279583376;
    tx.actions.push_back(raAction);
    Data ser3;
    tx.serialize(ser3);
    EXPECT_EQ(
        "3f99295ec99b904215ff0000000001"
        "003056372503a85b0000c6eaa66498ba0"
        "1102b2f46fca756b200000000a8ed3232"
        "65"
        "0f6164616d4066696f746573746e65743546494f366d31664d645470526b52426e6564765973685843784c4669433573755255384b44667838787874587032686e7478706e6600f2052a01000000102b2f46fca756b20e726577617264734077616c6c657400",
        hex(ser3));
}

TEST(FIOTransaction, ActionAddPubAddressInternal) {
    AddPubAddressData aadata("adam@fiotestnet", {
        {"BTC", "bc1qvy4074rggkdr2pzw5vpnn62eg0smzlxwp70d7v"},
        {"ETH", "0xce5cB6c92Da37bbBa91Bd40D4C9D4D724A3a8F51"},
        {"BNB", "bnb1ts3dg54apwlvr9hupv2n0j6e46q54znnusjk9s"}},
        0, "rewards@wallet", "qdfejz2a5wpl");
    Data ser1;
    aadata.serialize(ser1);
    EXPECT_EQ(
        "0f6164616d4066696f746573746e657403034254432a626331717679343037347267676b647232707a773576706e6e3632656730736d7a6c7877703730643776034554482a30786365356342366339324461333762624261393142643430443443394434443732344133613846353103424e422a626e6231747333646735346170776c76723968757076326e306a366534367135347a6e6e75736a6b39730000000000000000102b2f46fca756b20e726577617264734077616c6c6574",
        hex(ser1));

    Action aaAction;
    aaAction.account = "fio.address";
    aaAction.name = "addaddress";
    aaAction.includeExtra01BeforeData = true;
    aaAction.actionDataSer = ser1;
    aaAction.auth.authArray.push_back(Authorization{"qdfejz2a5wpl", "active"});
    Data ser2;
    aaAction.serialize(ser2);
    EXPECT_EQ(
        "003056372503a85b0000c6eaa66452320"
        "1102b2f46fca756b200000000a8ed3232"
        "bd01"
        "0f6164616d4066696f746573746e657403034254432a626331717679343037347267676b647232707a773576706e6e3632656730736d7a6c7877703730643776034554482a30786365356342366339324461333762624261393142643430443443394434443732344133613846353103424e422a626e6231747333646735346170776c76723968757076326e306a366534367135347a6e6e75736a6b39730000000000000000102b2f46fca756b20e726577617264734077616c6c657400",
        hex(ser2));

    Transaction tx;
    tx.expiration = 1579729429;
    tx.refBlockNumber = 11565;
    tx.refBlockPrefix = 4281229859;
    tx.actions.push_back(aaAction);
    Data ser3;
    tx.serialize(ser3);
    EXPECT_EQ(
        "15c2285e2d2d23622eff0000000001"
        "003056372503a85b0000c6eaa66452320"
        "1102b2f46fca756b200000000a8ed3232"
        "bd01"
        "0f6164616d4066696f746573746e657403034254432a626331717679343037347267676b647232707a773576706e6e3632656730736d7a6c7877703730643776034554482a30786365356342366339324461333762624261393142643430443443394434443732344133613846353103424e422a626e6231747333646735346170776c76723968757076326e306a366534367135347a6e6e75736a6b39730000000000000000102b2f46fca756b20e726577617264734077616c6c657400",
        hex(ser3));
}
