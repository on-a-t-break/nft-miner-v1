#pragma once

#include <math.h>
#include <cmath>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/transaction.hpp>

#include <contracts/atomicdata.hpp>

CONTRACT nftminer : public eosio::contract
{
  public:
    using eosio::contract::contract;

    struct collection_schema
    {
        eosio::name collection_name;
        eosio::name schema_name;
    };
    struct extended_template : collection_schema
    {
        int32_t template_id;
    };

    struct full_template : extended_template
    {
        uint64_t atomic;
        std::string element;
    };

    struct cfg_params
    {
        std::vector<collection_schema> allowed_schemas;
        
        std::map<std::string, full_template> mineral_elements;
        std::map<std::string, full_template> alt_elements;
        std::map<std::string, uint64_t> rarity_multiplier;
        
        uint32_t claim_delay = 60;
        
        std::vector<eosio::name> test_accs;
    };

    // Contract Actions
    [[eosio::action]] void init(std::string &memo);
    [[eosio::action]] void destruct(std::string &memo);
    [[eosio::action]] void maintenance(bool maintenance, std::string &memo);
    [[eosio::action]] void setparams(cfg_params &params, std::string &memo);

    // User Actions
    [[eosio::action]] void claim(eosio::name &owner, uint64_t asset_id);

  private:

    
    const std::vector<eosio::name> CONTRACT_ADDR = {eosio::name("tfmwnftminer")};

    // NFT Attributes
    const std::string TIME_ATT = "last_element"; // uint32_t
    const std::string RARI_ATT = "rarity"; // string
    const std::string MIN_ATT = "mineral"; // string
    const std::string ALT_ATT = "alternate rarity"; // string

    // Memos
    const std::string MIN_MEMO = "TFMW Mineral Element Reward!";
    const std::string ALT_MEMO = "TFMW Alternate Rarity Element Reward!";
    
    // Config Table
    struct [[eosio::table("config")]] _config_entity
    {
        bool maintenance = true;
        cfg_params params;
    };
    typedef eosio::singleton<eosio::name("config"), _config_entity> _config;

    _config get_config()
    {
        return _config(get_self(), get_self().value);
    }

    _config_entity config = get_config().get_or_default();

    void maintenance_check(eosio::name & owner);
    void has_contract_auth();
    void mint_template(eosio::name owner, eosio::name collection_name, eosio::name schema_name, int32_t template_id, 
            atomicdata::ATTRIBUTE_MAP immutable_data = {}, atomicdata::ATTRIBUTE_MAP mutable_data = {});

};
