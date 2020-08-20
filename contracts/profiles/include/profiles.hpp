// The profiles contract creates, updates, and deletes profile content.
//
// @author Craig Branscom
// @contract profiles
// @version v0.1.0

#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>

using namespace std;
using namespace eosio;

CONTRACT profiles : public contract {

    public:

    profiles(name self, name code, datastream<const char*> ds) : contract(self, code, ds) {};
    ~profiles() {};

    //======================== config actions ========================

    //intitialize the contract
    //auth: self
    ACTION init(string contract_name, string contract_version, name initial_admin);

    //set a new contract version
    //auth: admin
    ACTION setversion(string new_version);

    //set a new admin
    //auth: admin
    ACTION setadmin(name new_admin);

    //set a new default avatar
    //auth: admin
    ACTION setdefavatar(string new_default_avatar);

    //======================== profile actions ========================

    //create a new profile
    //auth: account
    ACTION newprofile(name account, optional<string> display_name, optional<string> avatar, optional<string> bio);

    //edit a display name
    //auth: account
    ACTION editdisplay(name account, string new_display_name);

    //edit a profile avatar
    //auth: account
    ACTION editavatar(name account, string new_avatar);

    //edit a profile bio
    //auth: account
    ACTION editbio(name account, string new_bio);

    //edit a profile status
    //auth: account
    ACTION editstatus(name account, string new_status);

    //verify a profile
    //auth: contract@verify
    ACTION verify(name account);

    //delete a profile
    //auth: account
    ACTION delprofile(name account);

    //======================== contract tables ========================

    //config table
    //scope: self
    TABLE config {
        string contract_name;
        string contract_version;
        name admin;
        string default_avatar;

        EOSLIB_SERIALIZE(config, (contract_name)(contract_version)(admin)(default_avatar))
    };
    typedef singleton<name("config"), config> config_singleton;

    //profiles table
    //scope: self
    TABLE profile {
        name account_name;
        string display_name = "";
        string avatar = "";
        string bio = "";
        string status = "";
        bool is_verified = false;

        uint64_t primary_key() const { return account_name.value; }

        EOSLIB_SERIALIZE(profile, (account_name)(display_name)(avatar)
            (bio)(status)(is_verified))
    };
    typedef multi_index<name("profiles"), profile> profiles_table;

};