// The profiles contract creates, updates, and deletes profile content.
//
// @author Craig Branscom
// @contract profiles
// @version v0.3.0

#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>

using namespace std;
using namespace eosio;

CONTRACT profiles : public contract {

    public:

    profiles(name self, name code, datastream<const char*> ds) : contract(self, code, ds) {};
    ~profiles() {};

    // ACTION delconf();

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

    //set new max metadata character limit
    //auth: admin
    ACTION setlength(name length_name, uint32_t new_length);

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

    //======================== metadata actions ========================

    //write new metadata for a profile
    //auth: writer
    ACTION writemeta(name writer, name account, string data);

    //delete metadata from a profile
    //auth: writer
    ACTION delmeta(name writer, name account);

    //======================== connection actions ========================

    //make a new profile connection
    //auth: from
    ACTION connect(name from, name to);

    //remove a profile connection
    //auth: from
    ACTION disconnect(name from, name to);

    //======================== contract functions ========================

    //retruns true of account has a profile
    bool has_profile(name account);

    //======================== contract tables ========================


    //config table
    //scope: self
    //ram payer: contract
    TABLE config {
        string contract_name;
        string contract_version;
        name admin;
        string default_avatar; //https://i.imgur.com/kZypAmC.png
        uint32_t max_display_name_length = 16;
        uint32_t max_avatar_length = 64;
        uint32_t max_bio_length = 256;
        uint32_t max_status_length = 16;

        EOSLIB_SERIALIZE(config, (contract_name)(contract_version)(admin)
            (default_avatar)(max_display_name_length)(max_avatar_length)
            (max_bio_length)(max_status_length))
    };
    typedef singleton<"config"_n, config> config_singleton;

    //profiles table
    //scope: self
    //ram payer: contract
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
    typedef multi_index<"profiles"_n, profile> profiles_table;

    //metadata table
    //scope: writer
    //ram payer: writer
    TABLE metadatum {
        name account;
        string data; //json

        uint64_t primary_key() const { return account.value; }

        EOSLIB_SERIALIZE(metadatum, (account)(data))
    };
    typedef multi_index<"metadata"_n, metadatum> metadata_table;

    //connections table
    //scope: self
    //ram payer: from
    TABLE connection {
        uint64_t connection_id;
        name from;
        name to;
        time_point_sec connection_time;
        
        uint64_t primary_key() const { return connection_id; }
        uint64_t by_from() const { return from.value; }
        uint64_t by_to() const { return to.value; }
        uint64_t by_time() const { return static_cast<uint64_t>(connection_time.utc_seconds); }

        EOSLIB_SERIALIZE(connection, (connection_id)(from)(to)(connection_time))
    };
    typedef multi_index<"connections"_n, connection,
        indexed_by<name("byfrom"), const_mem_fun<connection, uint64_t, &connection::by_from>>,
        indexed_by<name("byto"), const_mem_fun<connection, uint64_t, &connection::by_to>>,
        indexed_by<name("bytime"), const_mem_fun<connection, uint64_t, &connection::by_time>>
    > connections_table;

};