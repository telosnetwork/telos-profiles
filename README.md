# telos-profiles
A Decentralized User Profile Service for the Telos Blockchain Network.

## Dependencies

* eosio.cdt
* nodeos, cleos, keosd

## Setup

To begin, navigate to the project directory: `telos-profiles/`

    mkdir build && mkdir build/profiles

    chmod +x build.sh

    chmod +x deploy.sh

The `profiles` contract has already been implemented and is now build-ready.

## Build

    ./build.sh profiles

## Deploy

    ./deploy.sh profiles account-name { mainnet | testnet | local }

### The Profiles Contract API can be found in the documentation [here](docs/ContractAPI.md).
