#pragma once

#include <contracts/atomicassets.hpp>

void nftminer::claim(eosio::name &owner, uint64_t asset_id)
{
    maintenance_check(owner);

    auto assets = atomicassets::get_assets(owner);
    auto asset_itr = assets.require_find(asset_id, "Asset not owned!");
    
    // NFT Schema Check
    
    auto schema_check_itr = std::find_if(config.params.allowed_schemas.begin(), config.params.allowed_schemas.end(),
                    [&collection_name = asset_itr->collection_name, &schema_name = asset_itr->schema_name](const auto &entry) -> bool {
                        return collection_name == entry.collection_name && schema_name == entry.schema_name; });

    eosio::check(schema_check_itr != config.params.allowed_schemas.end(), "Asset schema not supported!");
    
    
    // NFT Data Prep
    auto schema = atomicassets::get_schemas(asset_itr->collection_name).get(asset_itr->schema_name.value);
    auto template_obj = atomicassets::get_templates(asset_itr->collection_name).get(asset_itr->template_id);

    auto asset_obj = assets.get(asset_id);

    auto asset_data = atomicdata::deserialize(asset_obj.mutable_serialized_data, schema.format);
    auto template_data = atomicdata::deserialize(template_obj.immutable_serialized_data, schema.format);
    
    // Time Management
    eosio::time_point now = eosio::current_time_point();
    auto _time_att = asset_data.find(TIME_ATT);

    // First time claiming, no time stamp found
    if (_time_att == asset_data.end()){

        
        asset_data[TIME_ATT] = (uint32_t)now.sec_since_epoch();
    }

    // !First time claiming, time stamp found
    if (_time_att != asset_data.end()){

        // Check if enough time has passed
        auto _time_att_itr = std::get<uint32_t>(_time_att->second);
        uint32_t time_difference = now.sec_since_epoch() - _time_att_itr;
        eosio::check(time_difference > config.params.claim_delay , "Not enough time has passed!");

        asset_data[TIME_ATT] = (uint32_t)now.sec_since_epoch();
    }

    // Update Time Stamp Attribute
    eosio::action(eosio::permission_level{get_self(), eosio::name("active")}, atomicassets::ATOMICASSETS_ACCOUNT,
                eosio::name("setassetdata"), std::make_tuple(get_self(), owner, asset_id, asset_data)).send();

    // Empty Attribute Maps for NFTs that will be minted
    atomicdata::ATTRIBUTE_MAP immutable_data;
    atomicdata::ATTRIBUTE_MAP mutable_data;

    // Rarity Multiplier
    auto _rarity_att = template_data.find(RARI_ATT);
    auto _rarity_att_itr = std::get<std::string>(_rarity_att->second);

    // Gets # of mints from rarity map
    auto mint_count = config.params.rarity_multiplier.at(_rarity_att_itr);
    
    // Mineral Lookup, Element Find
    auto _mineral_att = template_data.find(MIN_ATT);
    auto _mineral_att_itr = std::get<std::string>(_mineral_att->second);

    auto mineral_nft = config.params.mineral_elements.at(_mineral_att_itr);

    // Loop over & mint Mineral Elements
    for (uint64_t count = 0; count < mint_count; count++){
        mint_template(owner, mineral_nft.collection_name, mineral_nft.schema_name, mineral_nft.template_id, immutable_data, mutable_data);
    }

    // Alt Rarity Lookup, Element Find
    auto _alt_att = template_data.find(ALT_ATT);
    if (_alt_att != template_data.end()){
        // Only perform if Alt Rarity Attribute is found
        auto _alt_att_itr = std::get<std::string>(_alt_att->second);
    
        auto alt_rarity_nft = config.params.alt_elements.at(_alt_att_itr);
        // Loop over & mint Alt Rarity Elements 
        for (uint64_t count = 0; count < mint_count; count++){
            mint_template(owner, alt_rarity_nft.collection_name, alt_rarity_nft.schema_name, alt_rarity_nft.template_id, immutable_data, mutable_data);
        }
    }
}