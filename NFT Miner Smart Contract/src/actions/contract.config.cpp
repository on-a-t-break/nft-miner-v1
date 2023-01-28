#include <contracts/atomicassets.hpp>

void nftminer::init(std::string &memo)
{
    has_contract_auth();
    get_config().set(_config_entity{}, get_self());
}

void nftminer::destruct(std::string &memo)
{
    has_contract_auth();
    get_config().remove();
}

void nftminer::maintenance(bool maintenance, std::string &memo)
{
    has_contract_auth();

    auto _config = get_config();
    auto new_config = _config.get();

    new_config.maintenance = maintenance;

    _config.set(new_config, get_self());
}

void nftminer::setparams(cfg_params &params, std::string &memo)
{
    has_contract_auth();

    auto _config = get_config();
    auto new_config = _config.get();

    new_config.params = params;

    _config.set(new_config, get_self());
}

