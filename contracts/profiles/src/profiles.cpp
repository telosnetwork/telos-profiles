#include <profiles.hpp>

//======================== config actions ========================

ACTION profiles::init(string contract_name, string contract_version, name initial_admin)
{
    //authenticate
    require_auth(get_self());

    //open config table
    config_singleton configs(get_self(), get_self().value);

    //validate
    check(!configs.exists(), "contract already initialized");
    check(is_account(initial_admin), "initial admin account does not exist");

    //build new config
    config new_config = {
        contract_name,
        contract_version,
        initial_admin
    };

    //set new config
    //ram payer: contract
    configs.set(new_config, get_self());
}

ACTION profiles::setversion(string new_version)
{
    //open configs singleton
    config_singleton configs(get_self(), get_self().value);
    auto conf = configs.get();

    //authenticate
    require_auth(conf.admin);

    //update contract version
    conf.contract_version = new_version;

    //set config
    configs.set(conf, get_self());
}

ACTION profiles::setadmin(name new_admin)
{
    //open configs singleton
    config_singleton configs(get_self(), get_self().value);
    auto conf = configs.get();

    //authenticate
    require_auth(conf.admin);

    //update admin
    conf.admin = new_admin;

    //set config
    configs.set(conf, get_self());
}

ACTION profiles::setdefavatar(string new_default_avatar)
{
    //open configs singleton
    config_singleton configs(get_self(), get_self().value);
    auto conf = configs.get();

    //authenticate
    require_auth(conf.admin);

    //update default avatar
    conf.default_avatar = new_default_avatar;

    //set config
    configs.set(conf, get_self());
}

//======================== profile actions ========================

ACTION profiles::newprofile(name account, optional<string> display_name, optional<string> avatar, optional<string> bio)
{
    //authenticate
    require_auth(account);

    //open profiles table, find profile
    profiles_table profs(get_self(), get_self().value);
    auto prof_itr = profs.find(account.value);

    //validate
    check(prof_itr == profs.end(), "profile already exists");

    //open configs singleton, get config
    config_singleton configs(get_self(), get_self().value);
    auto conf = configs.get();
    
    //emplace new profile
    //ram payer: contract
    profs.emplace(get_self(), [&](auto& col) {
        col.account_name = account;
        col.display_name = (display_name) ? *display_name : account.to_string();
        col.avatar = (avatar) ? *avatar : conf.default_avatar;
        col.bio = (bio) ? *bio : string("");
    });
}

ACTION profiles::editdisplay(name account, string new_display_name)
{
    //authenticate
    require_auth(account);

    //open profiles table, get profile
    profiles_table profs(get_self(), get_self().value);
    auto& prof = profs.get(account.value, "profile not found");

    //update profile
    profs.modify(prof, same_payer, [&](auto& col) {
        col.display_name = new_display_name;
    });
}

ACTION profiles::editavatar(name account, string new_avatar)
{
    //authenticate
    require_auth(account);

    //open profiles table, get profile
    profiles_table profs(get_self(), get_self().value);
    auto& prof = profs.get(account.value, "profile not found");

    //update profile
    profs.modify(prof, same_payer, [&](auto& col) {
        col.avatar = new_avatar;
    });
}

ACTION profiles::editbio(name account, string new_bio)
{
    //authenticate
    require_auth(account);

    //open profiles table, get profile
    profiles_table profs(get_self(), get_self().value);
    auto& prof = profs.get(account.value, "profile not found");

    //update profile
    profs.modify(prof, same_payer, [&](auto& col) {
        col.bio = new_bio;
    });
}

ACTION profiles::editstatus(name account, string new_status)
{
    //authenticate
    require_auth(account);

    //open profiles table, get profile
    profiles_table profs(get_self(), get_self().value);
    auto& prof = profs.get(account.value, "profile not found");

    //update profile
    profs.modify(prof, same_payer, [&](auto& col) {
        col.status = new_status;
    });
}

ACTION profiles::verify(name account)
{
    //authenticate
    require_auth(permission_level{get_self(), name("verify")});

    //open profiles table, get profile
    profiles_table profs(get_self(), get_self().value);
    auto& prof = profs.get(account.value, "profile not found");

    //update profile
    profs.modify(prof, same_payer, [&](auto& col) {
        col.is_verified = true;
    });
}

ACTION profiles::delprofile(name account)
{
    //authenticate
    require_auth(account);

    //open profiles table, get profile
    profiles_table profs(get_self(), get_self().value);
    auto& prof = profs.get(account.value, "profile not found");

    //erase profile
    profs.erase(prof);
}
