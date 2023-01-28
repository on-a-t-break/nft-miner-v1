#include <nftminer.hpp>

#include <math.h>

#include "actions/contract.config.cpp"

#include "actions/user.cpp"

void nftminer::has_contract_auth()
{
    auto contract = get_self();

    require_auth(contract);

    auto contract_itr =
        std::find_if(CONTRACT_ADDR.begin(), CONTRACT_ADDR.end(),
                     [&contract = contract](const eosio::name &name) -> bool { return name == contract; });

    eosio::check(contract_itr->value == contract.value, "Not authorized!");
}

void nftminer::maintenance_check(eosio::name &owner)
{
    auto tester_itr = std::find(config.params.test_accs.begin(), config.params.test_accs.end(), owner);

    if (tester_itr == config.params.test_accs.end())
    {
        eosio::check(!config.maintenance, "Maintenance");
    }
}

void nftminer::mint_template(eosio::name owner, eosio::name collection_name, eosio::name schema_name, int32_t template_id, 
        atomicdata::ATTRIBUTE_MAP immutable_data, atomicdata::ATTRIBUTE_MAP mutable_data)
{
    eosio::action(eosio::permission_level{get_self(), eosio::name("active")}, atomicassets::ATOMICASSETS_ACCOUNT,eosio::name("mintasset"),
            std::make_tuple(get_self(), collection_name, schema_name, template_id, owner, immutable_data, mutable_data, (std::vector<eosio::asset>){})).send();
}