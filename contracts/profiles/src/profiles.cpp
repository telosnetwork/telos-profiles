#include <profiles.hpp>

// ACTION profiles::delconf()
// {
//     config_singleton configs(get_self(), get_self().value);
//     auto conf = configs.get();
//     require_auth(conf.admin);
//     configs.remove();
// }

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

ACTION profiles::setlength(name length_name, uint32_t new_length)
{
    //open configs singleton
    config_singleton configs(get_self(), get_self().value);
    auto conf = configs.get();

    //authenticate
    require_auth(conf.admin);

    //match length setting by name
    switch (length_name.value)
    {
        case name("displayname").value :
            //update max display name length
            conf.max_display_name_length = new_length;
            break;
        case name("avatar").value :
            //update max avatar length
            conf.max_avatar_length = new_length;
            break;
        case name("bio").value :
            //update max bio length
            conf.max_bio_length = new_length;
        case name("status").value :
            //update max status length
            conf.max_status_length = new_length;
            break;
        default:
            check(false, "invalid length name");
            break;
    }

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

    //initialize
    string profile_display_name = (display_name) ? *display_name : account.to_string();
    string profile_avatar = (avatar) ? *avatar : conf.default_avatar;
    string profile_bio = (bio) ? *bio : string("");

    //validate
    check(profile_display_name.length() <= conf.max_display_name_length, "display name too long");
    check(profile_avatar.length() <= conf.max_avatar_length, "avatar image link too long");
    check(profile_bio.length() <= conf.max_bio_length, "bio too long");
    
    //emplace new profile
    //ram payer: contract
    profs.emplace(get_self(), [&](auto& col) {
        col.account_name = account;
        col.display_name = profile_display_name;
        col.avatar = profile_avatar;
        col.bio = profile_bio;
    });
}

ACTION profiles::editdisplay(name account, string new_display_name)
{
    //authenticate
    require_auth(account);

    //open profiles table, get profile
    profiles_table profs(get_self(), get_self().value);
    auto& prof = profs.get(account.value, "profile not found");

    //open configs singleton, get config
    config_singleton configs(get_self(), get_self().value);
    auto conf = configs.get();

    //validate
    check(new_display_name.length() <= conf.max_display_name_length, "display name too long");

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

    //open configs singleton, get config
    config_singleton configs(get_self(), get_self().value);
    auto conf = configs.get();

    //validate
    check(new_avatar.length() <= conf.max_avatar_length, "avatar image link too long");

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

    //open configs singleton, get config
    config_singleton configs(get_self(), get_self().value);
    auto conf = configs.get();

    //validate
    check(new_bio.length() <= conf.max_bio_length, "bio too long");

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

    //open configs singleton, get config
    config_singleton configs(get_self(), get_self().value);
    auto conf = configs.get();

    //validate
    check(new_status.length() <= conf.max_status_length, "status too long");

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

//======================== metadata actions ========================

ACTION profiles::writemeta(name writer, name account, string data)
{
    //authenticate
    require_auth(writer);

    //TODO: validate data length

    //open profiles table, get profile
    profiles_table profiles(get_self(), get_self().value);
    auto& prof = profiles.get(account.value, "profile not found");

    //open metadata table, find meta
    metadata_table metadata(get_self(), writer.value);
    auto meta_itr = metadata.find(account.value);

    //if metadata not found
    if (meta_itr == metadata.end()) {
        //emplace new metadata
        //ram payer: writer
        metadata.emplace(writer, [&](auto& col) {
            col.account = account;
            col.data = data;
        });
    } else {
        //overwrite existing metadata
        metadata.modify(*meta_itr, same_payer, [&](auto& col) {
            col.data = data;
        });
    }
}

ACTION profiles::delmeta(name writer, name account)
{
    //authenticate
    require_auth(writer);

    //open metadata table, find meta
    metadata_table metadata(get_self(), writer.value);
    auto& meta = metadata.get(account.value, "metadata not found");

    //delete metadata
    metadata.erase(meta);
}

//======================== connection actions ========================

ACTION profiles::connect(name from, name to)
{
    //authenticate
    require_auth(from);

    //validate
    check(has_profile(from), "from account has no profile to connect");
    check(has_profile(to), "to account has no profile to connect");

    //open connections table, get by_from index, search for existing connection
    connections_table connections(get_self(), get_self().value);
    auto conns_by_from = connections.get_index<"byfrom"_n>();
    auto by_from_itr = conns_by_from.lower_bound(from.value);
    
    //loop over connections
    while (by_from_itr != conns_by_from.end()) {
        //validate
        check(by_from_itr->to.value != to.value, "connection already exists");

        //iterate
        by_from_itr++;
    }

    //emplace new connection
    //ram payer: from
    connections.emplace(get_self(), [&](auto& col) {
        col.connection_id = connections.available_primary_key();
        col.from = from;
        col.to = to;
        col.connection_time = time_point_sec(current_time_point());
    });
}

ACTION profiles::disconnect(name from, name to)
{
    //authenticate
    require_auth(from);

    //open connections table, get by_from index, search for existing connection
    connections_table connections(get_self(), get_self().value);
    auto conns_by_from = connections.get_index<"byfrom"_n>();
    auto by_from_itr = conns_by_from.lower_bound(from.value);

    //validate
    check(by_from_itr != conns_by_from.end(), "no connections to disconnect");

    //initialize
    bool found = false;
    
    //loop over connections until found or conn.from != from
    while (!found) {
        //if at end of table
        if (by_from_itr == conns_by_from.end()) {
            break;
        }

        //if connection found
        if (by_from_itr->to.value == to.value) {
            connections.erase(*by_from_itr);
            found = true;
        }

        //iterate
        by_from_itr++;
    }
    
}

//======================== contract functions ========================

bool profiles::has_profile(name account)
{
    //open prifles table, find profile
    profiles_table profs(get_self(), get_self().value);
    auto prof_itr = profs.find(account.value);

    return prof_itr != profs.end();
}
