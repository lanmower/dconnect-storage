# dconnect-storage
eosio storage smart contract for the dconnect network 

build:
eosio-cpp eosio_storage.cpp -o eosio_storage.wasm

apply contract with cleos:
cleos set contract <contract name> ./

ACTIONS
precede each action with the users name

(replace the bits in <> brackets with your own information)

cleos push action <contract name> set '[ "<user name>","<table key>","<value>" ]' -p <user name>@active
# SET - create an entry
username - freedom first username
key - username type index for path/key separation
value - value to apply

# TIP: You will get the primary key for an entry when you read it...

cleos push action <contract name> edit '[ "<user name>","<primary key>" ]' -p <user name>@active
# EDIT - modify an entry
primary - primary key of item
value - value to apply

cleos push action <contract name> erase '[ "<user name>","<primary key>" ]' -p <user name>@active
# ERASE - erase an entry
primary - primary key of item
value - value to apply

You can read items either from global scope, or scoped to the user as follows
cleos -u https://dconnect.live get table <contract name> <contract name> post

You can also read items from user scope (look at this as the users profile)
cleos -u https://dconnect.live get table <contract name> <user name> post

