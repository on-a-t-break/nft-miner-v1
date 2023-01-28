#pragma once

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

#include <contracts/atomicdata.hpp>

namespace atomicassets
{
    static constexpr eosio::name ATOMICASSETS_ACCOUNT = eosio::name("atomicassets");

    struct schemas_s
    {
        eosio::name schema_name;
        std::vector<atomicdata::FORMAT> format;

        uint64_t primary_key() const { return schema_name.value; }
    };

    struct templates_s
    {
        int32_t template_id;
        eosio::name schema_name;
        bool transferable;
        bool burnable;
        uint32_t max_supply;
        uint32_t issued_supply;
        std::vector<uint8_t> immutable_serialized_data;

        uint64_t primary_key() const { return (uint64_t)template_id; }
    };

    typedef multi_index<eosio::name("templates"), templates_s> templates_t;

    struct assets_s
    {
        uint64_t asset_id;
        eosio::name collection_name;
        eosio::name schema_name;
        int32_t template_id;
        eosio::name ram_payer;
        std::vector<eosio::asset> backed_tokens;
        std::vector<uint8_t> immutable_serialized_data;
        std::vector<uint8_t> mutable_serialized_data;

        uint64_t primary_key() const { return asset_id; };
    };

    typedef eosio::multi_index<eosio::name("schemas"), schemas_s> schemas_t;
    typedef eosio::multi_index<eosio::name("assets"), assets_s> assets_t;

    schemas_t get_schemas(eosio::name collection_name)
    {
        return schemas_t(ATOMICASSETS_ACCOUNT, collection_name.value);
    }

    templates_t get_templates(eosio::name collection_name)
    {
        return templates_t(ATOMICASSETS_ACCOUNT, collection_name.value);
    }

    assets_t get_assets(eosio::name acc)
    {
        return assets_t(ATOMICASSETS_ACCOUNT, acc.value);
    }
}